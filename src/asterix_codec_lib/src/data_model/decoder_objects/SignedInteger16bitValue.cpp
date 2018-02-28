#include "data_model/decoder_objects/SignedInteger16bitValue.h"

int16_t SignedInteger16bitValue::getDecodedValue() const {
    return decodedValue;
}

void SignedInteger16bitValue::setDecodedValue(int16_t _decodedValue,
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

void SignedInteger16bitValue::resetElementValue() {
    decodedValue = 0;
}

string SignedInteger16bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *SignedInteger16bitValue::getPointerToDecodedDataInString() {
    if (decodedValueInString == "") {
        if (representationMode == RepresentationModeEnum::OctalValue) {
            stringstream ss;
            ss << "0x" << std::oct << (int) decodedValue;
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

void to_json(json &j, const SignedInteger16bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, SignedInteger16bitValue &value) {
    value.setDecodedValue((int16_t) stoi(j.get<string>()));
}