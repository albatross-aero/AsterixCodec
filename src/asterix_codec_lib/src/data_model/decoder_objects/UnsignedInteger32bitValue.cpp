#include "data_model/decoder_objects/UnsignedInteger32bitValue.h"

uint32_t UnsignedInteger32bitValue::getDecodedValue() const {
    return decodedValue;
}

void UnsignedInteger32bitValue::setDecodedValue(uint32_t _decodedValue,
                                                RepresentationModeEnum _representationMode) {
    decodedValue = _decodedValue;
//    if (decodedValueInString != "")
//        decodedValueInString = "";
    representationMode = _representationMode;
    if (_representationMode == RepresentationModeEnum::OctalValue) {
        stringstream ss;
        ss << std::oct << (int) decodedValue;
        decodedValueInString = ss.str();
    } else if (_representationMode == RepresentationModeEnum::HexadecimalValue) {
        stringstream ss;
        ss << "0x" << std::hex << (int) decodedValue;
        decodedValueInString = ss.str();
    } else
        decodedValueInString = to_string(decodedValue);
}

void UnsignedInteger32bitValue::resetElementValue() {
    decodedValue = 0;
}

string UnsignedInteger32bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *UnsignedInteger32bitValue::getPointerToDecodedDataInString() {
    if (decodedValueInString == "") {
        if (representationMode == RepresentationModeEnum::OctalValue) {
            stringstream ss;
            ss << std::oct << (int) decodedValue;
            decodedValueInString = ss.str();
        } else if (representationMode == RepresentationModeEnum::HexadecimalValue) {
            stringstream ss;
            ss << "0x" << std::hex << (int) decodedValue;
            decodedValueInString = ss.str();
        } else
            decodedValueInString = to_string(decodedValue);
    }

    return &(decodedValueInString);
}

void to_json(json &j, const UnsignedInteger32bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, UnsignedInteger32bitValue &value) {
    value.setDecodedValue((uint32_t) stoi(j.get<string>()));
}