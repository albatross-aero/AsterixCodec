#ifndef UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONLOADER_H_
#define UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONLOADER_H_

#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <regex.h>
#include <cmath>

#include "config/CommonVariables.h"
#include "config/Constants.h"
#include "config/enum/FunctionNameEnum.h"
#include "config/enum/RepresentationModeEnum.h"
#include "data_model/asterix_description/external/UapAndMandatoryItems.h"
#include "data_model/AsterixConfiguration.h"
#include "data_model/ItemsToFilter.h"
#include "data_model/CategoryFilter.h"
#include "data_model/CategoriesFilters.h"
#include "data_model/CategoriesToFilter.h"
#include "data_model/asterix_description/CategoryDescription.h"
#include "data_model/asterix_description/external/ExternalItem.h"
#include "data_model/asterix_description/external/ExternalCategoryDescription.h"
#include "data_model/decoder_objects/SignedInteger8bitValue.h"
#include "data_model/decoder_objects/SignedInteger16bitValue.h"
#include "data_model/decoder_objects/SignedInteger32bitValue.h"
#include "data_model/decoder_objects/SignedInteger64bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger8bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger16bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger32bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger64bitValue.h"
#include "data_model/decoder_objects/FloatValue.h"
#include "data_model/decoder_objects/DoubleValue.h"
#include "data_model/decoder_objects/StringValue.h"
#include "asterix_codec/facade/reports/ReturnStatus.hpp"
#include "data_model/CategoryItemsFilter.h"
#include "boost/serialization/unordered_map.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "utils/asterix_json_description/JsonCategoriesDescriptionValidator.h"

#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

enum class LoadingTypeEnum {
    ItemEnum = 0, CompoundSubitemEnum = 1, REF_ItemEnum = 2, SPF_ItemEnum = 3
};

class JsonCategoriesDescriptionLoader {

public:
    /*
     * 1) Load AsterixConfiguration.json
     * FOR EACH CATEGORY (catXXX) FOUND:
     *  2) Load 'catXXX.json'
     *  3) Load 'uapAndMandatoryItems.json'
     *  4) [OPT] Load 'catXXX_ref.json'
     *  5) [OPT] Load 'catXXX_spf.json'
     *  6) [OPT] Load filters...
     */

    /**
     * Reading the 'asterixConfiguration.json' file, it loads the description of each Category.
     * @param asterixConfiguration_fpath Full path to 'asterixConfiguration.json' file
     * @param uapMap Pointer to a container where the Category descriptions are stored
     * @param status Pointer to a ReturnStatus instance, to track the status of this action
     */
    static void loadAsterixDescription(string& asterixConfiguration_fpath,
                                       unordered_map<int, CategoryDescription*>* uapMap,
                                       ReturnStatus* status);

    /**
     * Reading the 'uapAndMandatoryItemsList.json' file, it stores the values in a 'UapAndMandatoryItems' object.
     * @param asterixConfiguration_fpath Path where json configuration file is stored
     * @param uapDescription_fpath Path where 'uapAndMandatoryItemsList' json file is stored
     * @param uapAndMandatoryItems Reference to the object to be filled with the UAP and the list of mandatory Items
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    static void
    loadUapAndMandatoryItemsList(const string& asterixConfiguration_fpath, const string& uapDescription_fpath,
                                 UapAndMandatoryItems& uapAndMandatoryItems,
                                 ReturnStatus* status);

    /**
     * Reading the 'itemsDescriptionFile.json' file, it stores the values in a 'ExternalCategoryDescription' object.
     * @param asterixConfiguration_fpath Path where json configuration file is stored
     * @param itemsDescription_fpath Path where the 'catXXX.json' file is stored
     * @param extCategoryDescription Reference to the object to be filled with the Items description.
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    //FIXME: review 'extCategoryDescription' DOC description
    static void
    loadItemsDescription(const string& asterixConfiguration_fpath, const string& itemsDescription_fpath,
                         ExternalCategoryDescription& extCategoryDescription,
                         ReturnStatus* status);

    /**
     * Reading the 'catXXX_ref.json' file, it stores the values in a 'ExternalCategoryDescription' object.
     * @param asterixConfiguration_fpath Path where json configuration file is stored
     * @param refDescription_fpath Path where json configuration file is stored
     * @param itemsDescription Reference to the object to be filled with the Items description.
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    static void loadReservedExpansionFieldDescription(const string& asterixConfiguration_fpath,
                                                      const string& refDescription_fpath,
                                                      ExternalCategoryDescription& refItemsDescription,
                                                      ReturnStatus* status);

    /**
     * Reading the 'catXXX_spf.json' file, it stores the values in a 'ExternalCategoryDescription' object.
     * @param asterixConfiguration_fpath Path where json configuration file is stored
     * @param spfDescription_fpath Path where json configuration file is stored
     * @param itemsDescription Reference to the object to be filled with the Items description.
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    static void loadSpecialPurposeFieldDescription(const string& asterixConfiguration_fpath,
                                                   const string& spfDescription_fpath,
                                                   ExternalCategoryDescription& spfItemsDescription,
                                                   ReturnStatus* status);

    /**
     * Gets the Items description from the 'CategoryDescription' object and put the Items in order respect the 'uapList'.
     * @param uapDescription_fpath Path where 'uapAndMandatoryItemsFile' json file is stored
     * @param uapList Ordered list of Item names, representing the UAP
     * @param categoryDescription Reference to object filled with the description of items
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    static void putInOrderItemsDescription(string uapDescription_fpath, vector<string> uapList,
                                           CategoryDescription* categoryDescription,
                                           ReturnStatus* status);

    // ** MARSHALLING **
    /**
     * Maps all Items description contained in 'itemsDescription', 'refDescription' and 'spfDescription' to the 'categoryDescription'.
     * It also sets the mandatory items.
     * @param uapAndMandatoryItems Reference to object filled with the UAP and the list of mandatory Items of a given Category
     * @param itemsDescription Reference to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
     * @param refDescription Reference to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
     * @param spfDescription Reference to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
     * @param categoryDescription Pointer to object to be filled with the description of all Items
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    static void performMarshallingFromExternalToInternalDescription(UapAndMandatoryItems& uapAndMandatoryItems,
                                                                    ExternalCategoryDescription& itemsDescription,
                                                                    ExternalCategoryDescription& refDescription,
                                                                    ExternalCategoryDescription& spfDescription,
                                                                    CategoryDescription* categoryDescription,
                                                                    ReturnStatus* status);

    /**
     * Maps the lighter Category JSON description to the internal data model and set the mandatory item list.
     * @param extCategoryDescription Pointer to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
     * @param type Lets the function to store the information in the right attribute of the ExternalCategoryDescription class
     * @param mandatoryItemsList List of mandatory Items.
     * @param status Pointer to a ReturnStatus insfilterPathtance, where a code status and a message are stored
     * @return a pointer to a vector<Item>, holding a collection of Item (plain Item, compound subitems or REF/SPF items) of the internal data model
     */
    static vector<Item>*
    marshallingToInnerDatamodel(ExternalCategoryDescription* extCategoryDescription, LoadingTypeEnum type,
                                vector<string>* mandatoryItemsList, ReturnStatus* status);

    /**
     * Adds to 'compoundSubItemCollectionToBeUpdate' the sub Items of the RE/SP and updates indexes in 'RE_or_SP_itemCollectionToBeUpdate' in order to access the right sub Items in 'compoundSubItemCollectionToBeUpdate'
     * @param RE_or_SP_itemCollectionToBeUpdate List of all items that describe a RE/SP Item
     * @param RE_or_SP_compoundSubItemCollection List of all sub Items that describe a RE/SP Item
     * @param compoundSubItemCollectionToBeUpdate List of all sub Items that describe a Category
     */
    static void addRE_or_SP_itemInItemCollection(vector<Item>* RE_or_SP_itemCollectionToBeUpdate,
                                                 vector<Item>* RE_or_SP_compoundSubItemCollection,
                                                 vector<Item>* compoundSubItemCollectionToBeUpdate);

    /**
     * Converts the string version of a range description into a Range structure, with lower and upper bounds.
     * @param elementInfo Vector of strings representing all information related to a single Element (including the string range)
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     * @return A Range structure filled with bounds.
     */
    static Range marshallRangeValues(vector<string>* elementInfo, ReturnStatus* status);

    // ** SERIALIZATION **
    /**
     * Loads a serialized unordered map of Category descriptions.
     * @param serialized_asterixConfiguration_fpath File path to the serialized file
     * @param uapMap Pointer to a container of categories description, to be filled with the de-serialized data
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    static void getCategoriesDescriptionMapFromSerialization(const string& serialized_asterixConfiguration_fpath,
                                                             unordered_map<int, CategoryDescription*>* uapMap,
                                                             ReturnStatus* status);

    /**
     * To 'pathToArchive' serialized unordered map of Category description to a file.
     * @param pathToArchive File path to the serialized file
     * @param uapMap Pointer to a container for categories description filled with the Categories Description
     * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
     */
    //FIXME: review the documentation - unreadable
    static void serializeCategoriesDescriptionMap(string& pathToArchive,
                                                  unordered_map<int, CategoryDescription*>* uapMap,
                                                  ReturnStatus* status);


private:
    /**
     * Constructor.<br>
     * This class has only static methods therefore this method is private.
     */
    JsonCategoriesDescriptionLoader() = delete;

    /**
     * Destructor.<br>
     * This class has only static methods therefore this method is private.
     */
    ~JsonCategoriesDescriptionLoader() = delete;

    static CommonVariables* commonVariables;
};

#endif /* UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONLOADER_H_ */
