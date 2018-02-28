#ifndef DATA_MODEL_FUNCTIONNAMEENUM_H_
#define DATA_MODEL_FUNCTIONNAMEENUM_H_

#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

enum class FunctionNameEnum {
    InvalidValue = -1,
    toDecimal = 0,
    toHexadecimal = 1,
    CA2toDecimal = 2,
    CA2toHexadecimal = 3,
    toOctal = 4,
    toTargetId = 5,
    toASCII = 6,
    toMBData = 7
};

void to_json(json &j, const FunctionNameEnum &type);

void from_json(const json &j, FunctionNameEnum &type);

#endif /* DATA_MODEL_FUNCTIONNAMEENUM_H_ */
