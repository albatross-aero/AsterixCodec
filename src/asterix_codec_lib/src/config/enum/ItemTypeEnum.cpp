#include "config/enum/ItemTypeEnum.h"

void to_json(json &j, const ItemTypeEnum &type) {
    switch (type) {
        case ItemTypeEnum::InvalidValue:
            j = -1;
            break;
        case ItemTypeEnum::FixedLength:
            j = 0;
            break;
        case ItemTypeEnum::ExtendedLength:
            j = 1;
            break;
        case ItemTypeEnum::RepetitiveLength:
            j = 2;
            break;
        case ItemTypeEnum::CompoundLength:
            j = 3;
            break;
        case ItemTypeEnum::ExplicitLength:
            j = 4;
            break;
        case ItemTypeEnum::EmptyValue:
            j = 5;
            break;
    }
}

void from_json(const json &j, ItemTypeEnum &type) {
    if (j == "FixedLength")
        type = ItemTypeEnum::FixedLength;
    else if (j == "ExtendedLength")
        type = ItemTypeEnum::ExtendedLength;
    else if (j == "RepetitiveLength")
        type = ItemTypeEnum::RepetitiveLength;
    else if (j == "CompoundLength")
        type = ItemTypeEnum::CompoundLength;
    else if (j == "ExplicitLength")
        type = ItemTypeEnum::ExplicitLength;
    else if (j == "")
        type = ItemTypeEnum::EmptyValue;
    else
        type = ItemTypeEnum::InvalidValue;
}