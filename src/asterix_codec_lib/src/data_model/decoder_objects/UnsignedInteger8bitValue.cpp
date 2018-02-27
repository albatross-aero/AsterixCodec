#include "data_model/decoder_objects/UnsignedInteger8bitValue.h"

uint8_t UnsignedInteger8bitValue::getDecodedValue() const {
    return decodedValue;
}

void UnsignedInteger8bitValue::setDecodedValue(uint8_t _decodedValue,
                                               RepresentationModeEnum _representationMode) {
    decodedValue = _decodedValue;
//    if (decodedValueInString != "") {
//        decodedValueInString = "";
//    }
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

void UnsignedInteger8bitValue::resetElementValue() {
    decodedValue = 0;
}

string UnsignedInteger8bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *UnsignedInteger8bitValue::getPointerToDecodedDataInString() {
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

void to_json(json &j, const UnsignedInteger8bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, UnsignedInteger8bitValue &value) {
    value.setDecodedValue((uint8_t) stoi(j.get<string>()));
}