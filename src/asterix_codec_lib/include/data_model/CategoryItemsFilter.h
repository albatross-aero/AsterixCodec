#ifndef ASTERIXCODEC_CATEGORYITEMSFILTER_H
#define ASTERIXCODEC_CATEGORYITEMSFILTER_H

#include "ItemsToFilter.h"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 *\brief This class manages two instances of ItemsToFilter class: one for the encoder and one for the decoder. They are optional and they may be inclusive or exclusive
 *\brief type of filtering.
 */
class CategoryItemsFilter {

public:
    CategoryItemsFilter() {
    }

    ~CategoryItemsFilter() = default;

    // GETTERS and SETTERS
    ItemsToFilter getFilterForDecoder() const;

    void setFilterForDecoder(ItemsToFilter _filterForDecoder);

    ItemsToFilter getFilterForEncoder() const;

    void setFilterForEncoder(ItemsToFilter _filterForEncoder);

    //OTHERS
//    /**
//     * AutoJSON loading from JSON to Object
//     */
//    void staticjson_init(staticjson::ObjectHandler *h) {
//        h->add_property("decoderFilter", &this->decoderFilter, staticjson::Flags::Optional);
//        h->add_property("encoderFilter", &this->encoderFilter, staticjson::Flags::Optional);
//        h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
//    }

private:
    ItemsToFilter decoderFilter;
    ItemsToFilter encoderFilter;
};

void to_json(json &j, const CategoryItemsFilter &filter);

void from_json(const json &j, CategoryItemsFilter &filter);

#endif //ASTERIXCODEC_CATEGORYITEMSFILTER_H
