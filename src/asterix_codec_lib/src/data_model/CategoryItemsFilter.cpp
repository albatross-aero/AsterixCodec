#include "data_model/CategoryItemsFilter.h"

ItemsToFilter CategoryItemsFilter::getFilterForDecoder() const {
    return decoderFilter;
}

void CategoryItemsFilter::setFilterForDecoder(ItemsToFilter _filterForDecoder) {
    decoderFilter = _filterForDecoder;
}

ItemsToFilter CategoryItemsFilter::getFilterForEncoder() const {
    return encoderFilter;
}

void CategoryItemsFilter::setFilterForEncoder(ItemsToFilter _filterForEncoder) {
    encoderFilter = _filterForEncoder;
}

void to_json(json &j, const CategoryItemsFilter &filter) {
    j = json{{"decoderFilter", filter.getFilterForDecoder()},
             {"encoderFilter", filter.getFilterForEncoder()}};
}

void from_json(const json &j, CategoryItemsFilter &filter) {
    filter.setFilterForDecoder(j.at("decoderFilter").get<ItemsToFilter>());
    filter.setFilterForEncoder(j.at("encoderFilter").get<ItemsToFilter>());
}