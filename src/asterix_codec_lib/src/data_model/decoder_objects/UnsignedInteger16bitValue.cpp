#include "data_model/decoder_objects/UnsignedInteger16bitValue.h"

uint16_t UnsignedInteger16bitValue::getDecodedValue() const {
    return decodedValue;
}

void UnsignedInteger16bitValue::setDecodedValue(uint16_t _decodedValue,
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

void UnsignedInteger16bitValue::resetElementValue() {
    decodedValue = 0;
}

string UnsignedInteger16bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *UnsignedInteger16bitValue::getPointerToDecodedDataInString() {
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

void to_json(json &j, const UnsignedInteger16bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, UnsignedInteger16bitValue &value) {
    value.setDecodedValue((uint16_t) stoi(j.get<string>()));
}