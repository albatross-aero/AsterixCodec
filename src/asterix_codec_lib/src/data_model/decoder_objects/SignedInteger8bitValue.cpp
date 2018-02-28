#include "data_model/decoder_objects/SignedInteger8bitValue.h"

int8_t SignedInteger8bitValue::getDecodedValue() const {
    return decodedValue;
}

void SignedInteger8bitValue::setDecodedValue(int8_t _decodedValue,
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

void SignedInteger8bitValue::resetElementValue() {
    decodedValue = 0;
}

string SignedInteger8bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *SignedInteger8bitValue::getPointerToDecodedDataInString() {
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

void to_json(json &j, const SignedInteger8bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, SignedInteger8bitValue &value) {
    value.setDecodedValue((int8_t) stoi(j.get<string>()));
}