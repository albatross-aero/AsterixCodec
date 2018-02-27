#include "utils/asterix_json_description/JsonCategoriesDescriptionLoader.h"

using namespace std;

// Declaration of static fields
CommonVariables* JsonCategoriesDescriptionLoader::commonVariables = CommonVariables::getInstance();

void JsonCategoriesDescriptionLoader::loadAsterixDescription(string& asterixConfiguration_fpath,
                                                             unordered_map<int, CategoryDescription*>* uapMap,
                                                             ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
    AsterixConfiguration asterixConfiguration;
    struct stat file_probe { };   // used to check file existence
    unsigned int supportedAsterixCategories_counter = 0;
    unsigned short int categoryNumber = 0;

    CategoryDescription* categoryDescription = nullptr;
    UapAndMandatoryItems uap_mandatoryItems;

    ExternalCategoryDescription itemsDescription;
    ExternalCategoryDescription refItemsDescription;
    ExternalCategoryDescription spfItemsDescription;

    /********************************************************************************************
     * Loading of 'AsterixConfiguration.json' file content into 'asterixConfiguration' variable *
     ********************************************************************************************/
    if (stat(asterixConfiguration_fpath.c_str(), &file_probe) != 0) {    // Check 'asterixConfiguration.json' existence
        stringstream message;
        message << "The ASTERIX Configuration file (expected at " << asterixConfiguration_fpath << ") is MISSING.";
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return;
    }

    try {
        std::ifstream i(asterixConfiguration_fpath);
        json j;
        i >> j;
        asterixConfiguration = j;
    } catch (json::parse_error& e) {
        stringstream message;
        message << "Error while reading the ASTERIX Configuration file, located at '" << asterixConfiguration_fpath <<
                "': invalid JSON format." << endl << e.what() << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    /***********************
     * For EACH category.. *
     ***********************/
    supportedAsterixCategories_counter = (unsigned int) asterixConfiguration.getCategoriesConfiguration().size();

    for (unsigned int i = 0; i < supportedAsterixCategories_counter; i++) {
        categoryDescription = new CategoryDescription();

        CategoryConfiguration categoryConfiguration = asterixConfiguration.getCategoriesConfiguration().at(i);
        categoryNumber = categoryConfiguration.getCategoryNumber();

        if (categoryNumber <= 0) {
            stringstream message;
            message << "Error while reading ASTERIX Configuration file, located at '" << asterixConfiguration_fpath <<
                    "'" << endl << "Category number: " << categoryNumber << "This value SHALL be > 0" << endl;
            status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
            status->setMessage(message.str());
            return;
        }

        const string& itemsDescription_fpath = categoryConfiguration.getItemsDescriptionFile();
        const string& uapDescription_fpath = categoryConfiguration.getUapAndMandatoryItemsFile();
        const string& refDescription_fpath = categoryConfiguration.getRefDescriptionFile();
        const string& spfDescription_fpath = categoryConfiguration.getSpfDescriptionFile();

        /***********************************************
        * #1) Reading of UAP and mandatory items list *
        ***********************************************/
        loadUapAndMandatoryItemsList(asterixConfiguration_fpath, uapDescription_fpath, uap_mandatoryItems, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }
        vector<string> uap = uap_mandatoryItems.getUapList();

        /***********************************************************************
        * #2) Reading of Items description (both normal and compound subitems) *
        ************************************************************************/
        loadItemsDescription(asterixConfiguration_fpath, itemsDescription_fpath, itemsDescription, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        /******************************************************
        * #3) Reading of Reserved Expansion Field Description *
        *******************************************************/
        loadReservedExpansionFieldDescription(asterixConfiguration_fpath, refDescription_fpath, refItemsDescription,
                                              status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        /****************************************************
         * #4) Reading of Special Purpose Field Description *
         ****************************************************/
        loadSpecialPurposeFieldDescription(asterixConfiguration_fpath, spfDescription_fpath, spfItemsDescription,
                                           status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        /***********************************************************************
        * #5) MARSHALLING from the extCategoryDescription to the internal one *
        ***********************************************************************/
        performMarshallingFromExternalToInternalDescription(uap_mandatoryItems, itemsDescription, refItemsDescription,
                                                            spfItemsDescription, categoryDescription, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        /**********************************************
        * Reordering the Item list, following the UAP *
        ***********************************************/
        putInOrderItemsDescription(uapDescription_fpath, uap, categoryDescription, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        // Filling the 'mandatoryItemFRNs' structure of Category Description class
        long FRN_mandatoryItem = 0;
        for (const auto& mandatoryItemName : uap_mandatoryItems.getMandatoryItems()) {
            auto mandatoryItemName_it = find(uap.begin(), uap.end(), mandatoryItemName);
            if (mandatoryItemName_it != uap.end()) {
                // +1 is required due to the fact that FRN starts from 1
                FRN_mandatoryItem = (mandatoryItemName_it - uap.begin()) + 1;
                categoryDescription->getMandatoryItemFRNs()->push_back((unsigned short) FRN_mandatoryItem);
            }
        }

        /****************************************
         * VALIDATION of JSON description files *
         ****************************************/
        JsonCategoriesDescriptionValidator::performValidationOfJsonFiles(itemsDescription_fpath,
                                                                         refDescription_fpath,
                                                                         spfDescription_fpath,
                                                                         categoryDescription, status);
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

        /*****************************************************************
         *  Saving the new Category description into the description map *
         *****************************************************************/
        // If Description is set to default, it gets only the category number the key in the map of Asterix Description
        if (asterixConfiguration.getCategoriesConfiguration().at(i).isCategoryByDefault()) {
            //Check if the category exists already
            if (JsonCategoriesDescriptionValidator::checkIfKeyInCategoryDescriptionMapExists(categoryNumber, uapMap)) {
                stringstream message;
                message << "Error while reading ASTERIX Configuration file: " << asterixConfiguration_fpath << endl <<
                        "Category already declared: " << categoryNumber << endl;
                status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
                status->setMessage(message.str());
                return;
            } else {
                // Add new Category description into a map because it does not exist
                (*uapMap).insert(pair<int, CategoryDescription*>(categoryNumber, categoryDescription));
            }
        }

        /* If there are data sources specified for this description, in the Asterix Description map they get a key
         * built as: {<CategoryNumber><SAC><SIC>} (e.g. 2112053, where CAT=21, SAC=120 and SIC=53) */
        //FIXME: replace the following variable definitions
        unsigned short int numberOfDatasources_usingThisDescription = 0;
        int CATno_SAC_SIC_decimal = 0;

        numberOfDatasources_usingThisDescription = (unsigned short int)
                asterixConfiguration.getCategoriesConfiguration().at(i).getDataSourcesInDec().size();

        if (numberOfDatasources_usingThisDescription > 0) {
            for (unsigned int j = 0; j < numberOfDatasources_usingThisDescription; j++) {
                CATno_SAC_SIC_decimal = JsonCategoriesDescriptionValidator::splitDatasourceString(
                        asterixConfiguration_fpath, categoryNumber,
                        asterixConfiguration.getCategoriesConfiguration().at(i).getDataSourcesInDec().at(j), status);
                if (status->getCode() != ReturnCodes::SUCCESS) {
                    return;
                }

                //Check if the category-datasource exists already
                if (JsonCategoriesDescriptionValidator::checkIfKeyInCategoryDescriptionMapExists(CATno_SAC_SIC_decimal,
                                                                                                 uapMap)) {
                    stringstream message;
                    message << "Error while reading ASTERIX Configuration file: " << asterixConfiguration_fpath <<
                            "Datasource " <<
                            asterixConfiguration.getCategoriesConfiguration().at(i).getDataSourcesInDec().at(j) <<
                            " already declared for Category " << categoryNumber << "." << endl;
                    status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
                    status->setMessage(message.str());
                    return;
                }

                // Adding the NEW Category description into the map..
                (*uapMap).insert(pair<int, CategoryDescription*>(CATno_SAC_SIC_decimal, categoryDescription));
            }
        }

        // Cleaning up the collections used while loading a single Category description
        itemsDescription.getPointerToItemCollection()->clear();
        itemsDescription.getPointerToCompoundSubitemsCollection()->clear();
        refItemsDescription.getPointerToItemCollection()->clear();
        refItemsDescription.getPointerToCompoundSubitemsCollection()->clear();
        spfItemsDescription.getPointerToItemCollection()->clear();
        spfItemsDescription.getPointerToCompoundSubitemsCollection()->clear();
    }
}

void JsonCategoriesDescriptionLoader::loadUapAndMandatoryItemsList(const string& asterixConfiguration_fpath,
                                                                   const string& uapDescription_fpath,
                                                                   UapAndMandatoryItems& uapAndMandatoryItems,
                                                                   ReturnStatus* status) {
    ifstream uapAndMandatoryItems_fp;
    struct stat file_probe { };   // used to check file existence

    // Check if file exists..
    if (uapDescription_fpath.empty() || stat(uapDescription_fpath.c_str(), &file_probe) != 0) {
        stringstream message;
        message << "Error while opening a file, listed in the current ASTERIX Configuration ('" <<
                asterixConfiguration_fpath << ")" << endl << "'uapAndMandatoryItemsFile.json' file is required and " <<
                "it has NOT been found in ";
        if (uapDescription_fpath.empty()) {
            message << "'<empty_path>'." << endl;
        } else {
            message << "'" << uapDescription_fpath << "'." << endl;
        }

        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return;
    }

    // File opening..
    uapAndMandatoryItems_fp.open(uapDescription_fpath);
    if (!uapAndMandatoryItems_fp) {
        stringstream message;
        message << "Error while opening '" << uapDescription_fpath << "'" << endl <<
                "Check or add the file path because it is mandatory." << endl;
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return;
    }

    // Content loading..
    try {
        std::ifstream i(uapDescription_fpath);
        json j;
        i >> j;
        uapAndMandatoryItems = j;
    } catch (json::parse_error& e) {
        stringstream message;
        message << "Error while reading '" << uapDescription_fpath << "' file, it is not a valid JSON." <<
                endl << e.what() << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    //TODO: validation of 'uapAndMandatoryItems.json' content - TO BE REVIEWED
    // Check the correctness of file and remove FX string from the UAP list
    JsonCategoriesDescriptionValidator::checkAndPrepareUapAndMandatoryItemsList(uapDescription_fpath,
                                                                                uapAndMandatoryItems, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }
    //TODO: Please handle the case "status != SUCCESS", after loading the UAP and the mandatory items list
}

void JsonCategoriesDescriptionLoader::loadItemsDescription(const string& asterixConfiguration_fpath,
                                                           const string& itemsDescription_fpath,
                                                           ExternalCategoryDescription& extCategoryDescription,
                                                           ReturnStatus* status) {
    // Check if file exists..
    struct stat file_probe { };   // used to check file existence
    if (stat(itemsDescription_fpath.c_str(), &file_probe) != 0) {    // Check the existence of the REF description
        stringstream message;
        message << "Error while opening a file, listed in the current ASTERIX Configuration ('" <<
                asterixConfiguration_fpath << ")" << endl << "'catXXX.json' file is required and " <<
                "it has NOT been found in '" << itemsDescription_fpath << "'." << endl;
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return;
    }

    // Content loading..
    try {
        std::ifstream i(itemsDescription_fpath);
        json j;
        i >> j;
        extCategoryDescription = j;
    } catch (json::parse_error& e) {
        stringstream message;
        message << "Error while reading '" << itemsDescription_fpath << "' file, it is not a valid JSON." <<
                endl << e.what() << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    //TODO: validation of 'catXXX.json' content - TO BE REVIEWED
    JsonCategoriesDescriptionValidator::checkAndPrepareExternalItemsDescription(itemsDescription_fpath,
                                                                                extCategoryDescription, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }
}

void JsonCategoriesDescriptionLoader::loadReservedExpansionFieldDescription(const string& asterixConfiguration_fpath,
                                                                            const string& refDescription_fpath,
                                                                            ExternalCategoryDescription& refItemsDescription,
                                                                            ReturnStatus* status) {
    if (refDescription_fpath.empty()) {
        // ..this file is optional, no need to log an error if the field has been left empty..
        return;
    }
    // Check if file exists..
    struct stat file_probe { };   // used to check file existence
    if (stat(refDescription_fpath.c_str(), &file_probe) != 0) {    // Check the existence of the REF description
        stringstream message;
        message << "Error while opening a file, listed in the current ASTERIX Configuration ('" <<
                asterixConfiguration_fpath << ")" << endl << "'catXXX_ref.json' file (optional) " <<
                "has NOT been found in '" << refDescription_fpath << "'." << endl;
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return;
    }

    // Content loading..
    try {
        std::ifstream i(refDescription_fpath);
        json j;
        i >> j;
        refItemsDescription = j;
    } catch (json::parse_error& e) {
        stringstream message;
        message << "Error while reading ASTERIX Configuration file: '" << refDescription_fpath <<
                "': it is NOT a valid JSON." << endl << e.what() << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    JsonCategoriesDescriptionValidator::checkAndPrepareExternalItemsDescription(refDescription_fpath,
                                                                                refItemsDescription,
                                                                                status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }
}

void JsonCategoriesDescriptionLoader::loadSpecialPurposeFieldDescription(const string& asterixConfiguration_fpath,
                                                                         const string& spfDescription_fpath,
                                                                         ExternalCategoryDescription& spfItemsDescription,
                                                                         ReturnStatus* status) {
    if (spfDescription_fpath.empty()) {
        // ..this file is optional, no need to log an error if the field has been left empty..
        return;
    }
    // Check if file exists..
    struct stat file_probe { };   // used to check file existence
    if (stat(spfDescription_fpath.c_str(), &file_probe) != 0) {    // Check the existence of the REF description
        stringstream message;
        message << "Error while opening a file, listed in the current ASTERIX Configuration ('" <<
                asterixConfiguration_fpath << ")" << endl << "'catXXX_spf.json' file (optional) " <<
                "has NOT been found in '" << spfDescription_fpath << "'." << endl;
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return;
    }

    // Check if file exists..
    if (spfDescription_fpath.empty()) {
        // ..this file is optional, no need to log an error if the field has been left empty..
        return;
    }

    // Content loading..
    try {
        std::ifstream i(spfDescription_fpath);
        json j;
        i >> j;
        spfItemsDescription = j;
    } catch (json::parse_error& e) {
        stringstream message;
        message << "Error while reading ASTERIX Configuration file: '" << spfDescription_fpath <<
                "': it is NOT a valid JSON." << endl << e.what() << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    JsonCategoriesDescriptionValidator::checkAndPrepareExternalItemsDescription(spfDescription_fpath,
                                                                                spfItemsDescription,
                                                                                status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }
}

void JsonCategoriesDescriptionLoader::putInOrderItemsDescription(string uapDescription_fpath, vector<string> uapList,
                                                                 CategoryDescription* categoryDescription,
                                                                 ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
    vector<Item>* itemCollection_toOrder = nullptr;
    vector<Item> itemCollection_ordered;
    unsigned int itemCollection_toOrder_size = 0;
    unsigned long currentItemInUAP_index = 0;
    vector<string>::iterator itemNameInUAP_it;

    itemCollection_toOrder = categoryDescription->getItemCollection();
    itemCollection_toOrder_size = (unsigned int) itemCollection_toOrder->size();
    itemCollection_ordered = vector<Item>(uapList.size());

    // Scanning each Item of 'itemCollection_toOrder', looking for the Item name in 'uapList'
    for (unsigned int i = 0; i < itemCollection_toOrder_size; i++) {
        /* If the current Item name does NOT exist in the 'uapList',
         * the current Item shalll not be part of the final collection */
        if ((itemNameInUAP_it = find(uapList.begin(),
                                     uapList.end(),
                                     itemCollection_toOrder->at(i).getName())) != uapList.end()) {
            /* The current Item is present in the 'uapList'.. so let's put it in the 'itemCollection_ordered' collection.
             * After this step, the name of the current Item in the 'uapList' collection is set to an empty string,
             * to mark it as "done". A specific check on this will be done at the end of the ordering action. */
            currentItemInUAP_index = (unsigned long) (itemNameInUAP_it - uapList.begin());
            itemCollection_ordered.at(currentItemInUAP_index) = itemCollection_toOrder->at(i);
            uapList.at(currentItemInUAP_index) = "";
        }
    }

    // Creating Item for the REF
    Item REF_item;

    // If the REF description exists, let's create the corresponding Item for the Item collection
    if (!categoryDescription->getRefItemDescription()->empty()) {
        REF_item = Item(kREF, 1, ItemTypeEnum::ExplicitLength);

        vector<int> REF_elementsDescription;
        vector<string> REF_elementsName;
        Item REF_currentItem;

        /* Scanning each 'REF_currentItem' of REF description and putting the index and the name
         * in the 'elementsDescription' and 'elementsName', these two vectors will be part of the 'REF_item' object */
        for (unsigned int j = 0; j < categoryDescription->getRefItemDescription()->size(); j++) {
            REF_currentItem = categoryDescription->getRefItemDescription()->at(j);
            REF_elementsName.push_back(REF_currentItem.getName());
            REF_elementsDescription.push_back(j);
        }
        REF_item.setAllElementNames(REF_elementsName);
        REF_item.setAllElementDescription(REF_elementsDescription);
    } else {
        // If the REF description does NOT exist, let's create a place-holder for the Item collection
        REF_item = Item(kREF, 1, ItemTypeEnum::ExplicitLength);
    }

    //Create Item for the SPF
    Item SPF_item;

    //If exists the description of SPF, create the corresponding Item for the Item collection
    if (!categoryDescription->getSpfItemDescription()->empty()) {
        SPF_item = Item(kSPF, 1, ItemTypeEnum::ExplicitLength);

        vector<int> SPF_elementsDescription;
        vector<string> SPF_elementsName;
        Item SPF_currentItem;

        /* Scanning each 'SPF_currentItem' of SPF description and putting the index and the name
         * in the 'elementsDescription' and 'elementsName', these two vectors will be part of the 'SPF_item' object */
        for (unsigned int j = 0; j < categoryDescription->getSpfItemDescription()->size(); j++) {
            SPF_currentItem = categoryDescription->getSpfItemDescription()->at(j);
            SPF_elementsName.push_back(SPF_currentItem.getName());
            SPF_elementsDescription.push_back(j);
        }
        SPF_item.setAllElementNames(SPF_elementsName);
        SPF_item.setAllElementDescription(SPF_elementsDescription);
    } else {
        // If the SPF description does NOT exist, let's create a place-holder for the Item collection
        SPF_item = Item(kSPF, 1, ItemTypeEnum::ExplicitLength);
    }

    // Creating a place-holder for the Item witch name is "-", to maintain the right value of FRNs in the UAP
    Item dashItem("-", 0, ItemTypeEnum::FixedLength);

    /* Re-scanning the 'uapList' collection, if a Item name is:
     * 0) ""; Item already set
     * 1) "-": put the 'dashItem' in that position
     * 2) kREF: put  the 'REF_item' in that position
     * 3) kSPF: put the 'SPF_item' in that position
     * OTHERWISE -> error
     */
    for (unsigned int i = 0; i < uapList.size(); i++) {
        if (uapList.at(i) == "-") {
            itemCollection_ordered[i] = dashItem;
        } else if (uapList.at(i) == kREF) {
            itemCollection_ordered[i] = REF_item;
        } else if (uapList.at(i) == kSPF) {
            itemCollection_ordered[i] = SPF_item;
        } else if ((uapList.at(i) != "-") && (!uapList.at(i).empty())) {
            stringstream message;
            message << "Error while reading '" << uapDescription_fpath << "' file." << endl <<
                    "'" << uapList.at(i) << "' definition has NOT been found in the JSON description file." << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        }
    }

    // Storing the new ordered item collection..
    categoryDescription->setItemCollection(itemCollection_ordered);
}

// MARSHALLING
void JsonCategoriesDescriptionLoader::performMarshallingFromExternalToInternalDescription(
        UapAndMandatoryItems& uapAndMandatoryItems,
        ExternalCategoryDescription& itemsDescription,
        ExternalCategoryDescription& refDescription,
        ExternalCategoryDescription& spfDescription,
        CategoryDescription* categoryDescription,
        ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
    vector<Item>* itemCollectionTmp = nullptr;
    vector<Item>* compoundSubItemCollectionTmp = nullptr;
    vector<Item>* refItemCollectionTmp = nullptr;
    vector<Item>* refCompoundSubItemCollectionTmp = nullptr;
    vector<Item>* spfItemCollectionTmp = nullptr;
    vector<Item>* spfCompoundSubItemCollectionTmp = nullptr;

    vector<string> mandatoryItemsList = uapAndMandatoryItems.getMandatoryItems();

    // Loading the items description
    itemCollectionTmp = marshallingToInnerDatamodel(&itemsDescription,
                                                    LoadingTypeEnum::ItemEnum,
                                                    &mandatoryItemsList, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }

    categoryDescription->setItemCollection(*itemCollectionTmp);

    // Load COMPOUND SubITEMs description
    compoundSubItemCollectionTmp = marshallingToInnerDatamodel(&itemsDescription,
                                                               LoadingTypeEnum::CompoundSubitemEnum,
                                                               &mandatoryItemsList, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }

    //REF ITEMs - If REF file description exists, let's load the description of items and subItems
    if (!refDescription.getItemCollection().empty()) {
        // REF items description
        refItemCollectionTmp = marshallingToInnerDatamodel(&refDescription,
                                                           LoadingTypeEnum::ItemEnum,
                                                           &mandatoryItemsList, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        // Load COMPOUND SubITEMs description
        refCompoundSubItemCollectionTmp = marshallingToInnerDatamodel(&refDescription,
                                                                      LoadingTypeEnum::CompoundSubitemEnum,
                                                                      &mandatoryItemsList, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        //Put the REF's subItems in the 'compoundSubItemCollectionTmp' and update indexes in 'refItemCollectionTmp'
        //FIXME: rename 'addRE_or_SP_itemInItemCollection' function
        addRE_or_SP_itemInItemCollection(refItemCollectionTmp,
                                         refCompoundSubItemCollectionTmp,
                                         compoundSubItemCollectionTmp);

        categoryDescription->setRefItemDescription(*refItemCollectionTmp);
    }

    //SPF ITEMs - If SPF file description exists, let's load the description of items and subItems
    if (!spfDescription.getItemCollection().empty()) {
        // SPF items description
        spfItemCollectionTmp = marshallingToInnerDatamodel(&spfDescription,
                                                           LoadingTypeEnum::ItemEnum,
                                                           &mandatoryItemsList, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        // Load SPF SubITEMs description
        spfCompoundSubItemCollectionTmp = marshallingToInnerDatamodel(&spfDescription,
                                                                      LoadingTypeEnum::CompoundSubitemEnum,
                                                                      &mandatoryItemsList, status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }

        //Put the SPF's subItems in the 'compoundSubItemCollectionTmp' and update indexes in 'spfItemCollectionTmp'
        //FIXME: rename 'addRE_or_SP_itemInItemCollection' function
        addRE_or_SP_itemInItemCollection(spfItemCollectionTmp, spfCompoundSubItemCollectionTmp,
                                         compoundSubItemCollectionTmp);

        categoryDescription->setSpfItemDescription(*spfItemCollectionTmp);
    }

    categoryDescription->setCompoundSubItemsDescription(*compoundSubItemCollectionTmp);

    // CLEANING UP.. removing all temporary list created
    if (refItemCollectionTmp != nullptr) {
        refItemCollectionTmp->clear();
        delete refItemCollectionTmp;
    }

    if (refCompoundSubItemCollectionTmp != nullptr) {
        refCompoundSubItemCollectionTmp->clear();
        delete refCompoundSubItemCollectionTmp;
    }

    if (spfItemCollectionTmp != nullptr) {
        spfItemCollectionTmp->clear();
        delete spfItemCollectionTmp;
    }

    if (spfCompoundSubItemCollectionTmp != nullptr) {
        spfCompoundSubItemCollectionTmp->clear();
        delete spfCompoundSubItemCollectionTmp;
    }

    // These two are ALWAYS != nullptr
    compoundSubItemCollectionTmp->clear();
    delete compoundSubItemCollectionTmp;

    itemCollectionTmp->clear();
    delete itemCollectionTmp;
}

vector<Item>*
JsonCategoriesDescriptionLoader::marshallingToInnerDatamodel(ExternalCategoryDescription* extCategoryDescription,
                                                             LoadingTypeEnum type,
                                                             vector<string>* mandatoryItemsList,
                                                             ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
    vector<Item>* itemCollection = nullptr;
    vector<vector<string>> elementAllDescription;
    vector<ExternalItem> extItemCollection;
    // Attributes of "Item" class, to be set
    vector<string> elementNames;
    vector<int> elementDescriptions;
    vector<string> decodingFunction_name_vect;
    vector<string> decodedValue_variableType_vect;
    vector<vector<double>> validationRange_vect;
    vector<vector<double>> lsbValues_vect;
    vector<Range> rangeValue_vect;
    short int mandatoryItemNameIndex = -1;  // do NOT change '-1' value initialization: it is used below


    itemCollection = new vector<Item>();

    // Depending on the 'type' variable value, a collection type is selected and mapped to the internal data model
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
            stringstream message;
            message << "The LoadingTypeEnum value provided has not been recognized.";
            status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
            status->setMessage(message.str());
            return nullptr;
            break;
    }

    /* For each Item in the collection, its content is transferred to 'itemToFill' object,
    * an Item instance of the internal data model. */
    for (const ExternalItem& extItem : extItemCollection) {
        Item itemToFIll;
        itemToFIll.setName(extItem.getName());
        itemToFIll.setType(extItem.getType());
        itemToFIll.setLengthInByte(extItem.getLengthInByte());

        elementAllDescription = extItem.getElementDescriptions();
        // Iteration on ALL ELEMENTS of 'extItem'
        for (auto& elementDescription : elementAllDescription) {
            if (elementDescription.size() < kMinimumSizeOfExtElementDescription) {
                stringstream message;
                message << "An incomplete Element description has been found for Item " << extItem.getName() << " ("
                        << kMinimumSizeOfExtElementDescription << " attributes are required, at least)" << "." << endl;
                status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
                status->setMessage(message.str());
                return nullptr;
            }

            // Each string in the 'elementDescription' has role strictly related to its position in the array
            elementNames.push_back(elementDescription.at(0));                // ELEMENT NAME        @index 0
            elementDescriptions.push_back(stoi(elementDescription.at(1)));   // LENGTH IN BIT 	    @index 1
            decodingFunction_name_vect.push_back(elementDescription.at(2));  // DECODING FUNCTION 	@index 2

            // VALUE RANGES @index 4
            Range range_struct;
            if (elementDescription.at(4).empty()) {
                // No range has been provided for the current Element ==> All values are VALID
                range_struct.minValue = 0;  //FIXME redundant value setting (consider removing if block, keeping else one
                range_struct.maxValue = 0;  //FIXME redundant value setting
            } else {
                range_struct = marshallRangeValues(&elementDescription, status);
                if (status->getCode() != ReturnCodes::SUCCESS) {
                    return nullptr;
                }
            }
            rangeValue_vect.push_back(range_struct);

            // LSB VALUES -> from index 5 until the end of the description
            vector<double> lsbValuesOfCurrentElement;

            for (unsigned int i = 5; i < elementDescription.size(); i++) {
                if (!elementDescription.at(i).empty()) {
                    lsbValuesOfCurrentElement.push_back(stod(elementDescription.at(i)));
                } else {
                    lsbValuesOfCurrentElement.push_back(1);
                }
            }
            lsbValues_vect.push_back(lsbValuesOfCurrentElement);

            // REPRESENTATION MODE -> index 3
            if (elementDescription.at(3) == "IntegerValue") {
                // This parameter has to be updated with a correctly sized Integer (8, 16, 32 or 64 bits)
                double maxLsb = *(std::max_element(lsbValuesOfCurrentElement.begin(),
                                                   lsbValuesOfCurrentElement.end()));
                double maxElementValue = (std::pow(2, stoi(elementDescription[1]))) * maxLsb;
                double noBits = std::log2(maxElementValue);
                auto noBytes = (unsigned short int) std::ceil(noBits / 8);

                // SIGNED int values
                if (elementDescription.at(2) == "CA2toDecimal") {
                    switch (noBytes) {
                        case 1:
                            decodedValue_variableType_vect.emplace_back("SignedInteger8bitValue");
                            break;
                        case 2:
                            decodedValue_variableType_vect.emplace_back("SignedInteger16bitValue");
                            break;
                        case 3:
                        case 4:
                            decodedValue_variableType_vect.emplace_back("SignedInteger32bitValue");
                            break;
                        default:
                            decodedValue_variableType_vect.emplace_back("SignedInteger64bitValue");
                            break;
                    }
                } else {    // UNSIGNED int values
                    switch (noBytes) {
                        case 1:
                            decodedValue_variableType_vect.emplace_back("UnsignedInteger8bitValue");
                            break;
                        case 2:
                            decodedValue_variableType_vect.emplace_back("UnsignedInteger16bitValue");
                            break;
                        case 3:
                        case 4:
                            decodedValue_variableType_vect.emplace_back("UnsignedInteger32bitValue");
                            break;
                        default:
                            decodedValue_variableType_vect.emplace_back("UnsignedInteger64bitValue");
                            break;
                    }
                }
            } else {
                // ..all the other representation modes, except 'IntegerValue'
                decodedValue_variableType_vect.push_back(elementDescription.at(3));   // Representation Mode
            }
        }

        // Update element data in the current Item
        itemToFIll.setAllElementNames(elementNames);
        itemToFIll.setAllElementDescription(elementDescriptions);
        itemToFIll.setAllFunctionNamesForDecodingInString(decodingFunction_name_vect);
        itemToFIll.setAllRepresentationModesInStringForDecoding(decodedValue_variableType_vect);
        itemToFIll.setAllLsbValues(lsbValues_vect);
        itemToFIll.setAllValidRanges(rangeValue_vect);

        /* MANDATORY ITEM - Looking for the current Item in the the 'mandatoryItemsList':
        * if it exists -> set to mandatory and remove the Item name from 'mandatoryItemsList' list */
        vector<string>::iterator mandatoryItem_it;
        if ((mandatoryItem_it = find(mandatoryItemsList->begin(),
                                     mandatoryItemsList->end(),
                                     itemToFIll.getName())) != mandatoryItemsList->end()) {
            mandatoryItemNameIndex = (short int) (mandatoryItem_it - mandatoryItemsList->begin());

            if (mandatoryItemNameIndex != -1) {
                mandatoryItemsList->erase(mandatoryItemsList->begin() + mandatoryItemNameIndex);
            }
            itemToFIll.setItemMandatory(true);
        } else {
            // The current Item has not been found in the mandatory item list..
            itemToFIll.setItemMandatory(false);
        }

        // The Item is ready to be pushed back in the Category Item collection
        itemCollection->push_back(itemToFIll);

        // It's time to clear the local collection, where the Element information has been temporary stored
        elementNames.clear();
        elementDescriptions.clear();
        decodingFunction_name_vect.clear();
        decodedValue_variableType_vect.clear();
        lsbValues_vect.clear();
        rangeValue_vect.clear();
    }
    return itemCollection;
}

void
JsonCategoriesDescriptionLoader::addRE_or_SP_itemInItemCollection(vector<Item>* RE_or_SP_itemCollectionToBeUpdate,
                                                                  vector<Item>* RE_or_SP_compoundSubItemCollection,
                                                                  vector<Item>* compoundSubItemCollectionToBeUpdate) {
    /************************
     * Variable definitions *
     ************************/
    vector<int>* oldIndexesOfSubItem = nullptr;
    vector<int>* newIndexesOfSubItem = nullptr;
    unsigned short int newIndexInCompoundCollection = 0;

    unsigned short int itemTmp_index = 0;
    unsigned short int subitemTmp_index = 0;

    /* Scanning each Item..
     * If it is of 'CompoundLength' type, let's fill the 'elementDescription' field with the real index with respect to
     * 'compoundSubItemCollectionTmp' collection and let's add the Compound Item to 'compoundSubItemCollectionToBeUpdate' */
    for (auto& itemTmp : *RE_or_SP_itemCollectionToBeUpdate) {
        if (itemTmp.getItemType() == ItemTypeEnum::CompoundLength) {
            //Get all subItems of this current compound Item and create a new vector to store the new indexes
            oldIndexesOfSubItem = itemTmp.getAllElementDescription();
            newIndexesOfSubItem = new vector<int>(oldIndexesOfSubItem->size());

            /* Scanning each subitem..
             * Let's get the first free index in 'compoundSubItemCollectionTmp' and save at 'index' position the
             * current subitem and put the index in the 'newIndexesOfSubItem' */
            for (auto& subItemTmp : *RE_or_SP_compoundSubItemCollection) {
                newIndexInCompoundCollection = (unsigned short int) compoundSubItemCollectionToBeUpdate->size();
                compoundSubItemCollectionToBeUpdate->push_back(subItemTmp);
                newIndexesOfSubItem->at(subitemTmp_index) = newIndexInCompoundCollection;
                subitemTmp_index++;
            }

            itemTmp.setAllElementDescription(*newIndexesOfSubItem);
            RE_or_SP_itemCollectionToBeUpdate->at(
                    itemTmp_index) = itemTmp;    //Substitute the old Item with the one just updated
        }
        itemTmp_index++;
    }
}

Range JsonCategoriesDescriptionLoader::marshallRangeValues(vector<string>* elementInfo, ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
    Range range_struct;
    // REGEX to search for 'lt(n)' or 'gt(n)' in 'elementStringRange'
//    const char* lessThan_regexText = "(lt\\((-)?[0-9]+(\\.[0-9]+)?\\))";
    const char* lessThan_regexText = R"((lt\((-)?[0-9]+(\.[0-9]+)?\)))";        // lt(123.54)
//    const char* greaterThan_regexText = "(gt\\((-)?[0-9]+(\\.[0-9]+)?\\))";
    const char* greaterThan_regexText = R"((gt\((-)?[0-9]+(\.[0-9]+)?\)))";     // gt(123.54)
    const char* onlyDigits_regexText = "(-)?[0-9]+((\\.[0-9]+)?)";              // 123.54
    const char* textToSearch;
    char* singleRangeValue = nullptr;
    char* result = nullptr;
    regex_t lessThan_regex { }, greaterThan_regex { }, digits_regex { };
    vector<string> singleRange_vect;


    // The regular expressions need to be "compiled"
    regcomp(&lessThan_regex, lessThan_regexText, REG_EXTENDED | REG_NEWLINE);
    regcomp(&greaterThan_regex, greaterThan_regexText, REG_EXTENDED | REG_NEWLINE);
    regcomp(&digits_regex, onlyDigits_regexText, REG_EXTENDED | REG_NEWLINE);

    // Variables needed to get the result after a regular expression execution
    int maxMatches = 1, noMatch = 0;
    regmatch_t matches[maxMatches];        // Matches found

    string elementRange = elementInfo->at(4);

    // 'elementStringRange' string is duplicated because it is modified by 'strtok()' function, during the marshalling
    char* elementRangeCOPY = strdup(elementRange.c_str());

    // Parse input string based on '~' separator
    // E.g. [0]: "0" , [1]: "-180~lt(180)" , [2]: "-90~90" , [3]: "gt(0)~lt(100)"

    // 'strtok(input, separator)' | Tokenizes the 'input' string at each 'separator' occurrence
    singleRangeValue = strtok(elementRangeCOPY, &kTilde);

    while (singleRangeValue != nullptr) {
        singleRange_vect.emplace_back(singleRangeValue);
        singleRangeValue = strtok(nullptr, // with nullptr, 'strtok()' keeps tokenizing the previous string
                                  &kTilde);
    }
    delete[] singleRangeValue;
    singleRangeValue = nullptr;

    // Checking 'singleRange_vect' content with regular expression
    if (singleRange_vect.size() == 1) {    // Just a single value has been provided (e.g. "0" for Spare bit value)
        textToSearch = strdup(singleRange_vect.at(0).c_str());
        // Check that the range is really a number before converting to double
        noMatch = regexec(&digits_regex, textToSearch, maxMatches, matches, 0);    // Read only digits
        if (noMatch == 0) {
            //FIXME: consider using STRTOL instead of ATOI
            range_struct.minValue = atoi(singleRange_vect.at(0).c_str());
            range_struct.maxValue = atoi(singleRange_vect.at(0).c_str());
        } else {
            string elementName = elementInfo->at(0);
            stringstream message;
            message << "The range '" + singleRange_vect.at(0) + "' inserted for the Element '" + elementName +
                       "' is not a valid float value.";
            status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
            status->setMessage(message.str());
            return range_struct;
        }
    } else if (singleRange_vect.size() == 2) {    // LOWER and UPPER bounds have been provided
        // Parsing the LOWER Element BOUND
        textToSearch = strdup(singleRange_vect.at(0).c_str());

        // If regex matches, Element value shall be greater (>) than lower bound
        noMatch = regexec(&greaterThan_regex, textToSearch, maxMatches, matches, 0);
        if (noMatch == 0) {    // noMatch = 0 means regex matches
            range_struct.isMinIncluded = false;
        }
        // Check if the user are using lower than - lt(num) - applied to the lower bound of the range -> no make sense
        noMatch = regexec(&lessThan_regex, textToSearch, maxMatches, matches, 0);
        if (noMatch == 0) {    // noMatch = 0 means regex matches
            string elementName = elementInfo->at(0);
            stringstream message;
            message << "The lower bound range '" + string(textToSearch) + "' inserted for the Element '" + elementName
                       + "' does not make sense. For the lower bound, only 'gt(num)' or 'num' is supported.";
            status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
            status->setMessage(message.str());
            return range_struct;
        }
        // Extract digits from the string (if digits are valid)
        noMatch = regexec(&digits_regex, textToSearch, maxMatches, matches, 0);    // Read only digits
        if (noMatch == 0) {
            result = (char*) malloc((matches[0].rm_eo - matches[0].rm_so) + 1);    // +1 for the string terminator '\0'
            strncpy(result, &textToSearch[matches[0].rm_so], matches[0].rm_eo - matches[0].rm_so);
            result[(matches[0].rm_eo - matches[0].rm_so)] = '\0';    // adding the string terminator '\0'
            range_struct.isEmpty = false;
            //FIXME: Consider to use STRTOD instead of ATOF
            range_struct.minValue = (double) atof(result);
        } else {
            string elementName = elementInfo->at(0);
            stringstream message;
            message << "The range '" << string(textToSearch) << "' inserted for the Element '" << elementName <<
                    "' is not a valid float value." << endl;
            status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
            status->setMessage(message.str());
            return range_struct;
        }

        delete[] textToSearch;
        delete[] result;
        result = nullptr;

        // Parsing the UPPER Element BOUND
        textToSearch = strdup(singleRange_vect.at(1).c_str());

        // If regex matches, Element value shall be lower (<) than upper bound
        noMatch = regexec(&lessThan_regex, textToSearch, maxMatches, matches, 0);
        if (noMatch == 0) {    // noMatch = 0 means regex matches
            range_struct.isMaxIncluded = false;
        }
        // Check if the user are using greater than - gt(num) - applied to the upper bound of the range -> no make sense
        noMatch = regexec(&greaterThan_regex, textToSearch, maxMatches, matches, 0);
        if (noMatch == 0) {    // noMatch = 0 means regex matches
            string elementName = elementInfo->at(0);
            stringstream message;
            message << "The upper bound range '" << string(textToSearch) << "' inserted for the Element '"
                    << elementName
                    << "' does not make sense. For the upper bound, only 'lt(num)' or 'num' is supported." << endl;
            status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
            status->setMessage(message.str());
            return range_struct;
        }
        // Extract digits from the string (if digits are valid)
        noMatch = regexec(&digits_regex, textToSearch, maxMatches, matches, 0);    // Read only digits
        if (noMatch == 0) {
            result = (char*) malloc(
                    (matches[0].rm_eo - matches[0].rm_so) + 1);        // +1 for the string terminator '\0'
            strncpy(result, &textToSearch[matches[0].rm_so], matches[0].rm_eo - matches[0].rm_so);
            result[(matches[0].rm_eo - matches[0].rm_so)] = '\0';    // adding the string terminator '\0'
            //FIXME: Consider to use STRTOD instead of ATOF
            range_struct.maxValue = (double) atof(result);
            range_struct.isEmpty = false;
        } else {
            string elementName = elementInfo->at(0);
            stringstream message;
            message << "The range '" << string(textToSearch) << "' inserted for the Element '" << elementName <<
                    "' is not a valid float value.";
            status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
            status->setMessage(message.str());
            return range_struct;
        }

        delete[] textToSearch;
        delete[] result;
        result = nullptr;
    } else {
        string elementName = elementInfo->at(0);
        stringstream message;
        message << "Format of the range inserted for the Element '" << elementName
                << "' has not been recognized. Supported format are: \"0\", \"gt(0)~15\", \"-180~lt(180)\", ...";
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return range_struct;
    }

    delete[] elementRangeCOPY;
    elementRangeCOPY = nullptr;

    regfree(&lessThan_regex);
    regfree(&greaterThan_regex);

    return range_struct;
}

// ** SERIALIZATION **
void
JsonCategoriesDescriptionLoader::getCategoriesDescriptionMapFromSerialization(
        const string& serialized_asterixConfiguration_fpath,
        unordered_map<int, CategoryDescription*>* uapMap,
        ReturnStatus* status) {

    std::ifstream ifs(serialized_asterixConfiguration_fpath);
    if (!ifs.good()) {
        stringstream message;
        message << "Error opening '" << serialized_asterixConfiguration_fpath << "' file." << endl;
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return;
    }
    boost::archive::text_iarchive ia(ifs);
    ia >> *uapMap;

    ifs.close();
}

void JsonCategoriesDescriptionLoader::serializeCategoriesDescriptionMap(string& pathToArchive,
                                                                        unordered_map<int, CategoryDescription*>* uapMap,
                                                                        ReturnStatus* status) {

    std::ofstream ofs(pathToArchive);
    if (!ofs.good()) {
        stringstream message;
        message << "Error creating '" << pathToArchive << "' file." << endl;
        status->setCode(ReturnCodes::SERIALIZATION_FAILED);
        status->setMessage(message.str());
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    oa << *uapMap;

    ofs.close();
}