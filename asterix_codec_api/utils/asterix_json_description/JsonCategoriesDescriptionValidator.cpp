#include "JsonCategoriesDescriptionValidator.h"

using namespace std;

// Declaration of static fields
CommonVariables* JsonCategoriesDescriptionValidator::commonVariables = CommonVariables::getInstance();

bool JsonCategoriesDescriptionValidator::checkIfKeyInCategoryDescriptionMapExists(int key, unordered_map<int, CategoryDescription*> *uapMap) {
	return (uapMap->find(key) != uapMap->end());
}

int JsonCategoriesDescriptionValidator::splitDatasourceString(string originalDatasource, int categoryNumber, string astConfigurationFilePath,
		ReturnStatus *status) {
	// Variables needed to split SAC-SIC string
	string delimiter = "-";
	string decSac, decSic;
	size_t pos;
	short zeroMissing = 0;
	vector<string> zeroToBeAdded;
	zeroToBeAdded.push_back("");
	zeroToBeAdded.push_back("0");
	zeroToBeAdded.push_back("00");

	stringstream ss;

	// Regular Expression in order to check the Syntax of input datasources
	const char *sacSicRegexText = "^(([0-9]{1,3})-([0-9]{1,3}))";    // "50-120"
	regex_t sacSicRegex;
	int maxMatches = 1, noMatch = 0;
	regmatch_t matches[maxMatches];		// Matches found

	regcomp(&sacSicRegex, sacSicRegexText, REG_EXTENDED | REG_NEWLINE);

	noMatch = regexec(&sacSicRegex, originalDatasource.c_str(), maxMatches, matches, 0);
	if (noMatch != 0) {    // noMatch != 0 means regex not match
		// Error, input datasource with wrong syntax
		stringstream message;
		message << "Datasource " << originalDatasource
				<< " with wrong syntax. Supported syntax is \"SAC-SIC\" with SAC and SIC in decimal representation, i.e. \"50-80\". \nError while reading ASTERIX Configuration file: "
				<< astConfigurationFilePath << endl;
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message.str());
		return -1;
	}
	regfree(&sacSicRegex);

	// Split  SAC and SIC
	pos = originalDatasource.find(delimiter);
	if (pos != std::string::npos) {
		// SAC
		decSac = originalDatasource.substr(0, pos);
		originalDatasource.erase(0, pos + delimiter.length());
		// SIC
		decSic = originalDatasource;
	}

	// SAC and SIC are both represented in a single byte, so their values cannot exceed 255 (in decimal).
	if (stoi(decSac) > 255 || stoi(decSic) > 255) {
		stringstream message;
		message << "Datasource \"" << decSac << "-" << decSic
				<< "\" is out of range. Maximum value for SAC or SIC is 255.\nError while reading ASTERIX Configuration file: " << astConfigurationFilePath
				<< endl;
		status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		status->setMessage(message.str());
		return -1;
	}

	// First of all, flush CAT number in the stream
	ss << categoryNumber;

	// Check the size of SAC and SIC. They shall have 3 digits
	zeroMissing = 3 - decSac.length();    // zeroMissing is 0, 1 or 2
	ss << zeroToBeAdded[zeroMissing] << decSac;

	zeroMissing = 3 - decSic.length();    // zeroMissing is 0, 1 or 2
	ss << zeroToBeAdded[zeroMissing] << decSic;

	return stoi(ss.str());
}

// ** Check VALIDATION of JSON files **
void JsonCategoriesDescriptionValidator::performValidationOfJsonFiles(string itemDescriptionPath, string refDescriptionPath,string spfDescriptionPath,CategoryDescription* categoryDescription, ReturnStatus* status) {
	vector<string> allElementNames; // collect all elements name found during the validator of jsons

	// Check if 'coumpoundFile' contains valid objects
	compoundJsonValidator(itemDescriptionPath, categoryDescription, &allElementNames, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	// Check if 'jsonRefPath' contains valid objects
	REF_SPF_JsonValidator(refDescriptionPath, true, categoryDescription, &allElementNames, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	// Check if 'jsonSpfPath' contains valid objects
	REF_SPF_JsonValidator(spfDescriptionPath, false, categoryDescription, &allElementNames, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}

	// Check if 'categoryFile' contains valid objects
	jsonValidator(itemDescriptionPath, categoryDescription, &allElementNames, status);
	if (status->getCode() != ReturnCodes::SUCCESS) {
		return;
	}
}

void JsonCategoriesDescriptionValidator::jsonValidator(string fileName, CategoryDescription *categoryDescription, vector<string>* allElementNames,ReturnStatus *returnStatus) {
	/************************
	 * Variable definitions *
	 ************************/
	Item currentItem;
	string elementName;

	vector<Item>* itemCollection;
	vector<string>* elementNames;
	vector<int>* elementDescriptions;
	vector<string>* functionNameForDecoding;
	vector<vector<double>>* lsbValues;
	vector<string>* representationModeForDecoding;

	/*************************
	 * Start JSON Validation *
	 *************************/
	itemCollection = categoryDescription->getItemCollection();

	/********************************************************************
	 * Reading all the Items defined and checking Rules JSON Validation *
	 ********************************************************************/
	for (unsigned int index = 0; index < itemCollection->size(); index++) {
		currentItem = itemCollection->at(index);

		elementNames = currentItem.getAllElementNames();
		elementDescriptions = currentItem.getAllElementDescription();
		functionNameForDecoding = currentItem.getAllFunctionNamesForDecodingInString();
		lsbValues = currentItem.getLsbValues();
		representationModeForDecoding = currentItem.getAllRepresentationModesInStringForDecoding();

		// Checking the number of Element names and Element descriptions |  They shall be EQUAL
		if (elementNames->size() != elementDescriptions->size()) {
			cerr << "In "<< fileName << " file - " <<currentItem.getName() << " - The size of 'elementNames' and the size of 'elementDescriptions' are different." << endl;
			returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
			returnStatus->setMessage("Error while reading " + fileName + ".");
			return;
		}

		if ((currentItem.getItemType() == ItemTypeEnum::FixedLength) || (currentItem.getItemType() == ItemTypeEnum::RepetitiveLength)
				|| (currentItem.getItemType() == ItemTypeEnum::ExtendedLength)) {

			checkFixedRepetitiveExtendedItem(fileName, currentItem, returnStatus);

			// Checking that all Element names of Compound Items are UNIQUE
			for (unsigned int innerIndex = 0; innerIndex < elementNames->size(); innerIndex++) {
				elementName = elementNames->at(innerIndex);

				if ((elementName != kSpare) && (elementName != kFX)) {
					if ((find(allElementNames->begin(), allElementNames->end(), elementName)) == allElementNames->end()) {
						// A UNIQUE Element has been found.. so it is inserted in 'allElementNames' collection
						allElementNames->push_back(elementName);
					} else {
						// A DUPLICATED Element has been found.. throw a error!
						string message = "In " + fileName + " file - The Element '" + elementName + "' is duplicated!\n";
						returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
						returnStatus->setMessage(message);
						return;
					}
				}
			}

			// Vector sizes SHALL be EQUAL
			if (functionNameForDecoding->size() != elementDescriptions->size()) {
				cerr << "In "<< fileName << " file - " <<currentItem.getName() << " - The size of 'functionNameForDecoding' and the size of 'elementDescriptions' are different." << endl;
				returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
				returnStatus->setMessage("Error while reading " + fileName + ".");
				return;

			} else if (lsbValues->size() != elementDescriptions->size()) {
				cerr << "In "<< fileName << " file - " <<currentItem.getName() << " - The size of 'lsbValues' and the size of 'elementDescriptions' are different." << endl;
				returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
				returnStatus->setMessage("Error while reading " + fileName + ".");
				return;

			} else if (representationModeForDecoding->size() != elementDescriptions->size()) {
				cerr << "In "<< fileName << " file - " <<currentItem.getName() << " - The size of 'representationModeForDecoding' and the size of 'elementDescriptions' are different." << endl;
				returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
				returnStatus->setMessage("Error while reading " + fileName + ".");
				return;
			}

		} else if (currentItem.getItemType() == ItemTypeEnum::CompoundLength) {
			if (checkIndexesInSubItemsDescription(fileName, currentItem, categoryDescription->getCompoundSubItemsDescription(), returnStatus) == false) {
				return;
			}

		} else if (currentItem.getItemType() == ItemTypeEnum::ExplicitLength) {
			if (currentItem.getName() == kREF) {
				if (checkIndexesInSubItemsDescription(fileName, currentItem, categoryDescription->getRefItemDescription(), returnStatus) == false)
					return;
			} else if (currentItem.getName() == kSPF) {
				if (checkIndexesInSubItemsDescription(fileName, currentItem, categoryDescription->getSpfItemDescription(), returnStatus) == false) {
					return;
				}
			}
		} else {	// This point is unreachable, the syntax check of autoJSON not accept type not register in 'STATICJSON_DECLARE_ENUM' macro in 'ExternalItem' class
			string message = "In " + fileName + " file - " + currentItem.getName()
													+ " Unknown type of Item [could be:  extended, repetitive, compound, fixed, explicit - case matters!].\n";
			returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			returnStatus->setMessage(message);
			return;
		}
	}
}

void JsonCategoriesDescriptionValidator::compoundJsonValidator(string fileName, CategoryDescription *categoryDescription, vector<string>* allElementNames, ReturnStatus *status) {
	/************************
	 * Variable definitions *
	 ************************/
	Item currentSubItem;
	string elementName;

	vector<Item>* subItemCollection;
	vector<string>* elementNamesToValidate;
	vector<int>* elementDescriptions;
	vector<string>* functionNameForDecoding;
	vector<vector<double>>* lsbValues;
	vector<string>* representationModeForDecoding;

	/*************************
	 * Start JSON Validation *
	 *************************/
	subItemCollection = categoryDescription->getCompoundSubItemsDescription();

	// Checking all Compound subItems
	for (unsigned int index = 0; index < subItemCollection->size(); index++) {
		currentSubItem = subItemCollection->at(index);

		elementNamesToValidate = currentSubItem.getAllElementNames();
		elementDescriptions = currentSubItem.getAllElementDescription();
		functionNameForDecoding = currentSubItem.getAllFunctionNamesForDecodingInString();
		lsbValues = currentSubItem.getLsbValues();
		representationModeForDecoding = currentSubItem.getAllRepresentationModesInStringForDecoding();

		// Checking the number of Element names and Element descriptions | They must be EQUAL
		if (elementNamesToValidate->size() != elementDescriptions->size()) {
			cerr << "In " << fileName << " file - " << currentSubItem.getName()
													<< " - The size of 'elementNamesToValidate' and the size of 'elementDescriptions' are different." << endl;
			status->setCode(ReturnCodes::AST_LOADER_ERROR);
			status->setMessage("Error while reading " + fileName + ".");
			return;
		}

		if ((currentSubItem.getItemType() == ItemTypeEnum::FixedLength) || (currentSubItem.getItemType() == ItemTypeEnum::RepetitiveLength)
				|| (currentSubItem.getItemType() == ItemTypeEnum::ExtendedLength)) {

			checkFixedRepetitiveExtendedItem(fileName, currentSubItem, status);

			// Checking all Element names of Compound Items | They must be UNIQUE
			for (unsigned int innerIndex = 0; innerIndex < elementNamesToValidate->size(); innerIndex++) {
				elementName = elementNamesToValidate->at(innerIndex);

				if (elementName != kSpare && elementName != kFX) {
					if (find(allElementNames->begin(), allElementNames->end(), elementName) == allElementNames->end()) {
						// 'elementName' is UNIQUE -> it can be inserted in 'allElementNames' collection
						allElementNames->push_back(elementName);

					} else {
						// 'elementName' is NOT unique | it is already present in 'allElementNames' collection
						string message = "In " + fileName + " file - The Element '" + elementName + "' is duplicated.";
						status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
						status->setMessage(message);
						return;
					}
				}
			}

			// Vector sizes shall be EQUAL
			if (functionNameForDecoding->size() != elementDescriptions->size()) {
				cerr << "In "<< fileName << " - " << currentSubItem.getName()
														<< " - The size of 'functionNameForDecoding' and the size of 'elementDescriptions' are different." << endl;
				status->setCode(ReturnCodes::AST_LOADER_ERROR);
				status->setMessage("Error while reading " + fileName + ".");
				return;

			} else if (lsbValues->size() != elementDescriptions->size()) {
				cerr << "In "<< fileName << " - " << currentSubItem.getName()
														<< " - The size of 'lsbValues' and the size of 'elementDescriptions' are different." << endl;
				status->setCode(ReturnCodes::AST_LOADER_ERROR);
				status->setMessage("Error while reading " + fileName + ".");
				return;

			} else if (representationModeForDecoding->size() != elementDescriptions->size()) {
				cerr << "In "<< fileName << " - " << currentSubItem.getName()
														<< " - The size of 'representationModeForDecoding' and the size of 'elementDescriptions' are different." << endl;
				status->setCode(ReturnCodes::AST_LOADER_ERROR);
				status->setMessage("Error while reading " + fileName + ".");
				return;
			}

		} else if (currentSubItem.getItemType() == ItemTypeEnum::CompoundLength) {
			stringstream message;
			message << "In " << fileName << " file - " << currentSubItem.getName() << " - Wrong Item type: a Compound Item CANNOT contain another Compound."
					<< endl;
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message.str());
			return;

			//This point is unreachable, the syntax check of autoJSON not accept type not register in 'STATICJSON_DECLARE_ENUM' macro in 'ExternalItem' class
		} else if (currentSubItem.getItemType() != ItemTypeEnum::ExplicitLength) {
			string message = "In " + fileName + " file - " + currentSubItem.getName()
													+ " - Unknown Item type [could be:  extended, repetitive, compound, fixed, explicit - case matters!].\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage("Error while reading " + fileName + ".");
			return;
		}
	}
}

void JsonCategoriesDescriptionValidator::REF_SPF_JsonValidator(string fileName, bool isRefJson, CategoryDescription *categoryDescription, vector<string>* allElementNames, ReturnStatus *returnStatus) {
	/*********************
	 * Variable definitions *
	 *********************/
	Item currentItem;
	string elementName;

	vector<Item>* itemCollection;
	vector<string> *elementNamesToValidate;
	vector<int>* elementDescriptions;
	vector<string>* functionNameForDecoding;
	vector<vector<double>>* lsbValues;
	vector<string>* representationModeForDecoding;

	/*************************
	 * Start JSON Validation *
	 *************************/
	if(isRefJson)
		itemCollection = categoryDescription->getRefItemDescription();
	else
		itemCollection = categoryDescription->getSpfItemDescription();

	// Checking all subItems
	for (unsigned int index = 0; index < itemCollection->size(); index++) {
		currentItem = itemCollection->at(index);

		elementNamesToValidate = currentItem.getAllElementNames();
		elementDescriptions = currentItem.getAllElementDescription();
		functionNameForDecoding = currentItem.getAllFunctionNamesForDecodingInString();
		lsbValues = currentItem.getLsbValues();
		representationModeForDecoding = currentItem.getAllRepresentationModesInStringForDecoding();

		// Check number of elements names and elements description, they must be equivalent
		if (elementNamesToValidate->size() != elementDescriptions->size()) {
			cerr << "In " << fileName << " file - " << currentItem.getName()
													<< " - The size of 'elementNamesToValidate' and the size of 'elementDescriptions' are different." << endl;
			returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
			returnStatus->setMessage("Error while reading " + fileName + ".");
			return;
		}

		if ((currentItem.getItemType() == ItemTypeEnum::FixedLength) || (currentItem.getItemType() == ItemTypeEnum::RepetitiveLength)
				|| (currentItem.getItemType() == ItemTypeEnum::ExtendedLength)) {

			checkFixedRepetitiveExtendedItem(fileName, currentItem, returnStatus);

			// Checking all Element names of Compound item | They must be UNIQUE
			for (unsigned int innerIndex = 0; innerIndex < elementNamesToValidate->size(); innerIndex++) {
				elementName = elementNamesToValidate->at(innerIndex);

				if ((elementName != kSpare) && (elementName != kFX)) {
					if (find(allElementNames->begin(), allElementNames->end(), elementName) == allElementNames->end()) {
						// 'elementName' is UNIQUE -> it can be inserted in 'allElementNames' collection
						allElementNames->push_back(elementName);
					} else {
						// 'elementName' is NOT unique | it is already present in 'allElementNames' collection
						string message = "In " + fileName + " file - The Element '" + elementName + "' is duplicated.\n";
						returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
						returnStatus->setMessage(message);
						return;
					}
				}
			}

			// Vector sizes shall be EQUAL
			if (functionNameForDecoding->size() != elementDescriptions->size()) {
				cerr << "In " << fileName << " file - " << currentItem.getName()
														<< " - The size of 'functionNameForDecoding' and the size of 'elementDescriptions' are different." << endl;
				returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
				returnStatus->setMessage("Error while reading " + fileName + ".");
				return;
			} else if (lsbValues->size() != elementDescriptions->size()) {
				cerr << "In " << fileName << " file - " << currentItem.getName()
														<< " - The size of 'lsbValues' and the size of 'elementDescriptions' are different." << endl;
				returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
				returnStatus->setMessage("Error while reading " + fileName + ".");
				return;
			} else if (representationModeForDecoding->size() != elementDescriptions->size()) {
				cerr << "In " << fileName << " file - " << currentItem.getName()
														<< " - The size of 'representationModeForDecoding' and the size of 'elementDescriptions' are different." << endl;
				returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
				returnStatus->setMessage("Error while reading " + fileName + ".");
				return;
			}

		} else if (currentItem.getItemType() == ItemTypeEnum::CompoundLength) {
			if (checkIndexesInSubItemsDescription(fileName, currentItem, categoryDescription->getCompoundSubItemsDescription(), returnStatus) == false)
				return;

		} else if (currentItem.getItemType() == ItemTypeEnum::ExplicitLength && currentItem.getName() == kREF) {
			string message = "In " + fileName + " file - " + currentItem.getName()
													+ " - Wrong Item type: A Reserved Expansion Field CANNOT contain another REF.\n";
			returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			returnStatus->setMessage(message);
			return;

		} else if (currentItem.getItemType() == ItemTypeEnum::ExplicitLength && currentItem.getName() == kSPF) {
			string message = "In" + fileName + " file - " + currentItem.getName()
													+ " - Wrong Item type: A Reserved Expansion Field CANNOT contain another SPF.\n";
			returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			returnStatus->setMessage(message);
			return;

			//This point is unreachable, the syntax check of autoJSON not accept type not register in 'STATICJSON_DECLARE_ENUM' macro in 'ExternalItem' class
		} else if (currentItem.getItemType() != ItemTypeEnum::ExplicitLength) {
			string message = fileName + " - " + currentItem.getName() + " - Unknown Item type [could be:  extended, repetitive, compound, fixed, explicit - case matters!].\n";
			returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			returnStatus->setMessage("Error while reading " + fileName + ".");
			return;
		}
	}
}

void JsonCategoriesDescriptionValidator::checkFixedRepetitiveExtendedItem(string fileName, Item item, ReturnStatus *status) {
	/************************
	 * Variable definitions *
	 ************************/
	vector<string>* functionNameForDecoding = nullptr;
	vector<string>* representationModeForDecoding = nullptr;
	int sumOfElementLengthsInBit = 0, itemLengthInByte = 0;

	vector<string>* supportedFunctionNameForDecoding = commonVariables->getSupportedFunctionNameForDecoding();
	vector<string>* supportedRepresentationModeForDecoding = commonVariables->getRepresentationModeForDecodingList();

	itemLengthInByte = item.getLengthInByte();
	sumOfElementLengthsInBit = item.getElementsTotalSize();

	if (item.getItemType() == ItemTypeEnum::ExtendedLength) {

		// The sum of Element lengths in bit SHALL BE multiple of 8. If not, throw an error!
		if (sumOfElementLengthsInBit % 8 != 0) {
			string message = "In " + fileName + " file - " + item.getName()
													+ " - The sum of its Element lengths (in bit) is not an exact multiple of 8, so it can't stay on an integer number of Bytes.\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}
	} else {
		// Size in bytes shall be equal to the sum of size in bits
		if (((sumOfElementLengthsInBit / 8) != itemLengthInByte) || (sumOfElementLengthsInBit % 8 != 0)) {
			string message =
					"In " + fileName + " file - " + item.getName()
					+ " - The sum of its Element lengths (in bit) and its length (in Byte) don't match OR the sum of its Element lengths (in bit) is not an exact multiple of 8.\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}
	}

	functionNameForDecoding = item.getAllFunctionNamesForDecodingInString();
	representationModeForDecoding = item.getAllRepresentationModesInStringForDecoding();

	// Check if 'functionNameForDecoding' of each Element of the current Item exist in the 'supportedFunctionNameForDecoding' list
	for (vector<string>::const_iterator it = functionNameForDecoding->begin(); it != functionNameForDecoding->end(); it++) {
		if (find(supportedFunctionNameForDecoding->begin(), supportedFunctionNameForDecoding->end(), *it) == supportedFunctionNameForDecoding->end()) {
			// The listed 'functionNameForDecoding' has NOT been found in the list of known decoding functions | Throw an error!
			string message = "In " + fileName + " file - " + item.getName() + " - function '" + *it + "' is not a known Decoding Function name.\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}
	}

	// Check if 'representationModeForDecoding' of each Element of the current Item exist in the 'supportedRepresentationModeForDecoding' list
	for (vector<string>::const_iterator it = representationModeForDecoding->begin(); it != representationModeForDecoding->end(); it++) {
		if (find(supportedRepresentationModeForDecoding->begin(), supportedRepresentationModeForDecoding->end(), *it)
				== supportedRepresentationModeForDecoding->end()) {
			// The listed 'representationModeForDecoding' has NOT been found in the list of known representation modes | Throw an error!
			string message = "In " + fileName + " file - " + item.getName() + " - mode '" + *it + "' is not a known Representation Mode.\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}
	}
}

bool JsonCategoriesDescriptionValidator::checkIndexesInSubItemsDescription(string fileName, Item itemToCheck, vector<Item>* subitemsDescription, ReturnStatus *returnStatus) {
	/************************
	 * Variable definitions *
	 ************************/
	Item currentCompoundItem;
	unsigned int indexInsideSubItem = 0;

	vector<string>* allElementNames = itemToCheck.getAllElementNames();
	vector<int>* allElementDescriptions = itemToCheck.getAllElementDescription();

	string elementName_itemCollection;
	string elementName_compoundItemCollection;

	for (unsigned int index = 0; index < allElementDescriptions->size(); index++) {
		// Checking that in the compound JSON the name at (numberInDescription) is equal to Name of the Element in CAT JSON
		indexInsideSubItem = (*allElementDescriptions)[index];
		elementName_itemCollection = (*allElementNames)[index];

		if (indexInsideSubItem >= subitemsDescription->size()) {
			stringstream message;
			message << "In " << fileName << " file - " << itemToCheck.getName() << " .::. Index of Element " << (*allElementNames)[index]
																																   << " does not match a subitem description - position " << indexInsideSubItem << " is out of range." << endl;
			returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			returnStatus->setMessage(message.str());
			return false;
		}

		elementName_compoundItemCollection = subitemsDescription->at(indexInsideSubItem).getName();

		if (elementName_itemCollection != elementName_compoundItemCollection) {
			stringstream message;
			message << "In " << fileName << " file - " << itemToCheck.getName() << " .::. " << (*allElementNames)[index]
																												  << " in CAT file does not match with compound list - position " << indexInsideSubItem << ". Erroneous name "
																												  << subitemsDescription->at(indexInsideSubItem).getName() << endl;
			returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			returnStatus->setMessage(message.str());
			return false;
		}
	}
	return true;
}

// ** Check and prepare object from JSON **
void JsonCategoriesDescriptionValidator::checkAndPrepareCategoriesFilterList(string categoriesFilterPath, bool isDecoderFilter, CategoriesToFilter& categoriesToFilter, ReturnStatus* status){

	std::vector<int> listOfCategoryFilterInDecimal;			// Store the final list of Category to filter
	std::vector<int> listOfCategoryAndDatasourcesToFilter;	// Store the final list of Category + datasource to filter


	string currentDatasourceToFilter;
	std::vector<std::string> currentListOfDatasourcesToFilter;

	FilterType filterType = categoriesToFilter.getTypeOfFiltering();
	std::vector<CategoryFilter> listOfCategoryFilter = categoriesToFilter.getListOfCategoryFilter();

	// Check the correctness of 'filterType' (only inclusive or exclusive keywords are permitted)
	if (filterType != FilterType::ExclusiveFilter && filterType != FilterType::InclusiveFilter) {
		string message;

		if (isDecoderFilter) {
			message = "Wrong filter type 'typeOfFiltering' for 'decoderFilter'. Valid values are 'inclusive' or 'exclusive'. \nError while loading "
					+ categoriesFilterPath + " file.";
		} else {
			message = "Wrong filter type 'typeOfFiltering' for 'encoderFilter'. Valid values are 'inclusive' or 'exclusive'.\nError while loading " + categoriesFilterPath
			+ " file.";
		}

		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	// Scan all CategoryFilter from 'CategoriesToFilter' object
	for (CategoryFilter categoryFilter : listOfCategoryFilter) {

		//Check Category number is grater than 0
		if (categoryFilter.getCategoryNumber() <= 0) {
			string message;

			if(isDecoderFilter)
				message = "Category number is " + to_string(categoryFilter.getCategoryNumber()) + " for 'decoderFilter'. Valid values are grater than 0. \nError while loading "
				+ categoriesFilterPath + " file.";
			else
				message =  "Category number is " + to_string(categoryFilter.getCategoryNumber()) + " for 'encoderFilter'. Valid values are grater than 0. \nError while loading "
				+ categoriesFilterPath + " file.";

			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}

		currentListOfDatasourcesToFilter = categoryFilter.getListOfDatasourcesToFilter();

		// If current list of datasources does not contain datasources -> add the Category Number to the list of Category to filter
		if (currentListOfDatasourcesToFilter.size() == 0) {
			listOfCategoryFilterInDecimal.push_back(categoryFilter.getCategoryNumber());

		} else {

			// If current list of datasources contains datasources -> remove the '-' and add the CatNumber+SAC+SIC in the list of Category to filter
			for (unsigned int i = 0; i < currentListOfDatasourcesToFilter.size(); i++) {

				currentDatasourceToFilter = currentListOfDatasourcesToFilter.at(i);

				if (currentDatasourceToFilter == "") {
					string message = "Datasource in position " + to_string(i) + " has an empty name in the 'dataSourcesToFilter' field.\nError while reading: "
							+ categoriesFilterPath + " file\n";
					status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
					status->setMessage(message);
					return;
				}

				listOfCategoryAndDatasourcesToFilter.push_back(
						splitDatasourceString(currentDatasourceToFilter, categoryFilter.getCategoryNumber(), categoriesFilterPath, status));
			}
		}
	}

	// If 'listOfCategoryFilterInDecimal' and 'listOfCategoryFilterInDecimal' sizes == 0, I want to encode/decode all Categories -> put the 'filterType' equal to ExclusiveFilter
	// -> ExclusiveFilter + empty list = decode/encode all Items except Items in the list (is empty) -> encode/decode all Items
	if (listOfCategoryFilterInDecimal.size() == 0 && listOfCategoryAndDatasourcesToFilter.size() == 0) {
		categoriesToFilter.setTypeOfFiltering(FilterType::ExclusiveFilter);
	}

	categoriesToFilter.setListOfCategoryInDecimalToFilter(listOfCategoryFilterInDecimal);
	categoriesToFilter.setListOfCategoryAndDatasourcesInDecimalToFilter(listOfCategoryAndDatasourcesToFilter);
}

void JsonCategoriesDescriptionValidator::checkAndPrepareUapAndMandatoryItemsList(string uapAndMandatoryItemsFile, UapAndMandatoryItems& uapAndMandatoryItems, ReturnStatus* status) {
	/************************
	 * Variable definitions *
	 ************************/
	std::vector<std::string>* uapList = nullptr;
	std::vector<std::string>* mandatoryItems = nullptr;
	std::vector<std::string>::iterator uapListIteratorBegin;
	std::string* itemName = nullptr;

	int indexOfFXtoRemove = 0;
	unsigned int uapListSize = 0;
	unsigned int mandatoryItemListSize = 0;

	uapList = uapAndMandatoryItems.getPointerToUapList();
	mandatoryItems = uapAndMandatoryItems.getPointerToMandatoryItems();

	/******************
	 * Check UAP list *
	 ******************/
	uapListSize = uapList->size();

	if (uapListSize == 0){
		string message = "UAP list is empty.\nError while reading: " + uapAndMandatoryItemsFile + " file\n";
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	// If a string is empty -> error
	// Every Item name shall be in Uppercase
	// If Item name is 'REF', convert in 'RE'
	// If Item name is 'SPF', convert in 'SP'
	// Every 7 string - one FX - otherwise -> error
	for (unsigned int i = 0; i < uapListSize; i++) {
		itemName = &(uapList->at(i));

		if ((*itemName) == "") {
			string message = "An Item name in the UAP is empty.\nError while reading: " + uapAndMandatoryItemsFile + " file\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;

		} else {
			transform(itemName->begin(), itemName->end(), itemName->begin(), ::toupper);
		}

		if((*itemName) == "SPF")
			*itemName = kSPF;

		if((*itemName) == "REF")
			*itemName = kREF;

		if( ((i+1) % 8 == 0) && (*itemName) != kFX){
			string message = "A " + kFX + " string is missing in " + uapAndMandatoryItemsFile+" file.\nError while reading: " + uapAndMandatoryItemsFile+" file\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}
	}

	// UAP list shall be multiple of 8
	if (uapListSize % 8 != 0) {
		string message = "UAP list in incomplete: there are " + to_string(uapListSize)
												+ " items but expected the size of the list shall be multiple of 8.\nError while reading: " + uapAndMandatoryItemsFile + " file\n";
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	uapListIteratorBegin = uapList->begin();

	// Remove all FXs from the UAP - the FRN numbers do not include FXs
	for (auto it = uapList->end() - 1; it >= uapListIteratorBegin; it--) {
		if (*it == kFX) {
			indexOfFXtoRemove = it - uapListIteratorBegin;
			uapList->erase(uapList->begin() + indexOfFXtoRemove);
		}
	}

	/******************************
	 * Check mandatory Items list *
	 ******************************/
	mandatoryItemListSize = mandatoryItems->size();

	if (mandatoryItemListSize == 0) {
		// Return without errors because "mandatory" items list in JSON file is not mandatory
		return;
	}

	// If a string is empty -> error
	// Every name in Uppercase
	// If Item name is 'REF', convert in 'RE'
	// If Item name is 'SPF', convert in 'SP'
	for (unsigned int i = 0; i < mandatoryItemListSize; i++) {
		itemName = &(mandatoryItems->at(i));

		if ((*itemName) == "") {
			string message = "An Item name in the mandatory Items list is empty.\nError while reading: " + uapAndMandatoryItemsFile + " file\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;

		} else {
			transform(itemName->begin(), itemName->end(), itemName->begin(), ::toupper);
		}

		if((*itemName) == "SPF")
			*itemName = kSPF;

		if((*itemName) == "REF")
			*itemName = kREF;
	}
}

void JsonCategoriesDescriptionValidator::checkAndPrepareExternalItemsDescription(string itemsDescriptionFilePath, ExternalCategoryDescription& extItemsDescription, ReturnStatus* status){
	/************************
	 * Variable definitions *
	 ************************/
	vector<ExternalItem>* itemsDescription = nullptr;
	vector<ExternalItem>* compoundSubItemsDescription = nullptr;
	std::string* itemName = nullptr;
	ExternalItem* item = nullptr;

	unsigned int itemsDescriptionSize = 0;
	unsigned int compoundSubItemsDescriptionSize = 0;

	itemsDescription = extItemsDescription.getPointerToItemCollection();
	compoundSubItemsDescription = extItemsDescription.getPointerToCompoundSubitemsCollection();

	/***************************
	 * Check Items Description *
	 ***************************/
	itemsDescriptionSize = itemsDescription->size();

	if (itemsDescriptionSize == 0){
		string message = "'itemCollection' shall not be empty.\nError while reading: " + itemsDescriptionFilePath + "file\n";
		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	// If a string is empty -> error
	// Every name in Uppercase
	// number of elements shall be > 0
	for (unsigned int i = 0; i < itemsDescriptionSize; i++) {
		item = &(itemsDescription->at(i));
		itemName = item->getPointerToName();

		if ((*itemName) == "") {
			string message = "Item in position " + to_string(i) + " has an empty name in the 'itemCollection' field.\nError while reading: "
					+ itemsDescriptionFilePath + " file\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;

		} else {
			transform(itemName->begin(), itemName->end(), itemName->begin(), ::toupper);
		}

		/*if (((*itemName) == "SPF") || ((*itemName) == "SP")) {
		 string message = "In " + itemsDescriptionFilePath + " file: the Item '" + (*itemName)
		 + "' should not be here. It needs an external JSON description.\n";
		 status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		 status->setMessage(message);
		 return;
		 } else if (((*itemName) == "REF") || ((*itemName) == "RE")) {
		 string message = "In " + itemsDescriptionFilePath + " file: the Item '" + (*itemName)
		 + "' should not be here. It needs an external JSON description.\n";
		 status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		 status->setMessage(message);
		 return;
		 }*/

		if(item->getPointerToElementDescriptions()->size() == 0){
			stringstream message;
			message << "Error in the Item " << *itemName << " - the vector of elements shall not be empty.\nError while reading: " << itemsDescriptionFilePath
					<< " file" << endl;
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message.str());
			return;
		}

		checkAndPrepareItemElements(itemsDescriptionFilePath, *itemName,item->getPointerToElementDescriptions(), status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}
	}

	/**************************************
	 * Check Compound SubItem Description *
	 **************************************/
	compoundSubItemsDescriptionSize = compoundSubItemsDescription->size();

	// If a string is empty -> error
	// Every name in Uppercase
	// number of element > 0
	for (unsigned int i = 0; i < compoundSubItemsDescriptionSize; i++) {
		item = &(compoundSubItemsDescription->at(i));
		itemName = item->getPointerToName();

		if ((*itemName) == "") {
			string message = "Item in position " + to_string(i) + " has an empty name in the 'compoundSubitemCollection' filed.\nError while reading: "
					+ itemsDescriptionFilePath + " file\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}

		if(item->getPointerToElementDescriptions()->size() == 0){
			stringstream message;
			message << "Error in the Item " << *itemName << " - the vector of elements shall not be empty.\nError while reading: "
					<< itemsDescriptionFilePath + " file\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message.str());
			return;
		}

		checkAndPrepareItemElements(itemsDescriptionFilePath,*itemName,item->getPointerToElementDescriptions(), status);
		if (status->getCode() != ReturnCodes::SUCCESS) {
			return;
		}
	}
}

void JsonCategoriesDescriptionValidator::checkAndPrepareItemElements(string itemsDescriptionFilePath, string itemName, vector<vector<string>>* itemElements, ReturnStatus* status){
	unsigned int elementStringSize = 0;
	unsigned int itemElementsSize = 0;
	vector<string>* elementString = nullptr;
	string* elementName = nullptr;

	itemElementsSize = itemElements->size();

	// If case insensitive is 'FX' -> element name became 'FX'
	// If case insensitive is 'Spare' -> element name became 'Spare'
	// Number of string per element must be at least 'kMinumumSizeOfExtElementDescription' - otherwise -> error
	for(unsigned int i = 0; i< itemElementsSize; i++){
		// Get the description of one element
		elementString = &(itemElements->at(i));

		elementStringSize = elementString->size();


		if (elementStringSize < kMinimumSizeOfExtElementDescription) {
			string message = "Error in the Item " + itemName + " - vector size of one of the elements has less than "
					+ to_string(kMinimumSizeOfExtElementDescription) + " values.\n" + "\nError while reading: "
					+ itemsDescriptionFilePath + " file\n";
			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;
		}

		elementName = &(elementString->at(0));

		if (strcasecmp((*elementName).c_str(), kFX.c_str()) == 0) {
			*elementName = kFX;
		}

		if (strcasecmp((*elementName).c_str(), kSpare.c_str()) == 0) {
			*elementName = kSpare;
		}
	}
}

void JsonCategoriesDescriptionValidator::checkAndPrepareFilterList(string filterPath, bool isDecoderFilter, FilterType& filterType, std::vector<std::string>* filterList, ReturnStatus* status){
	unsigned int filterListSize = 0;
	string* itemToFilterName;

	// Check the correctness of 'filterType'
	if (filterType != FilterType::ExclusiveFilter && filterType != FilterType::InclusiveFilter) {
		string message;

		if(isDecoderFilter)
			message = "Wrong filter type 'typeOfFiltering' for 'decoderFilter'. Valid values are 'inclusive' or 'exclusive'. \nError while loading "
					+ filterPath + " file.";
		else
			message = "Wrong filter type 'typeOfFiltering' for 'encoderFilter'. Valid values are 'inclusive' or 'exclusive'.\nError while loading " + filterPath
			+ " file.";

		status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		status->setMessage(message);
		return;
	}

	// If 'filterList' size == 0, I want to encode/decode all Items -> put the 'filterType' equal to ExclusiveFilter
	// -> ExclusiveFilter + empty list = decode/encode all Items except Items in the list (is empty) -> encode/decode all Items
	if (filterList->size() == 0) {
		filterType = FilterType::ExclusiveFilter;
	}

	filterListSize = filterList->size();

	// If a string is empty -> error
	// Every Item name in Uppercase
	// If Item name is 'REF', convert in 'RE'
	// If Item name is 'SPF', convert in 'SP'
	for (unsigned int i = 0; i < filterListSize; i++) {
		itemToFilterName = &(filterList->at(i));

		if ((*itemToFilterName) == "") {
			string message;

			if(isDecoderFilter)
				message = "An Item name in the 'decoderFilter' is empty.\nError while loading " + filterPath + " file.";
			else
				message = "An Item name in the 'encoderFilter' is empty.\nError while loading " + filterPath + " file.";

			status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
			status->setMessage(message);
			return;

		} else {
			transform(itemToFilterName->begin(), itemToFilterName->end(), itemToFilterName->begin(), ::toupper);
		}

		if((*itemToFilterName) == "SPF")
			*itemToFilterName = kSPF;

		if((*itemToFilterName) == "REF")
			*itemToFilterName = kREF;
	}

	// Remove 'REF' and 'SPF' from 'filterList'-> enable or disable of 'REF' and 'SPF' is performed by the variables in CodecInterface
	std::vector<std::string>::iterator itemToRemoveFromList;

	if ((itemToRemoveFromList = find(filterList->begin(), filterList->end(), kREF)) != filterList->end()) {
		int index = itemToRemoveFromList - filterList->begin();
		filterList->erase(filterList->begin() + index);
	}

	if ((itemToRemoveFromList = find(filterList->begin(), filterList->end(), kSPF)) != filterList->end()) {
		int index = itemToRemoveFromList - filterList->begin();
		filterList->erase(filterList->begin() + index);
	}
}
