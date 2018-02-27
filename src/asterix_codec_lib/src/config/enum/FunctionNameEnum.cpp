#include "config/enum/FunctionNameEnum.h"

void to_json(json &j, const FunctionNameEnum &type) {
    switch (type) {
        case FunctionNameEnum::InvalidValue:
            j = -1;
            break;
        case FunctionNameEnum::toDecimal:
            j = 0;
            break;
        case FunctionNameEnum::toHexadecimal:
            j = 1;
            break;
        case FunctionNameEnum::CA2toDecimal:
            j = 2;
            break;
        case FunctionNameEnum::CA2toHexadecimal:
            j = 3;
            break;
        case FunctionNameEnum::toOctal:
            j = 4;
            break;
        case FunctionNameEnum::toTargetId:
            j = 5;
            break;
        case FunctionNameEnum::toASCII:
            j = 6;
            break;
        case FunctionNameEnum::toMBData:
            j = 7;
            break;
    }
}

void from_json(const json &j, FunctionNameEnum &type) {
    if (j == "toDecimal")
        type = FunctionNameEnum::toDecimal;
    else if (j == "toHexadecimal")
        type = FunctionNameEnum::toHexadecimal;
    else if (j == "CA2toDecimal")
        type = FunctionNameEnum::CA2toDecimal;
    else if (j == "CA2toHexadecimal")
        type = FunctionNameEnum::CA2toHexadecimal;
    else if (j == "toOctal")
        type = FunctionNameEnum::toOctal;
    else if (j == "toTargetId")
        type = FunctionNameEnum::toTargetId;
    else if (j == "toASCII")
        type = FunctionNameEnum::toASCII;
    else if (j == "toMBData")
        type = FunctionNameEnum::toMBData;
    else
        type = FunctionNameEnum::InvalidValue;
}