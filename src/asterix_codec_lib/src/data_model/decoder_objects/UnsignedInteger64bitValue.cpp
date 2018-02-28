#include "data_model/decoder_objects/UnsignedInteger64bitValue.h"

uint64_t UnsignedInteger64bitValue::getDecodedValue() const {
    return decodedValue;
}

void UnsignedInteger64bitValue::setDecodedValue(uint64_t _decodedValue,
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

void UnsignedInteger64bitValue::resetElementValue() {
    decodedValue = 0;
}

string UnsignedInteger64bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *UnsignedInteger64bitValue::getPointerToDecodedDataInString() {
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

void to_json(json &j, const UnsignedInteger64bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, UnsignedInteger64bitValue &value) {
    value.setDecodedValue((uint64_t) stoi(j.get<string>()));
}