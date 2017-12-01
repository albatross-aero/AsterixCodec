#include "JsonCategoriesDescriptionLoader.h"

using namespace std;

// Declaration of static fields
CommonVariables* JsonCategoriesDescriptionLoader::commonVariables = CommonVariables::getInstance();

void JsonCategoriesDescriptionLoader::getCategoriesDescriptionMapFromJSONFile(string &astConfigurationFilePath,
		unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status) {

	/************************
	 * Variable definitions *
	 ************************/
	autojsoncxx::ParsingResult result;
	AsterixConfiguration currentAsterixConfiguration;

	struct stat buffer;

	/***********************************************
	 * Reading of 'AsterixConfiguration.json' file *
	 ***********************************************/
	if (stat(astConfigurationFilePath.c_str(), &buffer) != 0) {    // Check if the path exists on the file system
		string message = "ASTERIX configuration files: " + astConfigurationFilePath + " does not exist";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return;
	}

	if (!autojsoncxx::from_json_file(astConfigurationFilePath, currentAsterixConfiguration, result)) {    // Try to load the JSON file
		string message = "Error while reading ASTERIX Configuration file: '" + astConfigurationFilePath + "', it is not a valid JSON.\n" + result.description()
																																																						+ '\n';
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	/***************************************************
	 *  Retrieving all informations for ALL categories *
	 ***************************************************/
	composeCategoriesDescriptionMapFromJsonFiles(currentAsterixConfiguration, astConfigurationFilePath, uapMap, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	// Now set the Categories Description to encode/decode (not filtered)
	loadCategoriesFilter(currentAsterixConfiguration.getCategoriesFilterFile(), uapMap, status);
	if (status->getCode() != ReturnCodes::SUCCESS && status->getCode() != ReturnCodes::WARNING) {
		return;
	}

	if (commonVariables->isDebugEnabledForDecoder()) {
		//vector<string> keys((*uapMap).size());
		for (auto kv : (*uapMap)) {
			cout << "Key: " << kv.first;
			cout << std::boolalpha << " - ToDecode: " << kv.second->isToBeDecoded() << " - ToEncode: " << kv.second->isToBeEncoded() << std::dec << endl;
		}
	}
	// ASTERIX Description are loaded without validation errors - Codec can start
}

void JsonCategoriesDescriptionLoader::composeCategoriesDescriptionMapFromJsonFiles(AsterixConfiguration currentAsterixConfiguration,
		string astConfigurationFilePath, unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status) {
	/************************
	 * Variable definitions *
	 ************************/
	unsigned int numberOfAsterixCategoriesSupported = 0;
	unsigned int sizeOfDataSourcesUsingThisDescription = 0;
	int categoryNumber = 0;
	CategoryDescription* currentCategoryDescription = nullptr;
	string categoryNumberString;    // contain the Category number as a string
	stringstream ss;
	int catAndSacSicInDecimal;

	numberOfAsterixCategoriesSupported = currentAsterixConfiguration.getCategoriesConfiguration().size();

	/*************************************************
	 *  Retrieving all informations for one category *
	 *************************************************/
	for (unsigned int i = 0; i < numberOfAsterixCategoriesSupported; i++) {
		currentCategoryDescription = new CategoryDescription();

		categoryNumber = currentAsterixConfiguration.getCategoriesConfiguration()[i].getCategoryNumber();

		if (categoryNumber <= 0) {
			stringstream message;
			message << "Category number is: " << categoryNumber << ". This filed must be > 0" << std::endl << "Error while reading ASTERIX Configuration file: "
					<< astConfigurationFilePath << std::endl;
			status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
			status->setMessage(message.str());
			return;
		}

		composeCategoryDescriptionFromJsonFiles(astConfigurationFilePath, currentAsterixConfiguration.getCategoriesConfiguration()[i], categoryNumber, currentCategoryDescription,
				status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}

		/**********************************************************************
		 *  Save the new category description into the map of descriptions *
		 **********************************************************************/

		// If Description is set to default, it gets only the category number the key in the map of Asterix Description
		if (currentAsterixConfiguration.getCategoriesConfiguration()[i].isCategoryByDefault()) {
			//Check if the category exists already
			if (JsonCategoriesDescriptionValidator::checkIfKeyInCategoryDescriptionMapExists(categoryNumber, uapMap)) {
				stringstream message;
				message << "Category already declared: " << categoryNumber << ".\nError while reading ASTERIX Configuration file: " << astConfigurationFilePath
						<< endl;
				status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
				status->setMessage(message.str());
				return;
			} else {
				// Add new Category description into a map because it does not exist
				(*uapMap).insert(pair<int, CategoryDescription*>(categoryNumber, currentCategoryDescription));
			}
		}

		// If there are data sources specified for this description, in the Asterix Description map they get a key built as
		// {<CategoryNumber><SAC><SIC>} (e.g. 2112053, where CAT=21, SAC=120 and SIC=53)
		sizeOfDataSourcesUsingThisDescription = currentAsterixConfiguration.getCategoriesConfiguration()[i].getDataSourcesInDec().size();

		if (sizeOfDataSourcesUsingThisDescription > 0) {

			for (unsigned int j = 0; j < sizeOfDataSourcesUsingThisDescription; j++) {

				catAndSacSicInDecimal = JsonCategoriesDescriptionValidator::splitDatasourceString(
						currentAsterixConfiguration.getCategoriesConfiguration()[i].getDataSourcesInDec()[j], categoryNumber, astConfigurationFilePath, status);

				if (status->getCode() != ReturnCodes::SUCCESS) {
					return;
				}

				//Check if the category-datasource exists already
				if (JsonCategoriesDescriptionValidator::checkIfKeyInCategoryDescriptionMapExists(catAndSacSicInDecimal, uapMap)) {
					std::stringstream message;
					message << "Data source already declared " << currentAsterixConfiguration.getCategoriesConfiguration()[i].getDataSourcesInDec()[j]
																																					<< " for Category " << categoryNumber << "." << std::endl << "Error while reading ASTERIX Configuration file: "
																																					<< astConfigurationFilePath << std::endl;
					status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
					status->setMessage(message.str());
					return;
				} else {
					// Add new Category description into a map because it does not exist
					(*uapMap).insert(pair<int, CategoryDescription*>(catAndSacSicInDecimal, currentCategoryDescription));
				}
			}
		}
	}
}

void JsonCategoriesDescriptionLoader::composeCategoryDescriptionFromJsonFiles(string astConfigurationFilePath, CategoriesConfiguration categoriesConfiguration,
		int categoryNumber, CategoryDescription* categoryDescription, ReturnStatus* status) {

	/************************
	 * Variable definitions *
	 ************************/
	string uapDescriptionPath = categoriesConfiguration.getUapAndMandatoryItemsFile();
	string itemsDescriptionPath = categoriesConfiguration.getItemsDescriptionFile();
	string refDescriptionPath = categoriesConfiguration.getRefDescriptionFile();
	string spfDescriptionPath = categoriesConfiguration.getSpfDescriptionFile();
	string filterPath = categoriesConfiguration.getFilterFile();

	UapAndMandatoryItems uapAndMandatoryItems;
	CategoryItemsFilters categoryItemsFilters;

	ExternalCategoryDescription itemsDescription;
	ExternalCategoryDescription reItemsDescription;
	ExternalCategoryDescription spItemsDescription;


	/*******************************************
	 * Reading of UAP and mandatory Items list *
	 *******************************************/
	loadUapAndMandatoryItemsList(astConfigurationFilePath, uapDescriptionPath, uapAndMandatoryItems, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	vector<string> uap = uapAndMandatoryItems.getUapList();

	/********************************
	 * Reading of Items Description *
	 ********************************/
	loadItemsDescription(astConfigurationFilePath, itemsDescriptionPath, itemsDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	/***************************************************
	 * Reading of Reserved Expansion Field Description *
	 ***************************************************/
	loadREForSPFDescription(refDescriptionPath, reItemsDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	/***************************************************
	 * Reading of Special Purpose Field Description *
	 ***************************************************/
	loadREForSPFDescription(spfDescriptionPath, spItemsDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	/*******************************************************************
	 * MARSHALLING from the extCategoryDescription to the internal one *
	 *******************************************************************/
	performMarshallingFromExternalToInternalDescription(itemsDescription, reItemsDescription, spItemsDescription, uapAndMandatoryItems, categoryDescription,
			status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	/********************************************************
	 * Put the items description in order following the UAP *
	 ********************************************************/
	putInOrderItemsDescription(uapDescriptionPath, uap, categoryDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	// Filling the 'mandatoryItemFRNs' structure of Category Description class
	unsigned int FRN_mandatoryItem = 0;
	for (auto mandatoryItemName : uapAndMandatoryItems.getMandatoryItems()) {
		vector<string>::iterator mandatoryItemName_it = find(uap.begin(), uap.end(), mandatoryItemName);
		if (mandatoryItemName_it != uap.end()) {
			FRN_mandatoryItem = (mandatoryItemName_it - uap.begin()) + 1;    // +1 is required due to the fact that FRN starts from 1
			categoryDescription->getMandatoryItemFRNs()->push_back(FRN_mandatoryItem);
		}
	}

	/****************************************
	 * VALIDATION of JSON description files *
	 ****************************************/
	JsonCategoriesDescriptionValidator::performValidationOfJsonFiles(itemsDescriptionPath, refDescriptionPath, spfDescriptionPath, categoryDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	/***************************************************
	 * Load FILTERS for current CategoryDescription *
	 ***************************************************/
	loadItemFilters(filterPath, categoryDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	/******************************************************************************************************
	 * Setup of enum values for 'itemType', 'functionNameForDecoding' and 'representationModeForDecoding' *
	 ******************************************************************************************************/
	categoryDescription->convertInformationFromStringToEnum();

	/********************************************************
	 * Filling the Elements MAP of the Category just loaded *
	 ********************************************************/
	categoryDescription->fillElementsMap();
}

// ** Load description files **
void JsonCategoriesDescriptionLoader::loadCategoriesFilter(string categoriesFilterPath, unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status){
	autojsoncxx::ParsingResult result;
	ifstream categoriesFilterFile;
	CategoriesFilters categoriesFilters;

	// Checking if the file exists - filterPath is optional, so the status shall not be set
	if (categoriesFilterPath == "")
		return;

	categoriesFilterFile.open(categoriesFilterPath);
	if (!categoriesFilterFile) {
		string message = "Error opening '" + categoriesFilterPath + "' file.\n";
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	if (!autojsoncxx::from_json_file(categoriesFilterPath, categoriesFilters, result)) {
		string message = "Error while reading '" + categoriesFilterPath + "' file, it is not a valid JSON." + '\n' + result.description() + '\n';
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;

	} else {
		checkAndSetFiltersForAllCategoriesDescription(categoriesFilterPath, categoriesFilters, uapMap, status);
		if (status->getCode() != ReturnCodes::SUCCESS && status->getCode() != ReturnCodes::WARNING) {
			return;
		}
	}
}

void JsonCategoriesDescriptionLoader::loadUapAndMandatoryItemsList(string astConfigurationFilePath, string uapDescriptionPath,
		UapAndMandatoryItems& uapAndMandatoryItems, ReturnStatus* status) {
	autojsoncxx::ParsingResult result;
	ifstream uapDescriptionFile;

	// Checking if the file exists - uapDescriptionPath is mandatory
	if (uapDescriptionPath == "") {
		string message =
				"JSON file missing: 'uapAndMandatoryItemsFile' filed is mandatory, please add the path for that file.\nError while reading files from ASTERIX Configuration: "
				+ astConfigurationFilePath + "\n";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return;
	}

	uapDescriptionFile.open(uapDescriptionPath);
	if (!uapDescriptionFile) {
		string message = "Error opening " + uapDescriptionPath + ". Check or add the file path because it is mandatory.\n";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return;
	}
	// Now it is time to load the UAP JSON content
	if (!autojsoncxx::from_json_file(uapDescriptionPath, uapAndMandatoryItems, result)) {
		string message = "Error while reading '" + uapDescriptionPath + "' file, it is not a valid JSON." + '\n' + result.description() + '\n';
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	// Check the correctness of file and remove FX string from the UAP list
	JsonCategoriesDescriptionValidator::checkAndPrepareUapAndMandatoryItemsList(uapDescriptionPath, uapAndMandatoryItems, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}
}

void JsonCategoriesDescriptionLoader::loadItemsDescription(string astConfigurationFilePath, string itemsDescriptionPath,
		ExternalCategoryDescription& extCategoryDescription, ReturnStatus* status) {
	ifstream itemsJsonDescription;
	autojsoncxx::ParsingResult result;

	// Checking if the file exists - itemsDescriptionPath is mandatory
	if (itemsDescriptionPath == "") {
		string message =
				"JSON file missing: 'itemsDescription' field is a mandatory, please add the path for that file.\nError while reading files from ASTERIX Configuration: "
				+ astConfigurationFilePath + "\n";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return;
	}

	itemsJsonDescription.open(itemsDescriptionPath);

	if (!itemsJsonDescription) {
		string message = "Error opening " + itemsDescriptionPath + ". Check or add the file path because it is mandatory.\n";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return;
	}

	// Now it is time to load the Category JSON description content
	if (!autojsoncxx::from_json_file(itemsDescriptionPath, extCategoryDescription, result)) {
		string message = "Error while reading '" + itemsDescriptionPath + "' file, it is not a valid JSON." + '\n' + result.description() + '\n';
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	JsonCategoriesDescriptionValidator::checkAndPrepareExternalItemsDescription(itemsDescriptionPath, extCategoryDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}
}

void JsonCategoriesDescriptionLoader::loadREForSPFDescription(string descriptionPath, ExternalCategoryDescription& itemsDescription, ReturnStatus* status) {
	ifstream itemsJsonDescription;
	autojsoncxx::ParsingResult result;

	// Checking if the file exists - itemsDescriptionPath is mandatory
	if (descriptionPath == "") {
		return;
	}

	itemsJsonDescription.open(descriptionPath);

	if (!itemsJsonDescription) {
		string message = "Error opening '" + descriptionPath + "' file.\n";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return;
	}

	// Now it is time to load the JSON description content
	if (!autojsoncxx::from_json_file(descriptionPath, itemsDescription, result)) {
		string message = "Error while reading '" + descriptionPath + "' file, it is not a valid JSON." + '\n' + result.description() + '\n';
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	JsonCategoriesDescriptionValidator::checkAndPrepareExternalItemsDescription(descriptionPath, itemsDescription, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}
}

void JsonCategoriesDescriptionLoader::loadItemFilters(string filterPath, CategoryDescription* categoryDescription, ReturnStatus* status) {
	autojsoncxx::ParsingResult result;
	ifstream filterFile;
	CategoryItemsFilters categoryItemsFilters;

	// Checking if the file exists - filterPath is optional
	if (filterPath == "")
		return;

	filterFile.open(filterPath);
	if (!filterFile) {
		string message = "Error opening '" + filterPath + "' file.\n";
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	if (!autojsoncxx::from_json_file(filterPath, categoryItemsFilters, result)) {
		string message = "Error while reading '" + filterPath + "' file, it is not a valid JSON." + '\n' + result.description() + '\n';
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;

	} else {
		checkAndSetFiltersForCategoryDescription(filterPath, categoryItemsFilters, categoryDescription, status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}
	}
}

void JsonCategoriesDescriptionLoader::putInOrderItemsDescription(string uapDescriptionPath, std::vector<std::string> uapList,
		CategoryDescription* categoryDescription, ReturnStatus* status) {
	/************************
	 * Variable definitions *
	 ************************/
	vector<Item>* itemCollectionToOrder = nullptr;
	vector<Item> orderedItemCollection;
	unsigned int itemCollectionToOrderSize = 0;
	int indexOfCurrentItemInUAP = 0;
	std::vector<std::string>::iterator itemNameInUAP;

	itemCollectionToOrder = categoryDescription->getItemCollection();
	itemCollectionToOrderSize = itemCollectionToOrder->size();

	orderedItemCollection = vector<Item>(uapList.size());

	//Scan each Item of 'itemCollectionToOrder' and looking for the Item name in 'uapList'
	for (unsigned int i = 0; i < itemCollectionToOrderSize; i++) {

		// If the current Item name not exists in the 'uapList' -> the current Item is not part of the final collection
		if ((itemNameInUAP = find(uapList.begin(), uapList.end(), itemCollectionToOrder->at(i).getName())) != uapList.end()) {

			// Current Item is present in the UAP -> puts it in 'orderedItemCollection' - The index of current items is given by the UAP vector
			// After usage, the name of current item in the 'uapList' is setted to "", in order to make some controls at the end of ordering.
			indexOfCurrentItemInUAP = itemNameInUAP - uapList.begin();
			orderedItemCollection[indexOfCurrentItemInUAP] = itemCollectionToOrder->at(i);
			uapList.at(indexOfCurrentItemInUAP) = "";
		}
	}

	//Create Item for the REF
	Item reItem;

	//If exists the description of REF, create the corresponding Item for the Item collection
	if (categoryDescription->getRefItemDescription()->size() > 0) {
		reItem = Item(kREF, 1, ItemTypeEnum::ExplicitLength);

		vector<int> elementsDescriptionOfREF;
		vector<string> elementsNameOfREF;
		Item itemOfREF;

		//Scan each itemOfREF of REF Description and put the index and the name in the 'elementsDescription' and 'elementsName', these two vectors will be part of the 'reItem' object
		for (unsigned int j = 0; j < categoryDescription->getRefItemDescription()->size(); j++) {
			itemOfREF = categoryDescription->getRefItemDescription()->at(j);

			elementsDescriptionOfREF.push_back(j);
			elementsNameOfREF.push_back(itemOfREF.getName());
		}

		reItem.setAllElementDescription(elementsDescriptionOfREF);
		reItem.setAllElementNames(elementsNameOfREF);

	} else {    //If NOT exists the description of REF, create a place-holder for the Item collection
		reItem = Item(kREF, 1, ItemTypeEnum::ExplicitLength);
	}

	//Create Item for the REF
	Item spItem;

	//If exists the description of SPF, create the corresponding Item for the Item collection
	if (categoryDescription->getSpfItemDescription()->size() > 0) {
		spItem = Item(kSPF, 1, ItemTypeEnum::ExplicitLength);

		vector<int> elementsDescriptionOfSPF;
		vector<string> elementsNameOfSPF;
		Item itemOfSPF;

		//Scan each item of SPF Description and put the index and the name in the 'elementsDescription' and 'elementsName', these two vectors will be part of the 'spItem' object
		for (unsigned int j = 0; j < categoryDescription->getSpfItemDescription()->size(); j++) {

			itemOfSPF = categoryDescription->getSpfItemDescription()->at(j);

			elementsDescriptionOfSPF.push_back(j);
			elementsNameOfSPF.push_back(itemOfSPF.getName());
		}

		spItem.setAllElementDescription(elementsDescriptionOfSPF);
		spItem.setAllElementNames(elementsNameOfSPF);

	} else {    //If NOT exists the description of SPF, create a place-holder for the Item collection
		spItem = Item(kSPF, 1, ItemTypeEnum::ExplicitLength);
	}

	//Create a place-holder for the Item witch name is "-", it is used like a place-holder for maintain the right value of FRNs in the UAP
	Item dashItem("-", 0, ItemTypeEnum::FixedLength);

	// Scan the 'uapList', if a Item name is:
	// 0) ""; Item already setted
	// 1) "-": put in that position the 'dashItem'
	// 2) kREF: put in that position the 'reItem'
	// 3) kSPF: put in that position the 'spItem'
	// otherwise -> error
	for (unsigned int i = 0; i < uapList.size(); i++) {
		if (uapList.at(i) == "-") {
			orderedItemCollection[i] = dashItem;

		} else if (uapList.at(i) == kREF) {
			orderedItemCollection[i] = reItem;

		} else if (uapList.at(i) == kSPF) {
			orderedItemCollection[i] = spItem;

		} else if (uapList.at(i) != "-" && uapList.at(i) != "") {
			string message = "Error, " + uapList.at(i) + " definition not found in the 'itemsDescription' JSON file.\nError reading '" + uapDescriptionPath
					+ "' file.\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}
	}

	// Store the new ordered itemCollection
	categoryDescription->setItemCollection(orderedItemCollection);
}

// ** MARSHALLING **
void JsonCategoriesDescriptionLoader::performMarshallingFromExternalToInternalDescription(ExternalCategoryDescription itemsDescription,
		ExternalCategoryDescription refDescription, ExternalCategoryDescription spfDescription, UapAndMandatoryItems uapAndMandatoryItems,
		CategoryDescription* categoryDescription, ReturnStatus* status) {
	/************************
	 * Variable definitions *
	 ************************/
	vector<Item>* itemCollectionTmp = nullptr;
	vector<Item>* compoundSubItemCollectionTmp = nullptr;
	vector<Item>* reItemCollectionTmp = nullptr;
	vector<Item>* reCompoundSubItemCollectionTmp = nullptr;
	vector<Item>* spItemCollectionTmp = nullptr;
	vector<Item>* spCompoundSubItemCollectionTmp = nullptr;

	std::vector<std::string> mandatoryItemsList = uapAndMandatoryItems.getMandatoryItems();

	// Load items description
	itemCollectionTmp = marshallingToInnerDatamodel(&itemsDescription, LoadingTypeEnum::ItemEnum, &mandatoryItemsList, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	categoryDescription->setItemCollection(*itemCollectionTmp);

	// Load COMPOUND SubITEMs description
	compoundSubItemCollectionTmp = marshallingToInnerDatamodel(&itemsDescription, LoadingTypeEnum::CompoundSubitemEnum, &mandatoryItemsList, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	//RE ITEMs - If REF file exists, load the description of items and subItems
	if (refDescription.getItemCollection().size() > 0) {

		// Load re items description
		reItemCollectionTmp = marshallingToInnerDatamodel(&refDescription, LoadingTypeEnum::ItemEnum, &mandatoryItemsList, status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}

		// Load COMPOUND SubITEMs description
		reCompoundSubItemCollectionTmp = marshallingToInnerDatamodel(&refDescription, LoadingTypeEnum::CompoundSubitemEnum, &mandatoryItemsList, status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}

		//Put the sub Items of REF in the 'compoundSubItemCollectionTmp' and update indexes in 'reItemCollectionTmp'
		addRE_or_SP_itemInItemCollection(reItemCollectionTmp, reCompoundSubItemCollectionTmp, compoundSubItemCollectionTmp);

		categoryDescription->setRefItemDescription(*reItemCollectionTmp);
	}

	//SPF ITEMs - If SPF file exists, load the description of items and subItems
	if (spfDescription.getItemCollection().size() > 0) {

		// Load re items description
		spItemCollectionTmp = marshallingToInnerDatamodel(&spfDescription, LoadingTypeEnum::ItemEnum, &mandatoryItemsList, status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}

		// Load COMPOUND SubITEMs description
		spCompoundSubItemCollectionTmp = marshallingToInnerDatamodel(&spfDescription, LoadingTypeEnum::CompoundSubitemEnum, &mandatoryItemsList, status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}

		//Put the sub Items of SPF in the 'compoundSubItemCollectionTmp' and update indexes in 'spItemCollectionTmp'
		addRE_or_SP_itemInItemCollection(spItemCollectionTmp, spCompoundSubItemCollectionTmp, compoundSubItemCollectionTmp);

		categoryDescription->setSpfItemDescription(*spItemCollectionTmp);
	}

	categoryDescription->setCompoundSubItemsDescription(*compoundSubItemCollectionTmp);

	// Remove all temporary list created
	if (reItemCollectionTmp != nullptr) {
		reItemCollectionTmp->clear();
		delete reItemCollectionTmp;
	}

	if (reCompoundSubItemCollectionTmp != nullptr) {
		reCompoundSubItemCollectionTmp->clear();
		delete reCompoundSubItemCollectionTmp;
	}

	if (spItemCollectionTmp != nullptr) {
		spItemCollectionTmp->clear();
		delete spItemCollectionTmp;
	}

	if (spCompoundSubItemCollectionTmp != nullptr) {
		spCompoundSubItemCollectionTmp->clear();
		delete spCompoundSubItemCollectionTmp;
	}

	if (compoundSubItemCollectionTmp != nullptr) {
		compoundSubItemCollectionTmp->clear();
		delete compoundSubItemCollectionTmp;
	}

	if (itemCollectionTmp != nullptr) {
		itemCollectionTmp->clear();
		delete itemCollectionTmp;
	}
}

void JsonCategoriesDescriptionLoader::addRE_or_SP_itemInItemCollection(vector<Item>* RE_or_SP_itemCollectionToBeUpdate,
		vector<Item>* RE_or_SP_compoundSubItemCollection, vector<Item>* compoundSubItemCollectionToBeUpdate) {
	/************************
	 * Variable definitions *
	 ************************/
	vector<int>* oldIndexesOfSubItem = nullptr;
	vector<int>* newIndexesOfSubItem = nullptr;
	int newIndexInCompoundCollection = 0;

	Item* subItemTmp;
	Item* itemTmp;

	// Scan each Item, if is CompoundLength type -> create 'elementDescription' with the real index respect the 'compoundSubItemCollectionTmp' and add the Coumpoud Item in the 'compoundSubItemCollectionToBeUpdate'
	for (unsigned int i = 0; i < RE_or_SP_itemCollectionToBeUpdate->size(); i++) {

		itemTmp = &(RE_or_SP_itemCollectionToBeUpdate->at(i));

		if (itemTmp != nullptr && itemTmp->getItemType() == ItemTypeEnum::CompoundLength) {

			//Get all subItems of this current compound Item and create a new vector to store the new indexes
			oldIndexesOfSubItem = itemTmp->getAllElementDescription();
			newIndexesOfSubItem = new vector<int>(oldIndexesOfSubItem->size());

			//Scan each sub Item, get the first free index in 'compoundSubItemCollectionTmp', save in index the current sub item and put the index in the 'newIndexesOfSubItem'
			for (unsigned int j = 0; j < oldIndexesOfSubItem->size(); j++) {

				subItemTmp = &(RE_or_SP_compoundSubItemCollection->at(j));

				newIndexInCompoundCollection = compoundSubItemCollectionToBeUpdate->size();

				compoundSubItemCollectionToBeUpdate->push_back(*subItemTmp);
				newIndexesOfSubItem->at(j) = newIndexInCompoundCollection;
			}

			itemTmp->setAllElementDescription(*newIndexesOfSubItem);
			RE_or_SP_itemCollectionToBeUpdate->at(i) = *itemTmp;	//Substitute the old Item with the updated one
		}
	}
}

vector<Item>* JsonCategoriesDescriptionLoader::marshallingToInnerDatamodel(ExternalCategoryDescription *extCategoryDescription, LoadingTypeEnum type,
		std::vector<std::string> *mandatoryItemsList, ReturnStatus* status) {
	/************************
	 * Variable definitions *
	 ************************/
	vector<Item> *itemCollection = nullptr;
	vector<vector<string>> elementAllDescription;
	vector<string> elementStringDescription;
	// Attributes of "Item" class, to be set
	vector<string> elementNames;
	vector<int> elementDescriptions;
	vector<string> functionNameForDecodingInString;
	vector<string> representationModeForDecodingInString;
	vector<vector<double>> validationRanges;
	vector<vector<double>> lsbValues;
	vector<Range> rangeValues;
	int mandatoryItemNameIndex = -1;

	vector<ExternalItem> extItemCollection;

	itemCollection = new vector<Item>();

	// Based on the 'type' variable value, the right collection is loaded and mapped to the internal data model
	switch (type) {
	case LoadingTypeEnum::ItemEnum:
		extItemCollection = extCategoryDescription->getItemCollection();
		break;
	case LoadingTypeEnum::CompoundSubitemEnum:
		extItemCollection = extCategoryDescription->getCompoundSubitemsCollection();
		break;
	default:
		// Condition never reached, the 'type' parameter is passed directly from
		// the JsonCategoriesDescriptionLoader::performMarshallingFromExternalToInternalDescription function, not from the client
		string message = "The LoadingTypeEnum value provided has not been recognized.";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return nullptr;
		break;
	}

	// For each Item in the collection, we transfer its content to 'itemToFill', which is an Item instance of the internal data model
	for (ExternalItem extItem : extItemCollection) {
		Item itemToFIll;
		itemToFIll.setName(extItem.getName());
		itemToFIll.setType(extItem.getType());
		itemToFIll.setLengthInByte(extItem.getLengthInByte());

		elementAllDescription = extItem.getElementDescriptions();
		// Iteration on ALL ELEMENTS of 'extItem'
		for (unsigned int elementIndex = 0; elementIndex < elementAllDescription.size(); elementIndex++) {
			elementStringDescription = elementAllDescription.at(elementIndex);

			if (elementStringDescription.size() >= kMinimumSizeOfExtElementDescription) {
				// Each string in the 'elementStringDescription' has role strictly related to its position in the array
				elementNames.push_back(elementStringDescription[0]);    					// ELEMENT NAME 		@index 0
				elementDescriptions.push_back(stoi(elementStringDescription[1]));    		// LENGTH IN BIT 		@index 1
				functionNameForDecodingInString.push_back(elementStringDescription[2]);    	// DECODING FUNCTION 	@index 2

				// VALUE RANGES @index 4
				Range rangeStruct;
				if (elementStringDescription[4] == "") {
					// No range has been provided for the current Element ==> All values are VALID
					rangeStruct.minValue = 0;
					rangeStruct.maxValue = 0;
				} else {
					rangeStruct = marshallRangeValues(&elementStringDescription, status);
					if (status->getCode() != ReturnCodes::SUCCESS) {
						return nullptr;
					}
				}
				rangeValues.push_back(rangeStruct);

				// LSB VALUES -> from index 5 until the end of the description
				vector<double> lsbValuesOfCurrentElement;

				for (unsigned int i = 5; i < elementStringDescription.size(); i++) {
					if (elementStringDescription[i] == "") {
						lsbValuesOfCurrentElement.push_back(1);
					} else {
						lsbValuesOfCurrentElement.push_back(stod(elementStringDescription[i]));
					}
				}
				lsbValues.push_back(lsbValuesOfCurrentElement);

				// REPRESENTATION MODE -> index 3
				if (elementStringDescription[3] == "IntegerValue") {
					// This parameter has to be updated with a correctly sized Integer (8, 16, 32 or 64 bits)
					double maxLsb = *(std::max_element(lsbValuesOfCurrentElement.begin(), lsbValuesOfCurrentElement.end()));
					double maxElementValue = (std::pow(2, stoi(elementStringDescription[1]))) * maxLsb;
					double noBits = std::log2(maxElementValue);
					int noBytes = std::ceil(noBits / 8);

					// SIGNED int values
					if (elementStringDescription[2] == "CA2toDecimal") {
						switch (noBytes) {
						case 1:
							representationModeForDecodingInString.push_back("SignedInteger8bitValue");
							break;
						case 2:
							representationModeForDecodingInString.push_back("SignedInteger16bitValue");
							break;
						case 3:
						case 4:
							representationModeForDecodingInString.push_back("SignedInteger32bitValue");
							break;
						default:
							representationModeForDecodingInString.push_back("SignedInteger64bitValue");
							break;
						}
					} else {    // UNSIGNED int values
						switch (noBytes) {
						case 1:
							representationModeForDecodingInString.push_back("UnsignedInteger8bitValue");
							break;
						case 2:
							representationModeForDecodingInString.push_back("UnsignedInteger16bitValue");
							break;
						case 3:
						case 4:
							representationModeForDecodingInString.push_back("UnsignedInteger32bitValue");
							break;
						default:
							representationModeForDecodingInString.push_back("UnsignedInteger64bitValue");
							break;
						}
					}
				} else {
					// All the other representation modes except 'IntegerValue'
					representationModeForDecodingInString.push_back(elementStringDescription[3]);    // Representation Mode
				}
			} else {
				stringstream message;
				message << "An incomplete Element description has been found for Item " << extItem.getName() << " (" << kMinimumSizeOfExtElementDescription
						<< " attributes are required, at least)" << "." << endl;
				status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
				status->setMessage(message.str());
				return nullptr;
			}
		}

		// Update elements information into the current Item
		itemToFIll.setAllElementNames(elementNames);
		itemToFIll.setAllElementDescription(elementDescriptions);
		itemToFIll.setAllFunctionNamesForDecodingInString(functionNameForDecodingInString);
		itemToFIll.setAllRepresentationModesInStringForDecoding(representationModeForDecodingInString);
		itemToFIll.setAllLsbValues(lsbValues);
		itemToFIll.setAllValidRanges(rangeValues);

		// MANDATORY ITEM - Looking for the current Item in the the 'mandatoryItemsList', if exist -> set to mandatory and remove the Item name from the list
		vector<string>::iterator mandatoryItem;
		if ((mandatoryItem = find(mandatoryItemsList->begin(), mandatoryItemsList->end(), itemToFIll.getName())) != mandatoryItemsList->end()) {
			mandatoryItemNameIndex = mandatoryItem - mandatoryItemsList->begin();

			if (mandatoryItemNameIndex != -1)
				mandatoryItemsList->erase(mandatoryItemsList->begin() + mandatoryItemNameIndex);

			itemToFIll.setItemMandatory(true);

		} else {
			itemToFIll.setItemMandatory(false);
		}

		// Push back the Item into the collection of the Category
		itemCollection->push_back(itemToFIll);

		// Clear the current temporary collection where Element information are stored
		elementNames.clear();
		elementDescriptions.clear();
		functionNameForDecodingInString.clear();
		representationModeForDecodingInString.clear();
		lsbValues.clear();
		rangeValues.clear();
	}

	return itemCollection;
}

Range JsonCategoriesDescriptionLoader::marshallRangeValues(vector<string>* elementInfo, ReturnStatus* status) {
	Range rangeStruct;    // Output filled with the current Element ranges
	// REGEX to search for 'lt(n)' or 'gt(n)' in 'elementStringRange'
	const char *lessThanRegexText = "(lt\\((-)?[0-9]+(\\.[0-9]+)?\\))";    		// lt(123.54)
	const char *greaterThanRegexText = "(gt\\((-)?[0-9]+(\\.[0-9]+)?\\))";    	// gt(123.54)
	const char *onlyDigitsRegexText = "(-)?[0-9]+((\\.[0-9]+)?)";				// 123.54
	regex_t lessThanRegex, greaterThanRegex, digitsRegex;
	char* singleRangevalue = nullptr;
	vector<string> singleRangeVect;
	const char* textToSearch;
	char* result;

	// Compile the regular expressions
	regcomp(&lessThanRegex, lessThanRegexText, REG_EXTENDED | REG_NEWLINE);
	regcomp(&greaterThanRegex, greaterThanRegexText, REG_EXTENDED | REG_NEWLINE);
	regcomp(&digitsRegex, onlyDigitsRegexText, REG_EXTENDED | REG_NEWLINE);

	// Variables needed to get the result after a regular expression execution
	int maxMatches = 1, noMatch = 0;
	regmatch_t matches[maxMatches];		// Matches found

	string elementRange = elementInfo->at(4);

	// 'elementStringRange' string gets duplicated because it is modified by strtok function called during the marshalling
	char* elementRangeCOPY = strdup(elementRange.c_str());

	// Parse input string based on '~' separator
	// E.g. [0]: "0" , [1]: "-180~lt(180)" , [2]: "-90~90" , [3]: "gt(0)~lt(100)"

	// 'strtok(input, separator)' | Tokenizes the 'input' string at each 'separator' occurrence
	singleRangevalue = strtok(elementRangeCOPY, &kTilde);

	while (singleRangevalue != nullptr) {
		singleRangeVect.push_back(string(singleRangevalue));
		singleRangevalue = strtok(nullptr, &kTilde);	// with 'nullptr' as input, 'strtok()' continues to tokenize the previous string
	}
	delete[] singleRangevalue;

	// Checking 'singleRangeVect' content with regular expression
	if (singleRangeVect.size() == 1) {    // Just a single value has been provided (e.g. "0" for Spare bit value)
		textToSearch = strdup(singleRangeVect[0].c_str());
		// Check that the range is really a number before converting to double
		noMatch = regexec(&digitsRegex, textToSearch, maxMatches, matches, 0);    // Read only digits
		if (noMatch == 0) {
			rangeStruct.minValue = atoi(singleRangeVect[0].c_str());
			rangeStruct.maxValue = atoi(singleRangeVect[0].c_str());
		} else {
			string elementName = elementInfo->at(0);
			string message = "The range '" + singleRangeVect[0] + "' inserted for the Element '" + elementName + "' is not a valid float value.";
			status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
			status->setMessage(message);
			return rangeStruct;
		}
	} else if (singleRangeVect.size() == 2) {    // LOWER and UPPER bounds have been provided
		// Parsing the LOWER Element BOUND
		textToSearch = strdup(singleRangeVect[0].c_str());

		// If regex matches, Element value shall be greater (>) than lower bound
		noMatch = regexec(&greaterThanRegex, textToSearch, maxMatches, matches, 0);
		if (noMatch == 0) {    // noMatch = 0 means regex matches
			rangeStruct.isMinIncluded = false;
		}
		// Check if the user are using lower than - lt(num) - applied to the lower bound of the range -> no make sense
		noMatch = regexec(&lessThanRegex, textToSearch, maxMatches, matches, 0);
		if (noMatch == 0) {    // noMatch = 0 means regex matches
			string elementName = elementInfo->at(0);
			string message = "The lower bound range '" + string(textToSearch) + "' inserted for the Element '" + elementName
					+ "' does not make sense. For the lower bound, only 'gt(num)' or 'num' is supported.";
			status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
			status->setMessage(message);
			return rangeStruct;
		}
		// Extract digits from the string (if digits are valid)
		noMatch = regexec(&digitsRegex, textToSearch, maxMatches, matches, 0);    // Read only digits
		if (noMatch == 0) {
			result = (char*) malloc((matches[0].rm_eo - matches[0].rm_so) + 1);    // +1 for the string terminator '\0'
			strncpy(result, &textToSearch[matches[0].rm_so], matches[0].rm_eo - matches[0].rm_so);
			result[(matches[0].rm_eo - matches[0].rm_so)] = '\0';    // adding the string terminator '\0'
			rangeStruct.isEmpty = false;
			rangeStruct.minValue = (double) atof(result);
		} else {
			string elementName = elementInfo->at(0);
			string message = "The range '" + string(textToSearch) + "' inserted for the Element '" + elementName + "' is not a valid float value.";
			status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
			status->setMessage(message);
			return rangeStruct;
		}

		delete[] textToSearch;
		delete[] result;

		// Parsing the UPPER Element BOUND
		textToSearch = strdup(singleRangeVect[1].c_str());

		// If regex matches, Element value shall be lower (<) than upper bound
		noMatch = regexec(&lessThanRegex, textToSearch, maxMatches, matches, 0);
		if (noMatch == 0) {    // noMatch = 0 means regex matches
			rangeStruct.isMaxIncluded = false;
		}
		// Check if the user are using greater than - gt(num) - applied to the upper bound of the range -> no make sense
		noMatch = regexec(&greaterThanRegex, textToSearch, maxMatches, matches, 0);
		if (noMatch == 0) {    // noMatch = 0 means regex matches
			string elementName = elementInfo->at(0);
			string message = "The upper bound range '" + string(textToSearch) + "' inserted for the Element '" + elementName
					+ "' does not make sense. For the upper bound, only 'lt(num)' or 'num' is supported.";
			status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
			status->setMessage(message);
			return rangeStruct;
		}
		// Extract digits from the string (if digits are valid)
		noMatch = regexec(&digitsRegex, textToSearch, maxMatches, matches, 0);    // Read only digits
		if (noMatch == 0) {
			result = (char*) malloc((matches[0].rm_eo - matches[0].rm_so) + 1);		// +1 for the string terminator '\0'
			strncpy(result, &textToSearch[matches[0].rm_so], matches[0].rm_eo - matches[0].rm_so);
			result[(matches[0].rm_eo - matches[0].rm_so)] = '\0';    // adding the string terminator '\0'
			rangeStruct.maxValue = (double) atof(result);
			rangeStruct.isEmpty = false;
		} else {
			string elementName = elementInfo->at(0);
			string message = "The range '" + string(textToSearch) + "' inserted for the Element '" + elementName + "' is not a valid float value.";
			status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
			status->setMessage(message);
			return rangeStruct;
		}

		delete[] textToSearch;
		delete[] result;
	} else {
		string elementName = elementInfo->at(0);
		string message = "Format of the range inserted for the Element '" + elementName
				+ "' has not been recognized. Supported format are: \"0\", \"gt(0)~15\", \"-180~lt(180)\", ...";
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return rangeStruct;
	}

	delete[] elementRangeCOPY;

	regfree(&lessThanRegex);
	regfree(&greaterThanRegex);

	return rangeStruct;
}

// ** FILTERING **
void JsonCategoriesDescriptionLoader::checkAndSetFiltersForAllCategoriesDescription(string categoriesFilterPath, CategoriesFilters& categoriesFilters,
		unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status) {
	/************************
	 * Variable definitions *
	 ************************/
	CategoriesToFilter decoderFilter;
	CategoriesToFilter encoderFilter;
	FilterType decoderFilterType;
	FilterType encoderFilterType;

	std::vector<int> decoderCategoryNumberToFilter;
	std::vector<int> encoderCategoryNumberToFilter;

	std::vector<int> decoderCategoryAndDatasourcesToFilter;
	std::vector<int> encoderCategoryAndDatasourcesToFilter;

	// Gets filters from 'categoriesFilters'
	decoderFilter = categoriesFilters.getFilterForDecoder();
	encoderFilter = categoriesFilters.getFilterForEncoder();

	//Check the validity and prepare the list of decoder and encoder before the usage
	JsonCategoriesDescriptionValidator::checkAndPrepareCategoriesFilterList(categoriesFilterPath, true, decoderFilter, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	JsonCategoriesDescriptionValidator::checkAndPrepareCategoriesFilterList(categoriesFilterPath, false, encoderFilter, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	//At this point, 'decoderFilter' and 'encoderFilter' contain the list of Categories and Categories+Datasources
	categoriesFilters.setFilterForDecoder(decoderFilter);
	categoriesFilters.setFilterForEncoder(encoderFilter);

	decoderFilterType = decoderFilter.getTypeOfFiltering();
	encoderFilterType = encoderFilter.getTypeOfFiltering();

	decoderCategoryNumberToFilter = decoderFilter.getListOfCategoryInDecimalToFilter();
	encoderCategoryNumberToFilter = encoderFilter.getListOfCategoryInDecimalToFilter();

	decoderCategoryAndDatasourcesToFilter = decoderFilter.getListOfCategoryAndDatasourcesInDecimalToFilter();
	encoderCategoryAndDatasourcesToFilter = encoderFilter.getListOfCategoryAndDatasourcesInDecimalToFilter();

	/*
	 * Start to set the Categories filter from the datasources and then set the filter based on the Category number, in order to prevent a strange behavior
	 * e.g. The keys of the uapMap are: 21, 21050080 and 62 - The filter for encoder or decoder is Include: Category 21
	 * 1) Check on the Cat+Datasources -> the list is empty -> no Category to decode/encode
	 * 2) Check the Category number -> the list contains 1 Category -> key 21 will decoded/encoded and the other two will be filter, because 21050080 is more complex
	 * The solution is to looking for the Category number in the keys of the uapMap -> keys 21 and 21050080 will be decoded/encoded
	 */
	setFiltersForAllCategoriesDescription(true, decoderFilterType, encoderFilterType, decoderCategoryNumberToFilter, encoderCategoryNumberToFilter, uapMap, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	setFiltersForAllCategoriesDescription(false, decoderFilterType, encoderFilterType, decoderCategoryAndDatasourcesToFilter, encoderCategoryAndDatasourcesToFilter, uapMap, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}
}

void JsonCategoriesDescriptionLoader::setFiltersForAllCategoriesDescription(bool isLookingCatNumber, FilterType decoderFilterType, FilterType encoderFilterType, std::vector<int>& decoderCategoryNumberToFilter, std::vector<int>& encoderCategoryNumberToFilter, unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status){

	// Variables used to set the filters in all Categories Description
	int currentCategoryDescriptionKey = 0;
	int indexOfCategoryKeyToRemove = 0;
	std::vector<int>::iterator categoryFilterInDecimal_it;
	bool categoryToBeDecoded;
	bool categoryToBeEncoded;

	for(auto categoryDescription_it = uapMap->begin(); categoryDescription_it!=uapMap->end(); categoryDescription_it++){

		//Looking for key of 'categoryDescription_it' in the list of Categories to Decode and to Encode

		if(isLookingCatNumber && categoryDescription_it->first>999){	// > 999 -> just to check that the number of digit is more than 3
			currentCategoryDescriptionKey = categoryDescription_it->first/1000000;
		}else{
			currentCategoryDescriptionKey = categoryDescription_it->first;
		}

		/**
		 * If currentCategoryDescriptionKey is in the 'decoderCategoryNumberToFilter' and
		 * - FilterType == ExclusiveFilter -> EXCLUDE current Category Description from the decoding
		 * - FilterType == InclusiveFilter -> INCLUDE current Category Description in the decoding
		 * Only if we are looking the Category Number, we set the 'categoryToBeDecoded' also if the Category is not in the 'decoderCategoryNumberToFilter'
		 * If we are looking the Category Number + Datasource, we propagate the already setted value because that filter not involved other Category Number + Datasource
		 */
		if ((categoryFilterInDecimal_it = find(decoderCategoryNumberToFilter.begin(), decoderCategoryNumberToFilter.end(), currentCategoryDescriptionKey)) != decoderCategoryNumberToFilter.end()) {    //Item exists in 'decoderFilterList'

			if (decoderFilterType == FilterType::ExclusiveFilter)
				categoryToBeDecoded = false;
			else
				categoryToBeDecoded = true;

			// If we are looking for the key of Cat+datasource, after that we found, we can get the index of Category key found in the 'decoderCategoryNumberToFilter' and remove it
			// If we are looking only for the Cat -> we don't remove because it can be used several times
			if(!isLookingCatNumber){
				indexOfCategoryKeyToRemove = categoryFilterInDecimal_it - decoderCategoryNumberToFilter.begin();

				if (indexOfCategoryKeyToRemove != -1)
					decoderCategoryNumberToFilter.erase(decoderCategoryNumberToFilter.begin() + indexOfCategoryKeyToRemove);
			}

		} else if(isLookingCatNumber){
			if (decoderFilterType == FilterType::ExclusiveFilter)
				categoryToBeDecoded = true;
			else
				categoryToBeDecoded = false;
		}else{
			categoryToBeDecoded = categoryDescription_it->second->isToBeDecoded();
		}

		/**
		 * If currentCategoryDescriptionKey is in the 'encoderCategoryToFilter' and
		 * - FilterType == ExclusiveFilter -> EXCLUDE current Category Description from the decoding
		 * - FilterType == InclusiveFilter -> INCLUDE current Category Description in the decoding
		 * Only if we are looking the Category Number, we set the 'categoryToBeEncoded' also if the Category is not in the 'encoderCategoryToFilter'
		 * If we are looking the Category Number + Datasource, we propagate the already setted value because that filter not involved other Category Number + Datasource
		 */
		if ((categoryFilterInDecimal_it = find(encoderCategoryNumberToFilter.begin(), encoderCategoryNumberToFilter.end(), currentCategoryDescriptionKey)) != encoderCategoryNumberToFilter.end()) {    //Item exists in 'decoderFilterList'

			if (encoderFilterType == FilterType::ExclusiveFilter)
				categoryToBeEncoded = false;
			else
				categoryToBeEncoded = true;

			// If we are looking for the key of Cat+datasource, after that we found, we can get the index of Category key found in the 'encoderCategoryNumberToFilter' and remove it
			// If we are looking only for the Cat -> we don't remove because it can be used several times
			if(!isLookingCatNumber){
				indexOfCategoryKeyToRemove = categoryFilterInDecimal_it - encoderCategoryNumberToFilter.begin();

				if (indexOfCategoryKeyToRemove != -1)
					encoderCategoryNumberToFilter.erase(encoderCategoryNumberToFilter.begin() + indexOfCategoryKeyToRemove);
			}

		} else if(isLookingCatNumber){
			if (encoderFilterType == FilterType::ExclusiveFilter)
				categoryToBeEncoded = true;
			else
				categoryToBeEncoded = false;
		}else{
			categoryToBeEncoded = categoryDescription_it->second->isToBeEncoded();
		}

		// Set the 'toBeDecoded' and 'toBeEncoded' fields for the current Category
		categoryDescription_it->second->setToBeDecoded(categoryToBeDecoded);
		categoryDescription_it->second->setToBeEncoded(categoryToBeEncoded);
	}

	if(!isLookingCatNumber){
		//Check the size of 'decoderCategoryToFilter' and 'encoderCategoryToFilter' -> if is 0 -> error, some Categories Number/Categories Number + Datasources in the list not exist in the 'asterixConfiguration.json' file
		//TODO: to improve the message, now is a list of: <CatN> [ + <SAC> + <SIC>]
		if (decoderCategoryNumberToFilter.size() != 0) {
			string message = "Some filters for the Categories listed in 'decoderFilter' were not found";

			for (unsigned int i = 0; i < decoderCategoryNumberToFilter.size(); i++) {
				message += " - " + to_string(decoderCategoryNumberToFilter.at(i));
			}
			message += ".\n";
			status->setCode(ReturnCodes::WARNING);
			status->setMessage(message);
		}

		if (encoderCategoryNumberToFilter.size() != 0) {
			string message = "Some filters for the Categories listed in 'decoderFilter' were not found";

			for (unsigned int i = 0; i < encoderCategoryNumberToFilter.size(); i++) {
				message += " - " + to_string(encoderCategoryNumberToFilter.at(i));
			}
			message += ".\n";
			status->setCode(ReturnCodes::WARNING);
			status->setMessage(message);
		}
	}
}


void JsonCategoriesDescriptionLoader::checkAndSetFiltersForCategoryDescription(string filterPath, CategoryItemsFilters categoryItemsFilters,
		CategoryDescription* categoryDescription, ReturnStatus* status) {
	/************************
	 * Variable definitions *
	 ************************/
	ItemsToFilter decoderFilter;
	ItemsToFilter encoderFilter;
	FilterType decoderFilterType;
	FilterType encoderFilterType;
	std::vector<std::string> decoderFilterList;
	std::vector<std::string> encoderFilterList;

	std::vector<Item> *compoundSubItemsDescription = nullptr;
	std::vector<Item> *reItemDescription = nullptr;
	std::vector<Item> *spItemDescription = nullptr;
	std::vector<Item> *itemsDescription = nullptr;

	// Gets filters from 'categoryItemsFilters'
	decoderFilter = categoryItemsFilters.getFilterForDecoder();
	encoderFilter = categoryItemsFilters.getFilterForEncoder();

	decoderFilterType = decoderFilter.getTypeOfFiltering();
	encoderFilterType = encoderFilter.getTypeOfFiltering();

	decoderFilterList = decoderFilter.getListOfItemsToFilter();
	encoderFilterList = encoderFilter.getListOfItemsToFilter();

	//Check the validity and prepare the list of decoder and encoder (put name in uppercase, remove ~REF or ~SPF) before the usage
	JsonCategoriesDescriptionValidator::checkAndPrepareFilterList(filterPath, true, decoderFilterType, &decoderFilterList, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	JsonCategoriesDescriptionValidator::checkAndPrepareFilterList(filterPath, false, encoderFilterType, &encoderFilterList, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	// Set the filters in all descriptions, pass 'compoundSubItemsDescription' to the methods in order to propagate the filter information in the subItems of the compound items
	// In 'setFiltersForVectorOfItems', when an Item to encode/decode is found in the description, this Item name is removed from the 'encoderFilterList' and from 'decoderFilterList'
	compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();

	reItemDescription = categoryDescription->getRefItemDescription();
	setFiltersForVectorOfItems(decoderFilterType, encoderFilterType, &decoderFilterList, &encoderFilterList, reItemDescription, compoundSubItemsDescription);

	spItemDescription = categoryDescription->getSpfItemDescription();
	setFiltersForVectorOfItems(decoderFilterType, encoderFilterType, &decoderFilterList, &encoderFilterList, spItemDescription, compoundSubItemsDescription);

	itemsDescription = categoryDescription->getItemCollection();
	setFiltersForVectorOfItems(decoderFilterType, encoderFilterType, &decoderFilterList, &encoderFilterList, itemsDescription, compoundSubItemsDescription);

	//Check the size of 'decoderFilterList' and 'encoderFilterList' -> if is 0 -> error, some Items in the list not exist in the descriptions

	if (decoderFilterList.size() != 0) {
		string message = "The description of the following Items listed in 'decoderFilter' were not found";

		for (unsigned int i = 0; i < decoderFilterList.size(); i++) {
			message += " - " + decoderFilterList.at(i);
		}
		message += "\nError while loading " + filterPath + " file.";

		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	if (encoderFilterList.size() != 0) {
		string message = "The description of the following Items listed in 'encoderFilter' were not found";

		for (unsigned int i = 0; i < encoderFilterList.size(); i++) {
			message += " - " + encoderFilterList.at(i);
		}
		message += "\nError while loading " + filterPath + " file.";

		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}
}

void JsonCategoriesDescriptionLoader::setFiltersForVectorOfItems(FilterType decoderFilterType, FilterType encoderFilterType,
		std::vector<std::string>* decoderFilterList, std::vector<std::string>* encoderFilterList, std::vector<Item>* itemsDescription,
		std::vector<Item>* compoundSubItemsDescription) {

	std::vector<Item>::iterator currentItem;
	std::vector<std::string>::iterator itemToFilterForDecoder;
	std::vector<std::string>::iterator itemToFilterForEncoder;

	string currentItemName;
	bool itemToBeDecoded = false;
	bool itemToBeEncoded = false;
	int indexOfItemNameToRemove = -1;

	// Scan each Item in the 'itemList' and set the right values of 'toBeDecoded' and 'toBeEncoded' fields.
	for (currentItem = itemsDescription->begin(); currentItem != itemsDescription->end(); currentItem++) {
		currentItemName = (*currentItem).getName();

		itemToBeDecoded = false;
		itemToBeEncoded = false;

		// Set to true the 'toBeDecoded' and 'toBeEncoded' field for REF and SPF ->  enable or disable is performed by the variables in CodecInterface
		if (currentItemName == kREF || currentItemName == kSPF) {
			(*currentItem).setToBeDecoded(true);
			(*currentItem).setToBeEncoded(true);
			continue;
		}

		// If currentItem.name is in the 'decoderFilterList' and
		// - FilterType == ExclusiveFilter -> EXCLUDE current Item from the decoding
		// - FilterType == InclusiveFilter -> INCLUDE current Item in the decoding
		if ((itemToFilterForDecoder = find(decoderFilterList->begin(), decoderFilterList->end(), currentItemName)) != decoderFilterList->end()) {    //Item exists in 'decoderFilterList'

			if (decoderFilterType == FilterType::ExclusiveFilter)
				itemToBeDecoded = false;
			else
				itemToBeDecoded = true;

			//Get index of Item name found in the 'decoderFilterList' and remove it
			indexOfItemNameToRemove = itemToFilterForDecoder - decoderFilterList->begin();

			if (indexOfItemNameToRemove != -1)
				decoderFilterList->erase(decoderFilterList->begin() + indexOfItemNameToRemove);

		} else {
			if (decoderFilterType == FilterType::ExclusiveFilter)
				itemToBeDecoded = true;
			else
				itemToBeDecoded = false;
		}

		// If currentItem.name is in the 'encoderFilterList' and
		// - FilterType == ExclusiveFilter -> EXCLUDE current item from the encoding
		// - FilterType == InclusiveFilter -> INCLUDE current item in the encoding
		if ((itemToFilterForEncoder = find(encoderFilterList->begin(), encoderFilterList->end(), currentItemName)) != encoderFilterList->end()) {    //Item exists in 'encoderFilterList'

			if (encoderFilterType == FilterType::ExclusiveFilter)
				itemToBeEncoded = false;
			else
				itemToBeEncoded = true;

			//Get index of Item name found in the 'encoderFilterList' and remove it
			indexOfItemNameToRemove = itemToFilterForEncoder - encoderFilterList->begin();
			if (indexOfItemNameToRemove != -1)
				encoderFilterList->erase(encoderFilterList->begin() + indexOfItemNameToRemove);

		} else {
			if (encoderFilterType == FilterType::ExclusiveFilter)
				itemToBeEncoded = true;
			else
				itemToBeEncoded = false;
		}

		// Set the correct value of the filter for the current Item
		(*currentItem).setToBeDecoded(itemToBeDecoded);
		(*currentItem).setToBeEncoded(itemToBeEncoded);

		// Propagate the 'itemToBeDecoded' and 'itemToBeEncoded' fields in the subItems - if exists
		if ((*currentItem).getItemType() == ItemTypeEnum::CompoundLength) {
			//Get indexes of the description of subItems
			vector<int>* indexesInCompoundDescription = (*currentItem).getAllElementDescription();
			int subItemDescriptionIndex = 0;

			//For each index in 'indexesInCompoundDescription' -> go to the description in 'compoundSubItemsDescription' and set the 'ToBeDecoded' and 'ToBeEncoded' fields
			for (unsigned int i = 0; i < indexesInCompoundDescription->size(); i++) {
				subItemDescriptionIndex = indexesInCompoundDescription->at(i);

				compoundSubItemsDescription->at(subItemDescriptionIndex).setToBeDecoded(itemToBeDecoded);
				compoundSubItemsDescription->at(subItemDescriptionIndex).setToBeDecoded(itemToBeEncoded);
			}
		}
	}
}

// ** SERIALIZATION **
void JsonCategoriesDescriptionLoader::getCategoriesDescriptionMapFromSerialization(string &astCatRepositoryPath,
		unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status) {

	std::ifstream ifs(astCatRepositoryPath);
	if (!ifs.good()) {
		string message = "Error opening '" + astCatRepositoryPath + "' file." + '\n';
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message);
		return;
	}

	boost::archive::text_iarchive ia(ifs);
	ia >> *uapMap;

	ifs.close();

}

void JsonCategoriesDescriptionLoader::serializeCategoriesDescriptionMap(string &pathToArchive, unordered_map<int, CategoryDescription*> *uapMap,
		ReturnStatus* status) {

	std::ofstream ofs(pathToArchive);
	if (!ofs.good()) {
		string message = "Error creating '" + pathToArchive + "' file." + '\n';
		status->setCode(ReturnCodes::SERIALIZATION_FAILED);
		status->setMessage(message);
		return;
	}
	boost::archive::text_oarchive oa(ofs);
	oa << *uapMap;

	ofs.close();
}
