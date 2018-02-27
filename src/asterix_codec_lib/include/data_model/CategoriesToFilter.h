#ifndef ASTERIXCODEC_CATEGORIESTOFILTER_H
#define ASTERIXCODEC_CATEGORIESTOFILTER_H

#include <vector>
#include <string>

#include "CategoryFilter.h"
#include "config/enum/FilterTypeEnum.h"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

using namespace std;

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
        typeOfFiltering = FilterTypeEnum::InclusiveFilter;
    }

    ~CategoriesToFilter() = default;

    // GETTERS and SETTERS
    FilterTypeEnum getTypeOfFiltering() const;

    void setTypeOfFiltering(FilterTypeEnum _typeOfFiltering);

    vector<CategoryFilter> getListOfCategoryFilter() const;

    vector<int> getListOfCategoryInDecimalToFilter() const;

    void setListOfCategoryInDecimalToFilter(vector<int> _listOfCategoryInDecimalToFilter);

    vector<int> getListOfCategoryAndDatasourcesInDecimalToFilter() const;

    void setListOfCategoryAndDatasourcesInDecimalToFilter(vector<int> _listOfCategoryAndDatasourcesInDecimalToFilter);

    void setListOfCategoryFilter(const vector<CategoryFilter> &listOfCategoryFilter);

    //OTHERS
//    /**
//     * AutoJSON loading from JSON to Object.
//     */
//    void staticjson_init(staticjson::ObjectHandler *h) {
//        h->add_property("typeOfFiltering", &this->typeOfFiltering);
//        h->add_property("categoriesToFilter", &this->listOfCategoryFilter);
//        h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
//    }

private:

    FilterTypeEnum typeOfFiltering;
    // Vector of Item name to filter
    vector<CategoryFilter> listOfCategoryFilter;
    vector<int> listOfCategoryInDecimalToFilter;
    vector<int> listOfCategoryAndDatasourcesInDecimalToFilter;
};

// NLOHMANN-JSON
void to_json(json &j, const CategoriesToFilter &categories);

void from_json(const json &j, CategoriesToFilter &categories);

#endif //ASTERIXCODEC_CATEGORIESTOFILTER_H
