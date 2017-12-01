#ifndef DATA_MODEL_CATEGORIESCONFIGURATION_H_
#define DATA_MODEL_CATEGORIESCONFIGURATION_H_

#include <string>
#include <vector>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"

/**
 * \brief This class contains all parameters to describe a single ASTERIX Category-Edition description, such as all paths to the JSON description files.
 *
 */
class CategoriesConfiguration {

	public:
		CategoriesConfiguration();

		~CategoriesConfiguration();

		// Getter and Setters
		bool isCategoryByDefault() const;

		int getCategoryNumber() const;

		const std::vector<string>& getDataSourcesInDec() const;

		const std::string& getFilterFile() const;

		const std::string& getItemsDescriptionFile() const;

		const std::string& getRefDescriptionFile() const;

		const std::string& getSpfDescriptionFile() const;

		const std::string& getUapAndMandatoryItemsFile() const;

		//OTHER FUNCTIONS
		/*
		 * AutoJSON loading from JSON to Object
		 */
		void staticjson_init(staticjson::ObjectHandler *h);

	private:
		/*
		 * Value coming from the ASTERIX configuration file to generate the map ASTERIX Description.
		 * Asterix Edition number a.k.a edition name
		 */
		int categoryNumber;

		/*
		 * Path to JSON file with UAP and mandatory items
		 */
		std::string uapAndMandatoryItemsFile;

		/*
		 * Path to items description JSON file
		 */
		std::string itemsDescriptionFile;

		/*
		 * Path to Reserved Expansion Field JSON file
		 */
		std::string refDescriptionFile;

		/*
		 * Path to Special Purpose Field JSON file
		 */
		std::string spfDescriptionFile;

		/*
		 * Path to filter JSON file
		 */
		std::string filterFile;

		/*
		 * Array of Data Sources using this description
		 */
		std::vector<string> dataSourcesInDec;

		/*
		 * Boolean to identify if this description is to be used by default to support the category, or not
		 */
		bool categoryByDefault = false;
};

#endif /* DATA_MODEL_CATEGORIESCONFIGURATION_H_ */
