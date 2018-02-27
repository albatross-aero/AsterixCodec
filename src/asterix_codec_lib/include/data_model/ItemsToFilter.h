#ifndef DATA_MODEL_ASTERIX_DESCRIPTION_ITEMSFILTER_HPP_
#define DATA_MODEL_ASTERIX_DESCRIPTION_ITEMSFILTER_HPP_

#include <string>
#include <vector>

#include "config/enum/FilterTypeEnum.h"

#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

using namespace std;

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
        typeOfFiltering = FilterTypeEnum::InclusiveFilter;
    }

    ~ItemsToFilter() = default;

    // GETTERS and SETTERS
    FilterTypeEnum getTypeOfFiltering() const;

    vector<string> getListOfItemsToFilter() const;

    void setListOfItemsToFilter(vector<string> _listOfItemsToFilter);

    void setTypeOfFiltering(FilterTypeEnum typeOfFiltering);

    //OTHERS
//    /**
//     * AutoJSON loading from JSON to Object.
//     */
//    void staticjson_init(staticjson::ObjectHandler *h) {
//        h->add_property("typeOfFiltering", &this->typeOfFiltering, staticjson::Flags::Optional);
//        h->add_property("itemsToFilter", &this->listOfItemsToFilter, staticjson::Flags::Optional);
//        h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
//    }

private:

    FilterTypeEnum typeOfFiltering;
    // Vector of Item name to filter
    vector<string> listOfItemsToFilter;
};

void to_json(json &j, const ItemsToFilter &items);

void from_json(const json &j, ItemsToFilter &items);

#endif /* DATA_MODEL_ASTERIX_DESCRIPTION_ITEMSFILTER_HPP_ */
