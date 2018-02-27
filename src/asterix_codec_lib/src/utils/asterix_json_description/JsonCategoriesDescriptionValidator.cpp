#include "utils/asterix_json_description/JsonCategoriesDescriptionValidator.h"

using namespace std;

// Declaration of static fields
CommonVariables* JsonCategoriesDescriptionValidator::commonVariables = CommonVariables::getInstance();

bool JsonCategoriesDescriptionValidator::checkIfKeyInCategoryDescriptionMapExists(int key,
                                                                                  unordered_map<int, CategoryDescription*>* uapMap) {
    return (uapMap->find(key) != uapMap->end());
}

int
JsonCategoriesDescriptionValidator::splitDatasourceString(string asterixConfiguration_fpath, int categoryNumber,
                                                          string originalDatasource, ReturnStatus* status) {
    //TODO: review the regex workflow - to match the previous one, in Category description loading
    // Variables needed to split SAC-SIC string
    string delimiter = "-";
    string SAC_decimal, SIC_decimal;
    size_t pos;
    unsigned short int zeroMissing = 0;
    vector<string> zeroToBeAdded;
    zeroToBeAdded.emplace_back("");
    zeroToBeAdded.emplace_back("0");
    zeroToBeAdded.emplace_back("00");

    stringstream ss;

    // Regular Expression in order to check the Syntax of input datasources
    const char* SAC_SIC_regex_text = "^(([0-9]{1,3})-([0-9]{1,3}))";    // e.g. "50-120"
    regex_t SAC_SIC_regex;
    int maxMatches = 1, noMatch = 0;
    regmatch_t matches[maxMatches];        // Matches found

    regcomp(&SAC_SIC_regex, SAC_SIC_regex_text, REG_EXTENDED | REG_NEWLINE);

    noMatch = regexec(&SAC_SIC_regex, originalDatasource.c_str(), maxMatches, matches, 0);
    if (noMatch != 0) {    // noMatch != 0 means regex doesn't match
        // Error, WRONG SYNTAX in the input datasource
        stringstream message;
        message << "Error while reading ASTERIX Configuration file: '" << asterixConfiguration_fpath << "'" << endl <<
                "Datasource '" << originalDatasource << "' has a wrong syntax." << endl <<
                "The supported syntax is 'SAC-SIC', with SAC and SIC in decimal representation (e.g. '50-80')."
                << endl;
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return -1;
    }
    regfree(&SAC_SIC_regex);

    // Split  SAC and SIC
    pos = originalDatasource.find(delimiter);
    if (pos != string::npos) {
        // SAC
        SAC_decimal = originalDatasource.substr(0, pos);
        originalDatasource.erase(0, pos + delimiter.length());
        // SIC
        SIC_decimal = originalDatasource;
    }

    // SAC and SIC are both represented in a single byte, so their values cannot exceed 255 (in decimal).
    if (stoi(SAC_decimal) > 255 || stoi(SIC_decimal) > 255) {
        stringstream message;
        message << "Error while reading ASTERIX Configuration file: '" << asterixConfiguration_fpath << "'" << endl <<
                "Datasource '" << SAC_decimal << "-" << SIC_decimal << "' is out of range. " <<
                "Maximum value for SAC or SIC is 255." << endl;
        status->setCode(ReturnCodes::AST_CONFIG_LOADER_ERROR);
        status->setMessage(message.str());
        return -1;
    }

    // First of all, flush CAT number in the stream
    ss << categoryNumber;

    // Checking the size of SAC and SIC: they SHALL have 3 digits
    zeroMissing = (unsigned short int) (3 - SAC_decimal.length());    // zeroMissing is 0, 1 or 2
    ss << zeroToBeAdded.at(zeroMissing) << SAC_decimal;

    zeroMissing = (unsigned short int) (3 - SIC_decimal.length());    // zeroMissing is 0, 1 or 2
    ss << zeroToBeAdded[zeroMissing] << SIC_decimal;

    return stoi(ss.str());
}

// ** Check VALIDATION of JSON files **
void
JsonCategoriesDescriptionValidator::performValidationOfJsonFiles(string itemDescription_fpath,
                                                                 string refDescription_fpath,
                                                                 string spfDescription_fpath,
                                                                 CategoryDescription* categoryDescription,
                                                                 ReturnStatus* status) {
    vector<string> allElementNames; // collect all elements name found during the validator of jsons

    // Check if 'coumpoundFile' contains valid objects
    compoundJsonValidator(itemDescription_fpath, categoryDescription, &allElementNames, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }

    // Check if 'jsonRefPath' contains valid objects
    REF_SPF_JsonValidator(refDescription_fpath, true, categoryDescription, &allElementNames, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }

    // Check if 'jsonSpfPath' contains valid objects
    REF_SPF_JsonValidator(spfDescription_fpath, false, categoryDescription, &allElementNames, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }

    // Check if 'categoryFile' contains valid objects
    jsonValidator(itemDescription_fpath, categoryDescription, &allElementNames, status);
    if (status->getCode() != ReturnCodes::SUCCESS) {
        return;
    }
}

void JsonCategoriesDescriptionValidator::jsonValidator(string fileName, CategoryDescription* categoryDescription,
                                                       vector<string>* allElementNames,
                                                       ReturnStatus* returnStatus) {
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
            stringstream message;
            message << "In " << fileName << " file - " << currentItem.getName()
                    << " - The size of 'elementNames' and the size of 'elementDescriptions' are different." << endl;
            returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
            returnStatus->setMessage("Error while reading " + fileName + ". " + message.str());
            return;
        }

        if ((currentItem.getItemType() == ItemTypeEnum::FixedLength) ||
            (currentItem.getItemType() == ItemTypeEnum::RepetitiveLength)
            || (currentItem.getItemType() == ItemTypeEnum::ExtendedLength)) {

            checkFixedRepetitiveExtendedItem(fileName, currentItem, returnStatus);

            // Checking that all Element names of Compound Items are UNIQUE
            for (unsigned int innerIndex = 0; innerIndex < elementNames->size(); innerIndex++) {
                elementName = elementNames->at(innerIndex);

                if ((elementName != kSpare) && (elementName != kFX)) {
                    if ((find(allElementNames->begin(), allElementNames->end(), elementName)) ==
                        allElementNames->end()) {
                        // A UNIQUE Element has been found.. so it is inserted in 'allElementNames' collection
                        allElementNames->push_back(elementName);
                    } else {
                        // A DUPLICATED Element has been found.. throw a error!
                        stringstream message;
                        message << "In " << fileName << " file - The Element '" << elementName << "' is duplicated!\n";
                        returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
                        returnStatus->setMessage(message.str());
                        return;
                    }
                }
            }

            // Vector sizes SHALL be EQUAL
            if (functionNameForDecoding->size() != elementDescriptions->size()) {
                stringstream message;
                message << "In " << fileName << " file - " << currentItem.getName()
                        << " - The size of 'functionNameForDecoding' and the size of 'elementDescriptions' are different."
                        << endl;
                returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
                returnStatus->setMessage("Error while reading " + fileName + ". " + message.str());
                return;

            } else if (lsbValues->size() != elementDescriptions->size()) {
                stringstream message;
                message << "In " << fileName << " file - " << currentItem.getName()
                        << " - The size of 'lsbValues' and the size of 'elementDescriptions' are different." << endl;
                returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
                returnStatus->setMessage("Error while reading " + fileName + ". " + message.str());
                return;

            } else if (representationModeForDecoding->size() != elementDescriptions->size()) {
                stringstream message;
                message << "In " << fileName << " file - " << currentItem.getName()
                        << " - The size of 'representationModeForDecoding' and the size of 'elementDescriptions' are different."
                        << endl;
                returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
                returnStatus->setMessage("Error while reading " + fileName + ". " + message.str());
                return;
            }

        } else if (currentItem.getItemType() == ItemTypeEnum::CompoundLength) {
            if (checkIndexesInSubItemsDescription(fileName, currentItem,
                                                  categoryDescription->getCompoundSubItemsDescription(),
                                                  returnStatus) ==
                false) {
                return;
            }

        } else if (currentItem.getItemType() == ItemTypeEnum::ExplicitLength) {
            if (currentItem.getName() == kREF) {
                if (checkIndexesInSubItemsDescription(fileName, currentItem,
                                                      categoryDescription->getRefItemDescription(), returnStatus) ==
                    false)
                    return;
            } else if (currentItem.getName() == kSPF) {
                if (checkIndexesInSubItemsDescription(fileName, currentItem,
                                                      categoryDescription->getSpfItemDescription(), returnStatus) ==
                    false) {
                    return;
                }
            }
        } else {    // This point is unreachable, the syntax check of autoJSON not accept type not register in 'STATICJSON_DECLARE_ENUM' macro in 'ExternalItem' class
            stringstream message;
            message << "In " << fileName + " file - " << currentItem.getName()
                    << " Unknown type of Item [could be:  extended, repetitive, compound, fixed, explicit - case matters!].\n";
            returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            returnStatus->setMessage(message.str());
            return;
        }
    }
}

void
JsonCategoriesDescriptionValidator::compoundJsonValidator(string fileName, CategoryDescription* categoryDescription,
                                                          vector<string>* allElementNames,
                                                          ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
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

    for (auto& currentSubItem : *subItemCollection) {
        elementNamesToValidate = currentSubItem.getAllElementNames();
        elementDescriptions = currentSubItem.getAllElementDescription();
        functionNameForDecoding = currentSubItem.getAllFunctionNamesForDecodingInString();
        lsbValues = currentSubItem.getLsbValues();
        representationModeForDecoding = currentSubItem.getAllRepresentationModesInStringForDecoding();

        // Checking the number of Element names and Element descriptions | They must be EQUAL
        if (elementNamesToValidate->size() != elementDescriptions->size()) {
            //FIXME: re-write the error message, using a stringstream 'message' variable
            cerr << "In " << fileName << " file - " << currentSubItem.getName()
                 << " - The size of 'elementNamesToValidate' and the size of 'elementDescriptions' are different."
                 << endl;
            status->setCode(ReturnCodes::AST_LOADER_ERROR);
            status->setMessage("Error while reading " + fileName + ".");
            return;
        }

        if ((currentSubItem.getItemType() == ItemTypeEnum::FixedLength) ||
            (currentSubItem.getItemType() == ItemTypeEnum::RepetitiveLength) ||
            (currentSubItem.getItemType() == ItemTypeEnum::ExtendedLength)) {

            checkFixedRepetitiveExtendedItem(fileName, currentSubItem, status);

            // Checking all Element names of Compound Items | They must be UNIQUE
            for (auto& elementName : *elementNamesToValidate) {
                if ((elementName != kSpare) && (elementName != kFX)) {
                    if (find(allElementNames->begin(), allElementNames->end(), elementName) == allElementNames->end()) {
                        // 'elementName' is UNIQUE -> it can be inserted in 'allElementNames' collection
                        allElementNames->push_back(elementName);
                    } else {
                        //FIXME: re-write the error message, using a stringstream 'message' variable
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
                //FIXME: re-write the error message, using a stringstream 'message' variable
                cerr << "In " << fileName << " - " << currentSubItem.getName()
                     << " - The size of 'functionNameForDecoding' and the size of 'elementDescriptions' are different."
                     << endl;
                status->setCode(ReturnCodes::AST_LOADER_ERROR);
                status->setMessage("Error while reading " + fileName + ".");
                return;

            } else if (lsbValues->size() != elementDescriptions->size()) {
                //FIXME: re-write the error message, using a stringstream 'message' variable
                cerr << "In " << fileName << " - " << currentSubItem.getName()
                     << " - The size of 'lsbValues' and the size of 'elementDescriptions' are different." << endl;
                status->setCode(ReturnCodes::AST_LOADER_ERROR);
                status->setMessage("Error while reading " + fileName + ".");
                return;

            } else if (representationModeForDecoding->size() != elementDescriptions->size()) {
                //FIXME: re-write the error message, using a stringstream 'message' variable
                cerr << "In " << fileName << " - " << currentSubItem.getName()
                     << " - The size of 'representationModeForDecoding' and the size of 'elementDescriptions' are different."
                     << endl;
                status->setCode(ReturnCodes::AST_LOADER_ERROR);
                status->setMessage("Error while reading " + fileName + ".");
                return;
            }

        } else if (currentSubItem.getItemType() == ItemTypeEnum::CompoundLength) {
            //FIXME: re-write the error message, using a stringstream 'message' variable
            cerr << "In " << fileName << " file - " << currentSubItem.getName()
                 << " - Wrong Item type: a Compound Item CANNOT contain another Compound."
                 << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage("Error while reading " + fileName + ".");
            return;

            //This point is unreachable, the syntax check of autoJSON not accept type not register in 'STATICJSON_DECLARE_ENUM' macro in 'ExternalItem' class
        } else if (currentSubItem.getItemType() != ItemTypeEnum::ExplicitLength) {
            cerr << "In " << fileName << " file - " << currentSubItem.getName()
                 << " - Unknown Item type [could be:  extended, repetitive, compound, fixed, explicit - case matters!].\n";
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage("Error while reading " + fileName + ".");
            return;
        }
    }
}

void JsonCategoriesDescriptionValidator::REF_SPF_JsonValidator(string fileName, bool isRefJson,
                                                               CategoryDescription* categoryDescription,
                                                               vector<string>* allElementNames,
                                                               ReturnStatus* returnStatus) {
    /*********************
     * Variable definitions *
     *********************/
    Item currentItem;
    string elementName;

    vector<Item>* itemCollection;
    vector<string>* elementNamesToValidate;
    vector<int>* elementDescriptions;
    vector<string>* functionNameForDecoding;
    vector<vector<double>>* lsbValues;
    vector<string>* representationModeForDecoding;

    /*************************
     * Start JSON Validation *
     *************************/
    if (isRefJson)
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
                 << " - The size of 'elementNamesToValidate' and the size of 'elementDescriptions' are different."
                 << endl;
            returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
            returnStatus->setMessage("Error while reading " + fileName + ".");
            return;
        }

        if ((currentItem.getItemType() == ItemTypeEnum::FixedLength) ||
            (currentItem.getItemType() == ItemTypeEnum::RepetitiveLength)
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
                        string message =
                                "In " + fileName + " file - The Element '" + elementName + "' is duplicated.\n";
                        returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
                        returnStatus->setMessage(message);
                        return;
                    }
                }
            }

            // Vector sizes shall be EQUAL
            if (functionNameForDecoding->size() != elementDescriptions->size()) {
                cerr << "In " << fileName << " file - " << currentItem.getName()
                     << " - The size of 'functionNameForDecoding' and the size of 'elementDescriptions' are different."
                     << endl;
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
                     << " - The size of 'representationModeForDecoding' and the size of 'elementDescriptions' are different."
                     << endl;
                returnStatus->setCode(ReturnCodes::AST_LOADER_ERROR);
                returnStatus->setMessage("Error while reading " + fileName + ".");
                return;
            }

        } else if (currentItem.getItemType() == ItemTypeEnum::CompoundLength) {
            if (checkIndexesInSubItemsDescription(fileName, currentItem,
                                                  categoryDescription->getCompoundSubItemsDescription(),
                                                  returnStatus) ==
                false)
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
            string message = fileName + " - " + currentItem.getName() +
                             " - Unknown Item type [could be:  extended, repetitive, compound, fixed, explicit - case matters!].\n";
            returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            returnStatus->setMessage("Error while reading " + fileName + ".");
            return;
        }
    }
}

void
JsonCategoriesDescriptionValidator::checkFixedRepetitiveExtendedItem(string fileName, Item item, ReturnStatus* status) {
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
                             +
                             " - The sum of its Element lengths (in bit) is not an exact multiple of 8, so it can't stay on an integer number of Bytes.\n";
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message);
            return;
        }
    } else {
        // Size in bytes shall be equal to the sum of size in bits
        if (((sumOfElementLengthsInBit / 8) != itemLengthInByte) || (sumOfElementLengthsInBit % 8 != 0)) {
            string message =
                    "In " + fileName + " file - " + item.getName()
                    +
                    " - The sum of its Element lengths (in bit) and its length (in Byte) don't match OR the sum of its Element lengths (in bit) is not an exact multiple of 8.\n";
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message);
            return;
        }
    }

    functionNameForDecoding = item.getAllFunctionNamesForDecodingInString();
    representationModeForDecoding = item.getAllRepresentationModesInStringForDecoding();

    // Check if 'functionNameForDecoding' of each Element of the current Item exist in the 'supportedFunctionNameForDecoding' list
    for (vector<string>::const_iterator it = functionNameForDecoding->begin();
         it != functionNameForDecoding->end(); it++) {
        if (find(supportedFunctionNameForDecoding->begin(), supportedFunctionNameForDecoding->end(), *it) ==
            supportedFunctionNameForDecoding->end()) {
            // The listed 'functionNameForDecoding' has NOT been found in the list of known decoding functions | Throw an error!
            string message = "In " + fileName + " file - " + item.getName() + " - function '" + *it +
                             "' is not a known Decoding Function name.\n";
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message);
            return;
        }
    }

    // Check if 'representationModeForDecoding' of each Element of the current Item exist in the 'supportedRepresentationModeForDecoding' list
    for (vector<string>::const_iterator it = representationModeForDecoding->begin();
         it != representationModeForDecoding->end(); it++) {
        if (find(supportedRepresentationModeForDecoding->begin(), supportedRepresentationModeForDecoding->end(), *it)
            == supportedRepresentationModeForDecoding->end()) {
            // The listed 'representationModeForDecoding' has NOT been found in the list of known representation modes | Throw an error!
            string message = "In " + fileName + " file - " + item.getName() + " - mode '" + *it +
                             "' is not a known Representation Mode.\n";
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message);
            return;
        }
    }
}

bool JsonCategoriesDescriptionValidator::checkIndexesInSubItemsDescription(string fileName, Item itemToCheck,
                                                                           vector<Item>* subitemsDescription,
                                                                           ReturnStatus* returnStatus) {
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
        indexInsideSubItem = (*allElementDescriptions).at(index);
        elementName_itemCollection = (*allElementNames).at(index);

        if (indexInsideSubItem >= subitemsDescription->size()) {
            stringstream message;
            message << "In " << fileName << " file - " << itemToCheck.getName() << " .::. Index of Element "
                    << (*allElementNames)[index]
                    << " does not match a subitem description - position " << indexInsideSubItem << " is out of range."
                    << endl;
            returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            returnStatus->setMessage(message.str());
            return false;
        }

        elementName_compoundItemCollection = subitemsDescription->at(indexInsideSubItem).getName();

        if (elementName_itemCollection != elementName_compoundItemCollection) {
            stringstream message;
            message << "In " << fileName << " file - " << itemToCheck.getName() << " .::. " << (*allElementNames)[index]
                    << " in CAT file does not match with compound list - position " << indexInsideSubItem
                    << ". Erroneous name "
                    << subitemsDescription->at(indexInsideSubItem).getName() << endl;
            returnStatus->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            returnStatus->setMessage(message.str());
            return false;
        }
    }
    return true;
}

// ** Check and prepare object from JSON **
void JsonCategoriesDescriptionValidator::checkAndPrepareCategoriesFilterList(string categoriesFilterPath,
                                                                             bool isDecoderFilter,
                                                                             CategoriesToFilter& categoriesToFilter,
                                                                             ReturnStatus* status) {

    std::vector<int> listOfCategoryFilterInDecimal;            // Store the final list of Category to filter
    std::vector<int> listOfCategoryAndDatasourcesToFilter;    // Store the final list of Category + datasource to filter


    string currentDatasourceToFilter;
    std::vector<string> currentListOfDatasourcesToFilter;

    FilterTypeEnum filterType = categoriesToFilter.getTypeOfFiltering();
    std::vector<CategoryFilter> listOfCategoryFilter = categoriesToFilter.getListOfCategoryFilter();

    // Check the correctness of 'filterType' (only inclusive or exclusive keywords are permitted)
    if (filterType != FilterTypeEnum::ExclusiveFilter && filterType != FilterTypeEnum::InclusiveFilter) {
        string message;

        if (isDecoderFilter) {
            message =
                    "Wrong filter type 'typeOfFiltering' for 'decoderFilter'. Valid values are 'inclusive' or 'exclusive'. \nError while loading "
                    + categoriesFilterPath + " file.";
        } else {
            message =
                    "Wrong filter type 'typeOfFiltering' for 'encoderFilter'. Valid values are 'inclusive' or 'exclusive'.\nError while loading " +
                    categoriesFilterPath
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

            if (isDecoderFilter)
                message = "Category number is " + to_string(categoryFilter.getCategoryNumber()) +
                          " for 'decoderFilter'. Valid values are grater than 0. \nError while loading "
                          + categoriesFilterPath + " file.";
            else
                message = "Category number is " + to_string(categoryFilter.getCategoryNumber()) +
                          " for 'encoderFilter'. Valid values are grater than 0. \nError while loading "
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
                    string message =
                            "Datasource in position " + to_string(i) +
                            " has an empty name in the 'dataSourcesToFilter' field.\nError while reading: "
                            + categoriesFilterPath + " file\n";
                    status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
                    status->setMessage(message);
                    return;
                }

                listOfCategoryAndDatasourcesToFilter.push_back(
                        splitDatasourceString(currentDatasourceToFilter, categoryFilter.getCategoryNumber(),
                                              categoriesFilterPath, status));
            }
        }
    }

    // If 'listOfCategoryFilterInDecimal' and 'listOfCategoryFilterInDecimal' sizes == 0, I want to encode/decode all Categories -> put the 'filterType' equal to ExclusiveFilter
    // -> ExclusiveFilter + empty list = decode/encode all Items except Items in the list (is empty) -> encode/decode all Items
    if (listOfCategoryFilterInDecimal.size() == 0 && listOfCategoryAndDatasourcesToFilter.size() == 0) {
        categoriesToFilter.setTypeOfFiltering(FilterTypeEnum::ExclusiveFilter);
    }

    categoriesToFilter.setListOfCategoryInDecimalToFilter(listOfCategoryFilterInDecimal);
    categoriesToFilter.setListOfCategoryAndDatasourcesInDecimalToFilter(listOfCategoryAndDatasourcesToFilter);
}

void JsonCategoriesDescriptionValidator::checkAndPrepareUapAndMandatoryItemsList(string uapAndMandatoryItems_fpath,
                                                                                 UapAndMandatoryItems& uapAndMandatoryItems,
                                                                                 ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
    vector<string>* uapList = nullptr;
    vector<string>* mandatoryItems = nullptr;
    vector<string>::iterator uapList_it_begin;
    string* itemName = nullptr;

    unsigned int uapListSize = 0;
    unsigned int mandatoryItemListSize = 0;
    long indexOfFXtoRemove = 0;  // 'long' as it comes from an iterator

    uapList = uapAndMandatoryItems.getPointerToUapList();
    mandatoryItems = uapAndMandatoryItems.getPointerToMandatoryItems();

    /******************
     * Check UAP list *
     ******************/
    uapListSize = (unsigned int) uapList->size();

    if (uapListSize == 0) {
        stringstream message;
        message << "Error while reading '" + uapAndMandatoryItems_fpath + "'" << endl <<
                "The UAP list is empty." << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    /*
     * RULES to enforce:
     * - UPPERCASE every Item name
     * - Convert 'REF' to 'RE', if needed
     * - Convert 'SPF' to 'SP', if needed
     * - ERROR if an empty string has been found
     * - ERROR if the FX is missing, after each set of 7 items
     */
    for (unsigned int i = 0; i < uapListSize; i++) {
        itemName = &(uapList->at(i));
        if ((*itemName).empty()) {
            stringstream message;
            message << "Error while reading '" << uapAndMandatoryItems_fpath << "'." << endl <<
                    "An Item with EMPTY name field has been found in the UAP." << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        } else {
            std::transform(itemName->begin(), itemName->end(), itemName->begin(), ::toupper);
        }

        if ((*itemName) == "SPF")
            *itemName = kSPF;
        if ((*itemName) == "REF")
            *itemName = kREF;

        if (((i + 1) % 8 == 0) && (*itemName) != kFX) {
            stringstream message;
            message << "Error while reading '" << uapAndMandatoryItems_fpath << "'." << endl <<
                    "A Field Extension (FX) definition is missing!" << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        }
    }

    // 'uapListSize' shall be multiple of 8
    if (uapListSize % 8 != 0) {
        stringstream message;
        message << "Error while reading '" << uapAndMandatoryItems_fpath << "'." << endl <<
                "The UAP list size is NOT an exact multiple of 8. Please check that the list is complete" << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    uapList_it_begin = uapList->begin();

    // Remove all FXs from the UAP - the Field Reference Numbers (FRN) do not include FXs
    for (auto it = uapList->end() - 1; it >= uapList_it_begin; it--) {
        if (*it == kFX) {
            indexOfFXtoRemove = it - uapList_it_begin;
            uapList->erase(uapList->begin() + indexOfFXtoRemove);
        }
    }

    /******************************
     * Check mandatory Items list *
     ******************************/
    mandatoryItemListSize = (unsigned int) mandatoryItems->size();

    if (mandatoryItemListSize == 0) {
        stringstream message;
        message << "Error while reading '" + uapAndMandatoryItems_fpath + "'" << endl <<
                "The mandatory items list is empty." << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    /*
     * RULES to enforce:
     * - UPPERCASE every Item name
     * - Convert 'REF' to 'RE', if needed
     * - Convert 'SPF' to 'SP', if needed
     * - ERROR if an empty string has been found
     */
    for (unsigned int i = 0; i < mandatoryItemListSize; i++) {
        itemName = &(mandatoryItems->at(i));

        if ((*itemName).empty()) {
            stringstream message;
            message << "Error while reading '" << uapAndMandatoryItems_fpath << "'." << endl <<
                    "An Item with EMPTY name field has been found in the mandatory items list." << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        } else {
            transform(itemName->begin(), itemName->end(), itemName->begin(), ::toupper);
        }

        if ((*itemName) == "SPF")
            *itemName = kSPF;
        if ((*itemName) == "REF")
            *itemName = kREF;
    }
}

void JsonCategoriesDescriptionValidator::checkAndPrepareExternalItemsDescription(string itemsDescriptionFilePath,
                                                                                 ExternalCategoryDescription& extItemsDescription,
                                                                                 ReturnStatus* status) {
    /************************
     * Variable definitions *
     ************************/
    vector<ExternalItem>* itemsDescription = nullptr;
    vector<ExternalItem>* compoundSubItemsDescription = nullptr;
    string* subitemName = nullptr;

    unsigned short int item_index = 0;

    itemsDescription = extItemsDescription.getPointerToItemCollection();
    compoundSubItemsDescription = extItemsDescription.getPointerToCompoundSubitemsCollection();

    /***************************
     * Check Items Description *
     ***************************/
    if (itemsDescription->empty()) {
        stringstream message;
        message << "Error while reading: '" << itemsDescriptionFilePath << "' file" << endl <<
                "'itemCollection' shall not be empty." << endl;
        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message.str());
        return;
    }

    // If a string is empty -> error
    // Every name in Uppercase
    // number of elements shall be > 0
    for (auto& item : *itemsDescription) {
        subitemName = item.getPointerToName();

        if (subitemName->empty()) {
            stringstream message;
            message << "Error while reading: " << itemsDescriptionFilePath + " file" << endl <<
                    "Item in position " << to_string(item_index) <<
                    " has an empty name in the 'itemCollection' field." << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        } else {
            transform(subitemName->begin(), subitemName->end(), subitemName->begin(), ::toupper);
        }

        if (item.getPointerToElementDescriptions()->empty()) {
            stringstream message;
            message << "Error while reading: " << itemsDescriptionFilePath << " file" << endl <<
                    "Error in the Item " << *subitemName << " - the vector of elements shall not be empty." << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        }

        checkAndPrepareItemElements(itemsDescriptionFilePath, *subitemName, item.getPointerToElementDescriptions(),
                                    status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }
        item_index++;
    }


    /**************************************
     * Check Compound SubItem Description *
     **************************************/
    // If a string is empty -> error
    // Every name in Uppercase
    // number of element > 0
    for (auto& item : *compoundSubItemsDescription) {
        subitemName = item.getPointerToName();

        if (subitemName->empty()) {
            stringstream message;
            message << "Error while reading: " << itemsDescriptionFilePath + " file" << endl <<
                    "Item in position " << to_string(item_index)
                    << " has an empty name in the 'compoundSubitemCollection' filed." << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        }

        if (item.getPointerToElementDescriptions()->empty()) {
            stringstream message;
            message << "Error while reading: " << itemsDescriptionFilePath + " file" << endl <<
                    "Error in the Item " << *subitemName
                    << " - the vector of elements shall not be empty." << endl;
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
            return;
        }

        checkAndPrepareItemElements(itemsDescriptionFilePath, *subitemName, item.getPointerToElementDescriptions(),
                                    status);
        if (status->getCode() != ReturnCodes::SUCCESS) {
            return;
        }
        item_index++;
    }
}

void JsonCategoriesDescriptionValidator::checkAndPrepareItemElements(string itemsDescriptionFilePath, string itemName,
                                                                     vector<vector<string>>* itemElements,
                                                                     ReturnStatus* status) {
    unsigned int elementStringSize = 0;
    unsigned int itemElementsSize = 0;
    vector<string>* elementString = nullptr;
    string* elementName = nullptr;

    itemElementsSize = itemElements->size();

    // If case insensitive is 'FX' -> element name became 'FX'
    // If case insensitive is 'Spare' -> element name became 'Spare'
    // Number of string per element must be at least 'kMinumumSizeOfExtElementDescription' - otherwise -> error
    for (unsigned int i = 0; i < itemElementsSize; i++) {
        // Get the description of one element
        elementString = &(itemElements->at(i));

        elementStringSize = elementString->size();


        if (elementStringSize < kMinimumSizeOfExtElementDescription) {
            stringstream message;
            message << "Error in the Item " << itemName << " - vector size of one of the elements has less than "
                    << to_string(kMinimumSizeOfExtElementDescription) + " values.\n" << "\nError while reading: "
                    << itemsDescriptionFilePath << " file\n";
            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message.str());
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

void JsonCategoriesDescriptionValidator::checkAndPrepareFilterList(string filterPath, bool isDecoderFilter,
                                                                   FilterTypeEnum& filterType,
                                                                   vector<string>* filterList, ReturnStatus* status) {
    unsigned int filterListSize = 0;
    string* itemToFilterName;

    // Check the correctness of 'filterType'
    if (filterType != FilterTypeEnum::ExclusiveFilter && filterType != FilterTypeEnum::InclusiveFilter) {
        string message;

        if (isDecoderFilter)
            message =
                    "Wrong filter type 'typeOfFiltering' for 'decoderFilter'. Valid values are 'inclusive' or 'exclusive'. \nError while loading "
                    + filterPath + " file.";
        else
            message =
                    "Wrong filter type 'typeOfFiltering' for 'encoderFilter'. Valid values are 'inclusive' or 'exclusive'.\nError while loading " +
                    filterPath
                    + " file.";

        status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
        status->setMessage(message);
        return;
    }

    // If 'filterList' size == 0, I want to encode/decode all Items -> put the 'filterType' equal to ExclusiveFilter
    // -> ExclusiveFilter + empty list = decode/encode all Items except Items in the list (is empty) -> encode/decode all Items
    if (filterList->size() == 0) {
        filterType = FilterTypeEnum::ExclusiveFilter;
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

            if (isDecoderFilter)
                message = "An Item name in the 'decoderFilter' is empty.\nError while loading " + filterPath + " file.";
            else
                message = "An Item name in the 'encoderFilter' is empty.\nError while loading " + filterPath + " file.";

            status->setCode(ReturnCodes::AST_MALFORMED_DESCRIPTION);
            status->setMessage(message);
            return;

        } else {
            transform(itemToFilterName->begin(), itemToFilterName->end(), itemToFilterName->begin(), ::toupper);
        }

        if ((*itemToFilterName) == "SPF")
            *itemToFilterName = kSPF;

        if ((*itemToFilterName) == "REF")
            *itemToFilterName = kREF;
    }

    // Remove 'REF' and 'SPF' from 'filterList'-> enable or disable of 'REF' and 'SPF' is performed by the variables in CodecInterface
    vector<string>::iterator itemToRemoveFromList;

    if ((itemToRemoveFromList = find(filterList->begin(), filterList->end(), kREF)) != filterList->end()) {
        int index = itemToRemoveFromList - filterList->begin();
        filterList->erase(filterList->begin() + index);
    }

    if ((itemToRemoveFromList = find(filterList->begin(), filterList->end(), kSPF)) != filterList->end()) {
        int index = itemToRemoveFromList - filterList->begin();
        filterList->erase(filterList->begin() + index);
    }
}
