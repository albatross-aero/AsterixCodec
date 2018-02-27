#include "config/enum/FilterTypeEnum.h"

void to_json(json &j, const FilterTypeEnum &type) {
    switch (type) {
        case FilterTypeEnum::InvalidFilter:
            j = -1;
            break;
        case FilterTypeEnum::ExclusiveFilter:
            j = 0;
            break;
        case FilterTypeEnum::InclusiveFilter:
            j = 1;
            break;
    }
}

void from_json(const json &j, FilterTypeEnum &type) {
    if (j == "FixedLength")
        type = FilterTypeEnum::ExclusiveFilter;
    else if (j == "ExtendedLength")
        type = FilterTypeEnum::InclusiveFilter;
    else
        type = FilterTypeEnum::InvalidFilter;
}