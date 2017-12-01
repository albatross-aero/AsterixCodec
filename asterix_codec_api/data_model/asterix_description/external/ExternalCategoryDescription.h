#ifndef DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_
#define DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_

#include <vector>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"

#include "data_model/asterix_description/external/ExternalItem.h"

/**
 * The ExternalCategoryDescription class maps the ASTERIX Category JSON description to the internal Codec data model
 */
class ExternalCategoryDescription {
	public:
		ExternalCategoryDescription();

		//GETTERS AND SETTERS

		/**
		 * Returns a vector of ExternalItem instances, where the ASTERIX Category JSON description is mapped to.
		 * @return a vector<ExternalItem> holding the Item collection of an ASTERIX Category
		 */
		vector<ExternalItem> getItemCollection();

		vector<ExternalItem>* getPointerToItemCollection();

		/**
		 * Sets a vector of ExternalItem instances, representing the Item collection of an ASTERIX Category JSON description.
		 */
		void setItemCollection(vector<ExternalItem> itemCollection);

		/**
		 * Returns a vector of ExternalItem instances, where the ASTERIX Category JSON description is mapped to.
		 * @return a vector<ExternalItem> holding the collection of Compound Subitems of an ASTERIX Category
		 */
		vector<ExternalItem> getCompoundSubitemsCollection();

		vector<ExternalItem>* getPointerToCompoundSubitemsCollection();

		/**
		 * Sets a vector of ExternalItem instances, representing the Compound Subitem collection of an ASTERIX Category JSON description.
		 */
		void setCompoundSubitemsCollection(vector<ExternalItem> compoundSubitemsCollection);

		// AUTOJSONCXX
		void staticjson_init(staticjson::ObjectHandler *h) {
			h->add_property("itemCollection", &this->itemCollection, staticjson::Flags::Default);
			h->add_property("compoundSubitemCollection", &this->compoundSubitemCollection, staticjson::Flags::Optional);

			h->set_flags(staticjson::Flags::Default);
		}

	private:
		vector<ExternalItem> itemCollection;
		vector<ExternalItem> compoundSubitemCollection;
};

#endif /* DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_ */
