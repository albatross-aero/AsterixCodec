#ifndef ASTERIXCODEC_CATEGORYFILTER_H
#define ASTERIXCODEC_CATEGORYFILTER_H

#include <string>
#include <vector>

#include "config/enum/FilterTypeEnum.h"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

using namespace std;

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

    ~CategoryFilter() = default;

    // GETTERS and SETTERS
    int getCategoryNumber() const;

    vector<string> getListOfDatasourcesToFilter() const;

    void setCategoryNumber(int categoryNumber);

    void setListOfDatasourcesToFilter(const vector<string> &listOfDatasourcesToFilter);

    //OTHERS
//    /**
//     * AutoJSON loading from JSON to Object.
//     */
//    void staticjson_init(staticjson::ObjectHandler *h) {
//        h->add_property("category", &this->categoryNumber);
//        h->add_property("dataSourcesToFilter", &this->listOfDatasourcesToFilter, staticjson::Flags::Optional);
//        h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
//    }

private:

    int categoryNumber;
    // Vector of Item name to filter
    vector<string> listOfDatasourcesToFilter;
};

// NLOHMANN-JSON
void to_json(json &j, const CategoryFilter &categoryFilter);

void from_json(const json &j, CategoryFilter &categoryFilter);

#endif /* ASTERIXCODEC_CATEGORYFILTER_H */
