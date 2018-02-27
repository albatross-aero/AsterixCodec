#include "data_model/decoder_objects/FloatValue.h"

float FloatValue::getDecodedValue() {
    return decodedValue;
}

void FloatValue::setDecodedValue(float _decodedValue) {
    decodedValue = _decodedValue;
//    if (decodedValueInString != "")
//        decodedValueInString = "";
    decodedValueInString = to_string(decodedValue);
}

void FloatValue::resetElementValue() {
    decodedValue = 0;
}

string FloatValue::getDecodedDataInString() const {
//    if (decodedValueInString == "")
//        decodedValueInString = to_string(decodedValue);
    return decodedValueInString;
}

string *FloatValue::getPointerToDecodedDataInString() {
//    if (decodedValueInString == "")
//        decodedValueInString = to_string(decodedValue);
    return &(decodedValueInString);
}

void to_json(json &j, const FloatValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, FloatValue &value) {
    value.setDecodedValue(stof(j.get<string>()));
}
