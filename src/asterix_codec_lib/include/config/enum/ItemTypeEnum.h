#ifndef DATA_MODEL_ITEMTYPEENUM_H_
#define DATA_MODEL_ITEMTYPEENUM_H_

#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

enum class ItemTypeEnum {
    InvalidValue = -1,
    FixedLength = 0,
    ExtendedLength = 1,
    RepetitiveLength = 2,
    CompoundLength = 3,
    ExplicitLength = 4,
    EmptyValue = 5
};

void to_json(json &j, const ItemTypeEnum &type);

void from_json(const json &j, ItemTypeEnum &type);

#endif /* DATA_MODEL_ITEMTYPEENUM_H_ */
