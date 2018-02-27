#include "data_model/CategoriesFilters.h"

CategoriesToFilter CategoriesFilters::getFilterForDecoder() const {
    return decoderFilter;
}

void CategoriesFilters::setFilterForDecoder(CategoriesToFilter _filterForDecoder) {
    decoderFilter = _filterForDecoder;
}

CategoriesToFilter CategoriesFilters::getFilterForEncoder() const {
    return encoderFilter;
}

void CategoriesFilters::setFilterForEncoder(CategoriesToFilter _filterForEncoder) {
    encoderFilter = _filterForEncoder;
}

// NLOHMANN-JSON
void to_json(json &j, const CategoriesFilters &categoriesFilters) {
    j = json{{"decoderFilter", categoriesFilters.getFilterForDecoder()},
             {"encoderFilter", categoriesFilters.getFilterForEncoder()}};
}

void from_json(const json &j, CategoriesFilters &categoriesFilters) {
    categoriesFilters.setFilterForDecoder(j.at("decoderFilter").get<CategoriesToFilter>());
    categoriesFilters.setFilterForEncoder(j.at("encoderFilter").get<CategoriesToFilter>());
}