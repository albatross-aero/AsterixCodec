#include "data_model/decoder_objects/SignedInteger32bitValue.h"

int32_t SignedInteger32bitValue::getDecodedValue() const {
    return decodedValue;
}

void SignedInteger32bitValue::setDecodedValue(int32_t _decodedValue,
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

void SignedInteger32bitValue::resetElementValue() {
    decodedValue = 0;
}

string SignedInteger32bitValue::getDecodedDataInString() const {
//    if (decodedValueInString == "") {
//    }
    return decodedValueInString;
}

string *SignedInteger32bitValue::getPointerToDecodedDataInString() {
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

void to_json(json &j, const SignedInteger32bitValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, SignedInteger32bitValue &value) {
    value.setDecodedValue((int32_t) stoi(j.get<string>()));
}