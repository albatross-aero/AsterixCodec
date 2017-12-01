#ifndef DATA_MODEL_FILTERTYPEENUM_H_
#define DATA_MODEL_FILTERTYPEENUM_H_

#include <string>
#include <vector>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"

/**
 * Enumeration type matching 'typeOfFiltering' field in filter JSON file.
 */
enum class FilterType {
	ExclusiveFilter = 0, InclusiveFilter = 1
};

/**
 * FilterType registration in autoJSONcxx to be automatically loaded from JSON file to object instance.
 */
STATICJSON_DECLARE_ENUM(FilterType, {"exclusive", FilterType::ExclusiveFilter}, {"inclusive", FilterType::InclusiveFilter})

#endif /* DATA_MODEL_FILTERTYPEENUM_H_ */
