#ifndef DATA_MODEL_ASTERIXCONFIGURATION_HPP_
#define DATA_MODEL_ASTERIXCONFIGURATION_HPP_

#include <string>
#include <vector>

#include "data_model/CategoriesConfiguration.h"
#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"

/**
 * \brief This class contains Category configurations with all file paths needed for each available Category. It is directly loaded from the JSON configuration file and
 * all information loaded are used later to create an ASTERIX categories description map.
 */
class AsterixConfiguration {

	public:
		AsterixConfiguration() {

		}

		~AsterixConfiguration() {

		}

		/**
		 * Getter for the categoriesConfiguration object (it makes a copy of the private variable).
		 */
		const std::vector<CategoriesConfiguration>& getCategoriesConfiguration() const {
			return categoriesConfiguration;
		}

		/**
		 * Getter for the categoriesFilterFile path.
		 */
		std::string& getCategoriesFilterFile() {
			return categoriesFilterFile;
		}

		/**
		 * AutoJSON loading from JSON to Object
		 */
		void staticjson_init(staticjson::ObjectHandler *h) {
			h->add_property("categories", &this->categoriesConfiguration);
			h->add_property("categoriesFilterFile", &this->categoriesFilterFile, staticjson::Flags::Optional);
			h->set_flags(staticjson::Flags::Default | staticjson::Flags::AllowDuplicateKey);
		}

	private:
		/**
		 * Vector of Category Description coming from ASTERIX Configuration File
		 */
		std::vector<CategoriesConfiguration> categoriesConfiguration;

		std::string categoriesFilterFile;
};

#endif /* DATA_MODEL_ASTERIXCONFIGURATION_HPP_ */
