#ifndef UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONLOADER_H_
#define UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONLOADER_H_

#include <config/CommonVariables.h>
#include <config/Constants.h>
#include <config/enum/FunctionNameEnum.hpp>
#include <config/enum/RepresentationModeEnum.hpp>
#include <data_model/asterix_description/external/UapAndMandatoryItems.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <regex.h>
#include <cmath>

#include "data_model/AsterixConfiguration.hpp"
#include "data_model/itemsFilter.hpp"
#include "data_model/categoriesFilter.hpp"
#include "data_model/asterix_description/CategoryDescription.h"
#include "data_model/asterix_description/external/ExternalItem.h"
#include "data_model/asterix_description/external/ExternalCategoryDescription.h"
#include "data_model/decoder_objects/SignedInteger8bitValue.hpp"
#include "data_model/decoder_objects/SignedInteger16bitValue.hpp"
#include "data_model/decoder_objects/SignedInteger32bitValue.hpp"
#include "data_model/decoder_objects/SignedInteger64bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger8bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger16bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger32bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger64bitValue.hpp"
#include "data_model/decoder_objects/FloatValue.hpp"
#include "data_model/decoder_objects/DoubleValue.hpp"
#include "data_model/decoder_objects/StringValue.hpp"
#include "facade/reports/ReturnStatus.hpp"
#include "lib/boost/serialization/unordered_map.hpp"
#include "lib/boost/archive/text_oarchive.hpp"
#include "lib/boost/archive/text_iarchive.hpp"
#include "utils/asterix_json_description/JsonCategoriesDescriptionValidator.h"

enum class LoadingTypeEnum {
	ItemEnum = 0, CompoundSubitemEnum = 1, REF_ItemEnum = 2, SPF_ItemEnum = 3
};

class JsonCategoriesDescriptionLoader {

public:

	/**
	 * It reads the asterixConfiguration file and loads the Categories descriptions.
	 * @param astCatRepositoryPath Path where json configuration file is stored
	 * @param uapMap Pointer to a container for categories description to be filled
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void getCategoriesDescriptionMapFromJSONFile(string &astCatRepositoryPath, unordered_map<int, CategoryDescription*> *uapMap,
			ReturnStatus* status);

	/**
	 * It loads all JSON configuration files and fills the map of categories descriptions.	 *
	 * @param currentAsterixConfiguration Object contain paths for the Categories description files.
	 * @param astConfigurationFilePath Path where json configuration file is stored
	 * @param uapMap Pointer to a container for categories description to be filled by that function
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void composeCategoriesDescriptionMapFromJsonFiles(AsterixConfiguration currentAsterixConfiguration, string astConfigurationFilePath,	unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status);

	/**
	 * It loads all items, compound, REF and SPF descriptions of a category in a 'CategoryDescription' object.
	 * @param astConfigurationFilePath Path where json configuration file is stored
	 * @param categoriesConfiguration Object that contains paths of json description files for the current Category
	 * @param categoryNumber Number of category to search inside folderPath
	 * @param categoryDescription Pointer to an object to be filled with category description
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void composeCategoryDescriptionFromJsonFiles(string astConfigurationFilePath, CategoriesConfiguration categoriesConfiguration,
			int categoryNumber, CategoryDescription* categoryDescription, ReturnStatus* status);

	// ** Description files loading **
	/**
	 * Loads 'categoriesFilterPath' JSON file and update the Categories Description map with the information about Categories to be/not to be encoded/decoded - if the file exists.
	 * @param categoriesFilterPath Path where 'categoriesFilterPath' json file is stored
	 * @param uapMap Pointer to a container for Categories Description to be update with the encode/decode information.
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void loadCategoriesFilter(string categoriesFilterPath, unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status);

	/**
	 * It loads 'uapAndMandatoryItemsList' JSON file and store the values in a 'UapAndMandatoryItems' object.
	 * @param astConfigurationFilePath Path where json configuration file is stored
	 * @param uapDescriptionPath Path where 'uapAndMandatoryItemsList' json file is stored
	 * @param uapAndMandatoryItems Reference to object to be filled with the UAP and the list of mandatory Items
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void loadUapAndMandatoryItemsList(string astConfigurationFilePath, string uapDescriptionPath, UapAndMandatoryItems& uapAndMandatoryItems,
			ReturnStatus* status);

	/**
	 * It loads 'itemsDescriptionFile' JSON file and store the values in a 'ExternalCategoryDescription' object.
	 * @param astConfigurationFilePath Path where json configuration file is stored
	 * @param itemsDescriptionPath Path where 'itemsDescriptionFile' json file is stored
	 * @param extCategoryDescription Reference to object to be filled with the Items description.
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void loadItemsDescription(string astConfigurationFilePath, string itemsDescriptionPath, ExternalCategoryDescription& extCategoryDescription,
			ReturnStatus* status);

	/**
	 * It loads 'refDescriptionFile' or 'spfDescriptionFile' JSON file and store the values in a 'ExternalCategoryDescription' object - if the file exists.
	 * @param descriptionPath Path where 'refDescriptionFile' or 'spfDescriptionFile' json file is stored
	 * @param itemsDescription Reference to object to be filled with the Items description.
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void loadREForSPFDescription(string descriptionPath, ExternalCategoryDescription& itemsDescription, ReturnStatus* status);

	/**
	 * Loads 'filterFile' JSON file and update the 'CategoryDescription' object with the information about Items to be/not to be encoded/decoded - if the file exists.
	 * @param filterPath Path where 'filterFile' json file is stored
	 * @param categoryDescription Reference to object filled with the description of items and to be updated with the information about filters.
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void loadItemFilters(string filterPath, CategoryDescription* categoryDescription, ReturnStatus* status);

	/**
	 * Gets the Items description from the 'CategoryDescription' object and put the Items in order respect the 'uapList'.
	 * @param uapDescriptionPath Path where 'uapAndMandatoryItemsFile' json file is stored
	 * @param uapList Ordered list of Items name that represent the UAP
	 * @param categoryDescription Reference to object filled with the description of items
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void putInOrderItemsDescription(string uapDescriptionPath, std::vector<std::string> uapList, CategoryDescription* categoryDescription,
			ReturnStatus* status);

	// ** MARSHALLING **
	/**
	 * Maps all Items description contained in 'itemsDescription', 'refDescription' and 'spfDescription' in the 'categoryDescription'.
	 * Also sets the mandatory Items.
	 * @param itemsDescription Pointer to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
	 * @param refDescription is a pointer to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
	 * @param spfDescription is a pointer to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
	 * @param uapAndMandatoryItems Reference to object filled with the UAP and the list of mandatory Items
	 * @param categoryDescription Reference to object to be filled with the description of all Items
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void performMarshallingFromExternalToInternalDescription(ExternalCategoryDescription itemsDescription,
			ExternalCategoryDescription refDescription, ExternalCategoryDescription spfDescription, UapAndMandatoryItems uapAndMandatoryItems,
			CategoryDescription* categoryDescription, ReturnStatus* status);

	/**
	 * This function maps the lighter Category JSON description to the internal data model and set the mandatory Items.
	 * @param extCategoryDescription Pointer to an ExternalCategoryDescription instance, holding the ASTERIX Category JSON description loaded from a file
	 * @param type Lets the function to store the information in the right attribute of the ExternalCategoryDescription class
	 * @param mandatoryItemsList List of mandatory Items.
	 * @param status Pointer to a ReturnStatus insfilterPathtance, where a code status and a message are stored
	 * @return a pointer to a vector<Item>, holding a collection of Item (plain Item, compound subitems or REF/SPF items) of the internal data model
	 */
	static vector<Item>* marshallingToInnerDatamodel(ExternalCategoryDescription *extCategoryDescription, LoadingTypeEnum type,
			std::vector<std::string>* mandatoryItemsList, ReturnStatus* status);

	/**
	 * Adds to 'compoundSubItemCollectionToBeUpdate' the sub Items of the RE/SP and updates indexes in 'RE_or_SP_itemCollectionToBeUpdate' in order to access the right sub Items in 'compoundSubItemCollectionToBeUpdate'
	 * @param RE_or_SP_itemCollectionToBeUpdate List of all items that describe a RE/SP Item
	 * @param RE_or_SP_compoundSubItemCollection List of all sub Items that describe a RE/SP Item
	 * @param compoundSubItemCollectionToBeUpdate List of all sub Items that describe a Category
	 */
	static void addRE_or_SP_itemInItemCollection(vector<Item>* RE_or_SP_itemCollectionToBeUpdate, vector<Item>* RE_or_SP_compoundSubItemCollection,
			vector<Item>* compoundSubItemCollectionToBeUpdate);

	/**
	 * It converts the string version of the range into a Range structure with lower and upper bounds.
	 * @param elementInfo Vector of strings representing all information related to a single Element (including the string range)
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 * @return A Range structure filled with bounds.
	 */
	static Range marshallRangeValues(vector<string>* elementInfo, ReturnStatus* status);

	// ** FILTERING **
	/**
	 * Check the correctness of filters and update the Categories Description map with the information about Categories to be/not to be encoded/decoded.
	 * @param categoriesFilterPath Path where 'categoriesFilterPath' json file is stored
	 * @param categoriesFilters Object filled with the filters information for the decoder and encoder
	 * @param uapMap Pointer to a container for Categories Description to be update with the encode/decode information.
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkAndSetFiltersForAllCategoriesDescription(string categoriesFilterPath, CategoriesFilters& categoriesFilters,
			unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status);

	/**
	 * Updates the Categories Description with the information of filters.
	 * @param isLookingCatNumber If true, this method trying to set the filter only for the Category Number, if is false for the Category Number and Datasources
	 * @param decoderFilterType Type of filter for the decoder
	 * @param encoderFilterType Type of filter for the encoder
	 * @param decoderCategoryNumberToFilter List of Categories to be/not to be decode
	 * @param encoderCategoryNumberToFilter List of Categories to be/not to be encode
	 * @param uapMap Pointer to a container for Categories Description to be update with the encode/decode information.
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void setFiltersForAllCategoriesDescription(bool isLookingCatNumber, FilterType decoderFilterType, FilterType encoderFilterType,
			std::vector<int>& decoderCategoryNumberToFilter, std::vector<int>& encoderCategoryNumberToFilter, unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status);

	/**
	 * Check the correctness of filters and update 'CategoryDescription' object with the information about Items to be/not to be encoded/decoded.
	 * @param filterPath Path where 'uapAndMandatoryItemsFile' json file is stored
	 * @param categoryItemsFilters Object filled with the filters information for the decoder and encoder
	 * @param categoryDescription Reference to object filled with the description of items and to be updated with the information about filters.
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkAndSetFiltersForCategoryDescription(string filterPath, CategoryItemsFilters categoryItemsFilters,
			CategoryDescription* categoryDescription, ReturnStatus* status);

	/**
	 * Updates the Items and Compound description with the information of filters.
	 * @param decoderFilterType Type of filter for the decoder
	 * @param encoderFilterType Type of filter for the encoder
	 * @param decoderFilterList List of items to be/not to be decode
	 * @param encoderFilterList List of items to be/not to be encode
	 * @param itemsDescription List of Items description
	 * @param compoundSubItemsDescription List of all sub Items description
	 */
	static void setFiltersForVectorOfItems(FilterType decoderFilterType, FilterType encoderFilterType, std::vector<std::string>* decoderFilterList,
			std::vector<std::string>* encoderFilterList, std::vector<Item>* itemsDescription, std::vector<Item>* compoundSubItemsDescription);

	// ** SERIALIZATION **
	/**
	 * From 'astCatRepositoryPath' loads serialized unordered map of Category description.
	 * @param astCatRepositoryPath File path to the serialized file
	 * @param uapMap Pointer to a container for categories description to be filled with the de-serialized information
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void getCategoriesDescriptionMapFromSerialization(string &astCatRepositoryPath, unordered_map<int, CategoryDescription*> *uapMap,
			ReturnStatus* status);

	/**
	 * To 'pathToArchive' serialized unordered map of Category description to a file.
	 * @param pathToArchive File path to the serialized file
	 * @param uapMap Pointer to a container for categories description filled with the Categories Description
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void serializeCategoriesDescriptionMap(string &pathToArchive, unordered_map<int, CategoryDescription*> *uapMap, ReturnStatus* status);


private:
	/**
	 * Constructor.<br>
	 * This class has only static methods therefore this method is private.
	 */
	JsonCategoriesDescriptionLoader();

	/**
	 * Destructor.<br>
	 * This class has only static methods therefore this method is private.
	 */
	~JsonCategoriesDescriptionLoader();

	static CommonVariables* commonVariables;
};

#endif /* UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONLOADER_H_ */
