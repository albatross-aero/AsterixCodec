#ifndef DATA_MODEL_ASTERIX_DESCRIPTION_ITEMSFILTER_HPP_
#define DATA_MODEL_ASTERIX_DESCRIPTION_ITEMSFILTER_HPP_

#include <config/enum/FilterTypeEnum.hpp>
#include <string>
#include <vector>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"

/**
 *\brief This class matches a single filter (decoderFilter or encoderFilter) described in a JSON file needed to activate the filtering function (optional).
 *\brief The autoJSONcxx library loads the filter into an instance of that class.
 *\brief <br>
 *\brief This class represent a list of Data Item to be filtered out (exclusive type of filtering) or to be processed (inclusive type of filtering) during
 *\brief the decoding or the encoding phase.
 */
class ItemsToFilter {

	public:
		ItemsToFilter() {
			typeOfFiltering = FilterType::InclusiveFilter;
		}

		~ItemsToFilter() {

		}

		// GETTERS and SETTERS
		FilterType getTypeOfFiltering() {
			return typeOfFiltering;
		}

		std::vector<std::string> getListOfItemsToFilter() {
			return listOfItemsToFilter;
		}

		void setListOfItemsToFilter(std::vector<std::string> _listOfItemsToFilter) {
			listOfItemsToFilter = _listOfItemsToFilter;
		}

		//OTHERS
		/**
		 * AutoJSON loading from JSON to Object.
		 */
		void staticjson_init(staticjson::ObjectHandler *h) {
			h->add_property("typeOfFiltering", &this->typeOfFiltering, staticjson::Flags::Optional);
			h->add_property("itemsToFilter", &this->listOfItemsToFilter, staticjson::Flags::Optional);
			h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
		}

	private:

		FilterType typeOfFiltering;

		// Vector of Item name to filter
		std::vector<std::string> listOfItemsToFilter;
};


/**
 *\brief This class manages two instances of ItemsToFilter class: one for the encoder and one for the decoder. They are optional and they may be inclusive or exclusive
 *\brief type of filtering.
 */
class CategoryItemsFilters {

	public:
		CategoryItemsFilters() {
		}

		~CategoryItemsFilters() {
		}

		// GETTERS and SETTERS
		ItemsToFilter getFilterForDecoder() {
			return decoderFilter;
		}

		void setFilterForDecoder(ItemsToFilter _filterForDecoder) {
			decoderFilter = _filterForDecoder;
		}

		ItemsToFilter getFilterForEncoder() {
			return encoderFilter;
		}

		void setFilterForEncoder(ItemsToFilter _filterForEncoder) {
			encoderFilter = _filterForEncoder;
		}

		//OTHERS
		/**
		 * AutoJSON loading from JSON to Object
		 */
		void staticjson_init(staticjson::ObjectHandler *h) {
			h->add_property("decoderFilter", &this->decoderFilter, staticjson::Flags::Optional);
			h->add_property("encoderFilter", &this->encoderFilter, staticjson::Flags::Optional);
			h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
		}

	private:
		ItemsToFilter decoderFilter;
		ItemsToFilter encoderFilter;
};

#endif /* DATA_MODEL_ASTERIX_DESCRIPTION_ITEMSFILTER_HPP_ */
