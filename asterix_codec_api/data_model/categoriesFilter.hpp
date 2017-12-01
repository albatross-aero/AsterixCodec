#ifndef DATA_MODEL_ASTERIX_DESCRIPTION_CATEGORIESFILTER_HPP_
#define DATA_MODEL_ASTERIX_DESCRIPTION_CATEGORIESFILTER_HPP_

#include <config/enum/FilterTypeEnum.hpp>
#include <string>
#include <vector>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"

/**
 *\brief This class matches a filter on a Category described in a JSON file.
 *\brief The autoJSONcxx library loads the filter into an instance of that class.
 *\brief <br>
 *\brief This class contains a Category number or the Category number and its Datasources to be filtered out (exclusive type of filtering) or to be processed (inclusive type of filtering) during
 *\brief the decoding or the encoding phase.
 */
class CategoryFilter {

public:

	CategoryFilter() {
		categoryNumber = 0;
	}

	~CategoryFilter() {

	}

	// GETTERS and SETTERS
	int getCategoryNumber(){
		return categoryNumber;
	}

	std::vector<std::string>  getListOfDatasourcesToFilter(){
		return listOfDatasourcesToFilter;
	}

	//OTHERS
	/**
	 * AutoJSON loading from JSON to Object.
	 */
	void staticjson_init(staticjson::ObjectHandler *h) {
		h->add_property("category", &this->categoryNumber);
		h->add_property("dataSourcesToFilter", &this->listOfDatasourcesToFilter, staticjson::Flags::Optional);
		h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
	}

private:

	int categoryNumber;

	// Vector of Item name to filter
	std::vector<std::string> listOfDatasourcesToFilter;
};

/**
 *\brief This class matches a filter (decoderFilter or encoderFilter) described in a JSON file needed to activate the filtering function (optional).
 *\brief The autoJSONcxx library loads the filter into an instance of that class.
 *\brief <br>
 *\brief This class represent a list of Category / Category+Data sources to be filtered out (exclusive type of filtering) or to be processed (inclusive type of filtering) during
 *\brief the decoding or the encoding phase.
 */
class CategoriesToFilter {

public:
	CategoriesToFilter() {
		typeOfFiltering = FilterType::InclusiveFilter;
	}

	~CategoriesToFilter() {
	}

	// GETTERS and SETTERS
	FilterType getTypeOfFiltering() {
		return typeOfFiltering;
	}

	void setTypeOfFiltering(FilterType _typeOfFiltering) {
		typeOfFiltering = _typeOfFiltering;
	}

	std::vector<CategoryFilter> getListOfCategoryFilter() {
		return listOfCategoryFilter;
	}

	std::vector<int> getListOfCategoryInDecimalToFilter() {
		return listOfCategoryInDecimalToFilter;
	}

	void setListOfCategoryInDecimalToFilter(std::vector<int> _listOfCategoryInDecimalToFilter) {
		listOfCategoryInDecimalToFilter = _listOfCategoryInDecimalToFilter;
	}

	std::vector<int> getListOfCategoryAndDatasourcesInDecimalToFilter() {
		return listOfCategoryAndDatasourcesInDecimalToFilter;
	}

	void setListOfCategoryAndDatasourcesInDecimalToFilter(std::vector<int> _listOfCategoryAndDatasourcesInDecimalToFilter) {
		listOfCategoryAndDatasourcesInDecimalToFilter = _listOfCategoryAndDatasourcesInDecimalToFilter;
	}

	//OTHERS
	/**
	 * AutoJSON loading from JSON to Object.
	 */
	void staticjson_init(staticjson::ObjectHandler *h) {
		h->add_property("typeOfFiltering", &this->typeOfFiltering);
		h->add_property("categoriesToFilter", &this->listOfCategoryFilter);
		h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
	}

private:

	FilterType typeOfFiltering;

	// Vector of Item name to filter
	std::vector<CategoryFilter> listOfCategoryFilter;

	std::vector<int> listOfCategoryInDecimalToFilter;
	std::vector<int> listOfCategoryAndDatasourcesInDecimalToFilter;
};


/**
 *\brief This class manages two instances of CategoriesToFilter class: one for the encoder and one for the decoder. They are optional and they may be inclusive or exclusive
 *\brief type of filtering.
 */
class CategoriesFilters {

public:
	CategoriesFilters() {
	}

	~CategoriesFilters() {
	}

	// GETTERS and SETTERS
	CategoriesToFilter getFilterForDecoder() {
		return decoderFilter;
	}

	void setFilterForDecoder(CategoriesToFilter _filterForDecoder) {
		decoderFilter = _filterForDecoder;
	}

	CategoriesToFilter getFilterForEncoder() {
		return encoderFilter;
	}

	void setFilterForEncoder(CategoriesToFilter _filterForEncoder) {
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
	CategoriesToFilter decoderFilter;
	CategoriesToFilter encoderFilter;
};

#endif /* DATA_MODEL_ASTERIX_DESCRIPTION_CATEGORIESFILTER_HPP_ */
