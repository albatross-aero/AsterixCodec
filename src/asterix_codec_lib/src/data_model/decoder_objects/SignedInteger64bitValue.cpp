#include "data_model/decoder_objects/SignedInteger64bitValue.h"

int64_t SignedInteger64bitValue::getDecodedValue() const {
    return decodedValue;
}

void SignedInteger64bitValue::setDecodedValue(int64_t _decodedValue,
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

void SignedInteger64bitValue::resetElementValue() {
    decodedValue = 0;
}

string SignedInteger64bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *SignedInteger64bitValue::getPointerToDecodedDataInString() {
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

void to_json(json &j, const SignedInteger64bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, SignedInteger64bitValue &value) {
    value.setDecodedValue((int64_t) stoi(j.get<string>()));
}