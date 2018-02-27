#include "config/enum/RepresentationModeEnum.h"

void to_json(json &j, const RepresentationModeEnum &type) {
    switch (type) {
        case RepresentationModeEnum::InvalidValue:
            j = -1;
            break;
        case RepresentationModeEnum::IntegerValue:
            j = 0;
            break;
        case RepresentationModeEnum::FloatValue:
            j = 1;
            break;
        case RepresentationModeEnum::DoubleValue:
            j = 2;
            break;
        case RepresentationModeEnum::HexadecimalValue:
            j = 3;
            break;
        case RepresentationModeEnum::OctalValue:
            j = 4;
            break;
        case RepresentationModeEnum::StringValue:
            j = 5;
            break;
        case RepresentationModeEnum::Signed8bitIntegerValue:
            j = 6;
            break;
        case RepresentationModeEnum::Signed16bitIntegerValue:
            j = 7;
            break;
        case RepresentationModeEnum::Signed32bitIntegerValue:
            j = 8;
            break;
        case RepresentationModeEnum::Signed64bitIntegerValue:
            j = 9;
            break;
        case RepresentationModeEnum::Unsigned8bitIntegerValue:
            j = 10;
            break;
        case RepresentationModeEnum::Unsigned16bitIntegerValue:
            j = 11;
            break;
        case RepresentationModeEnum::Unsigned32bitIntegerValue:
            j = 12;
            break;
        case RepresentationModeEnum::Unsigned64bitIntegerValue:
            j = 13;
            break;
    }
}

void from_json(const json &j, RepresentationModeEnum &type) {
    if (j == "IntegerValue")
        type = RepresentationModeEnum::IntegerValue;
    else if (j == "FloatValue")
        type = RepresentationModeEnum::FloatValue;
    else if (j == "DoubleValue")
        type = RepresentationModeEnum::DoubleValue;
    else if (j == "HexadecimalValue")
        type = RepresentationModeEnum::HexadecimalValue;
    else if (j == "OctalValue")
        type = RepresentationModeEnum::OctalValue;
    else if (j == "StringValue")
        type = RepresentationModeEnum::StringValue;
    else if (j == "Signed8bitIntegerValue")
        type = RepresentationModeEnum::Signed8bitIntegerValue;
    else if (j == "Signed16bitIntegerValue")
        type = RepresentationModeEnum::Signed16bitIntegerValue;
    else if (j == "Signed32bitIntegerValue")
        type = RepresentationModeEnum::Signed32bitIntegerValue;
    else if (j == "Signed64bitIntegerValue")
        type = RepresentationModeEnum::Signed64bitIntegerValue;
    else if (j == "Unsigned8bitIntegerValue")
        type = RepresentationModeEnum::Unsigned8bitIntegerValue;
    else if (j == "Unsigned16bitIntegerValue")
        type = RepresentationModeEnum::Unsigned16bitIntegerValue;
    else if (j == "Unsigned32bitIntegerValue")
        type = RepresentationModeEnum::Unsigned32bitIntegerValue;
    else if (j == "Unsigned64bitIntegerValue")
        type = RepresentationModeEnum::Unsigned64bitIntegerValue;
    else
        type = RepresentationModeEnum::InvalidValue;
}