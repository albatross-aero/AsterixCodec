#ifndef DATA_MODEL_FILTERTYPEENUM_H_
#define DATA_MODEL_FILTERTYPEENUM_H_

#include <string>
#include <vector>

#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Enumeration type matching 'typeOfFiltering' field in filter JSON file.
 */
enum class FilterTypeEnum {
    InvalidFilter = -1, ExclusiveFilter = 0, InclusiveFilter = 1
};

void to_json(json &j, const FilterTypeEnum &type);

void from_json(const json &j, FilterTypeEnum &type);

// /**
// * FilterTypeEnum registration in autoJSONcxx to be automatically loaded from JSON file to object instance.
// */
//STATICJSON_DECLARE_ENUM(FilterTypeEnum, { "exclusive", FilterTypeEnum::ExclusiveFilter }, { "inclusive", FilterTypeEnum::InclusiveFilter })

#endif /* DATA_MODEL_FILTERTYPEENUM_H_ */
