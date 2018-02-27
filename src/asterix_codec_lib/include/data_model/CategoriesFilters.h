#ifndef ASTERIXCODEC_CATEGORIESFILTERS_H
#define ASTERIXCODEC_CATEGORIESFILTERS_H

#include "CategoriesToFilter.h"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 *\brief This class manages two instances of CategoriesToFilter class: one for the encoder and one for the decoder. They are optional and they may be inclusive or exclusive
 *\brief type of filtering.
 */
class CategoriesFilters {

public:
    CategoriesFilters() = default;

    ~CategoriesFilters() = default;

    // GETTERS and SETTERS
    CategoriesToFilter getFilterForDecoder() const;

    void setFilterForDecoder(CategoriesToFilter _filterForDecoder);

    CategoriesToFilter getFilterForEncoder() const;

    void setFilterForEncoder(CategoriesToFilter _filterForEncoder);

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
    CategoriesToFilter decoderFilter;
    CategoriesToFilter encoderFilter;
};

// NLOHMANN-JSON
void to_json(json &j, const CategoriesFilters &categoriesFilters);

void from_json(const json &j, CategoriesFilters &categoriesFilters);

#endif //ASTERIXCODEC_CATEGORIESFILTERS_H
