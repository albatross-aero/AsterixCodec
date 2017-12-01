#ifndef UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONVALIDATOR_H_
#define UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONVALIDATOR_H_

#include <config/CommonVariables.h>
#include <config/Constants.h>
#include <data_model/asterix_description/external/UapAndMandatoryItems.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <regex.h>

#include "data_model/asterix_description/CategoryDescription.h"
#include "data_model/asterix_description/external/ExternalCategoryDescription.h"
#include "facade/reports/ReturnStatus.hpp"
#include "data_model/itemsFilter.hpp"
#include "data_model/categoriesFilter.hpp"

class JsonCategoriesDescriptionValidator {
public:

	/**
	 * It checks if the key already exist in the map, to avoid declaring the category or category-data_source several times.
	 * @param key the key to be inserted in the map
	 * @param uapMap map containing already declared ASTERIX Description
	 * @return False if it does not exist, otherwise true
	 */
	static bool checkIfKeyInCategoryDescriptionMapExists(int key, unordered_map<int, CategoryDescription*> *uapMap);

	/**
	 * Split the 'originalDatasource' string in order to compose the interger in the following structure: <Category Number> + <SAC> + <SIC>
	 *@param originalDatasource String that contains SAC + SIC
	 *@param categoryNumber Number of Category
	 *@return Number composed by: <Category Number> + <SAC> + <SIC>
	 */
		static int splitDatasourceString(string originalDatasource, int categoryNumber, string astConfigurationFilePath, ReturnStatus *status);

	// ** Check VALIDATION of JSON files **
	/**
	 * It checks the validity of all Items description contained in 'categoryDescription' input parameter.
	 * @param itemDescriptionPath Path where 'itemsDescriptionFile' json file is stored
	 * @param refDescriptionPath Path where 'refDescriptionFile' json file is stored
	 * @param spfDescriptionPath Path where 'spfDescriptionFile' json file is stored
	 * @param categoryDescription Pointer to an object filled with all Items description
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void performValidationOfJsonFiles(string itemDescriptionPath, string refDescriptionPath, string spfDescriptionPath,
			CategoryDescription* categoryDescription, ReturnStatus* status);

	/**
	 * It performs the validation of the Items description - e.g. check the uniqueness of element names, check the indexes and name between compound items and subItems description.
	 * @param fileName Name of the file from where Items description is loaded
	 * @param categoryDescription Pointer to an object filled with all Items description
	 * @param allElementNames Vector that collects all Elements name of all Items - used to check the uniqueness of element names
	 * @param returnStatus Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void jsonValidator(std::string fileName, CategoryDescription *categoryDescription, vector<string>* allElementNames, ReturnStatus *returnStatus);

	/**
	 * It performs the validation of the compound subItems description - e.g. check the uniqueness of element names, check the type of the subItems.
	 * @param fileName Name of the file from where Items description is loaded
	 * @param categoryDescription Pointer to an object filled with all Items description
	 * @param allElementNames Vector that collect all Elements name of all Items - used to check the uniqueness of element names
	 * @param returnStatus Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void compoundJsonValidator(std::string fileName, CategoryDescription *categoryDescription, vector<string>* allElementNames,
			ReturnStatus *returnStatus);

	/**
	 * It performs the validation of a Reserved Expansion Field [REF] / Special Purpose Field [SPF] description for a specific category. - e.g. check the uniqueness of element names, check the type of the subItems.
	 * @param fileName Name of the file from where REF/SPF description is loaded
	 * @param isRefJson Is set to 'true' if the validation is performed on a REF description - 'false' if SPF description
	 * @param categoryDescription Pointer to an object filled with all Items description
	 * @param allElementNames Vector that collect all Elements name of all Items - used to check the uniqueness of element names
	 * @param returnStatus Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void REF_SPF_JsonValidator(std::string fileName, bool isRefJson, CategoryDescription *categoryDescription, vector<string>* allElementNames,
			ReturnStatus *returnStatus);

	/**
	 * It performs the validation of a fixed, repetitive or extended Item.
	 * @param fileName Name of the file from where the item was loaded
	 * @param item Item to be checked
	 * @param returnStatus Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkFixedRepetitiveExtendedItem(std::string fileName, Item item, ReturnStatus *returnStatus);

	/**
	 * It performs a cross-check between the indexes contained into a single item and the sub items pointed by the indexes.
	 * @param fileName Name of the file from where the sub items were loaded
	 * @param item Item object that contains a vector of indexes used to access to the subItemsDescription
	 * @param subItemsDescription ItemsDescription object with all sub items - can be: compound / re / spf
	 * @param returnStatus Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static bool checkIndexesInSubItemsDescription(std::string fileName, Item item, vector<Item>* subItemsDescription, ReturnStatus *returnStatus);

	// ** Check and prepare object from JSON **
	/**
	 * It checks and prepares 'filterList' - e.g. check the correctness of 'filterType', check that an Category number is grater than 0.
	 * @param categoriesFilterPath Path where 'categoriesFilterPath' json file is stored
	 * @param isDecoderFilter Is setted to 'true' if the validation is performed on a decoder filter - 'false' if encoder filter
	 * @param categoriesToFilter Contains type of filter - exclusive / inclusive - for decoder/encoder and the list of Categories to be/not to be decode/encode
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkAndPrepareCategoriesFilterList(string categoriesFilterPath, bool isDecoderFilter, CategoriesToFilter& categoriesToFilter, ReturnStatus* status);

	/**
	 * It checks and prepares 'uapAndMandatoryItems' - e.g. listSize > 0, convert Item name to uppercase, remove FXs, check that an Item name is different from "" (empty string).
	 * @param uapAndMandatoryItemsFile Path where 'uapAndMandatoryItemsFile' json file is stored
	 * @param uapAndMandatoryItems Reference to object filled with the UAP and the list of mandatory Items to be checked and prepared
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkAndPrepareUapAndMandatoryItemsList(string uapAndMandatoryItemsFile, UapAndMandatoryItems& uapAndMandatoryItems, ReturnStatus* status);

	/**
	 * It checks and prepares 'itemsCollection' and 'compoundSubitemCollection' of 'extItemsDescription' - e.g. convert Item name to uppercase, check that an Item name is different from "" (empty string), check vector of elements dimension.
	 * @param itemsDescriptionFilePath Path where 'itemsDescriptionFile' json file is stored
	 * @param extItemsDescription Reference to object filled with the Items description and to be checked and prepared
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkAndPrepareExternalItemsDescription(string itemsDescriptionFilePath, ExternalCategoryDescription& extItemsDescription,
			ReturnStatus* status);

	/**
	 * It checks and prepares 'itemElements' - e.g. check the number of information for each Element.
	 * @param itemsDescriptionFilePath Path where 'itemsDescriptionFile' json file is stored
	 * @param itemName Item name of the elements to be checked  and prepared
	 * @param itemElements Pointer to a vector, each vector contains the description of one element
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkAndPrepareItemElements(string itemsDescriptionFilePath, string itemName, vector<vector<string>>* itemElements, ReturnStatus* status);

	/**
	 * It checks and prepares 'filterList' - e.g. check the correctness of 'filterType', convert Item name to upper case, check that an Item name is different from "" (empty string).
	 * @param filterPath Path where 'filterFile' json file is stored
	 * @param isDecoderFilter Is setted to 'true' if the validation is performed on a decoder filter - 'false' if encoder filter
	 * @param filterType Type of filter - exclusive / inclusive - for decoder/encoder
	 * @param filterList List of items to be/not to be decode/encode
	 * @param status Pointer to a ReturnStatus instance, where a code status and a message are stored
	 */
	static void checkAndPrepareFilterList(string filterPath, bool isDecoderFilter, FilterType& filterType, std::vector<std::string>* filterList,
			ReturnStatus* status);

private:
	/**
	 * Constructor.<br>
	 * This class has only static methods therefore this method is private.
	 */
	JsonCategoriesDescriptionValidator();

	/**
	 * Destructor.<br>
	 * This class has only static methods therefore this method is private.
	 */
	virtual ~JsonCategoriesDescriptionValidator();

	static CommonVariables* commonVariables;
};

#endif /* UTILS_ASTERIX_JSON_DESCRIPTION_JSONCATEGORIESDESCRIPTIONVALIDATOR_H_ */
