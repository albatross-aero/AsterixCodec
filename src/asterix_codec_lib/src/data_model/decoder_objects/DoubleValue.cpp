#include "data_model/decoder_objects/DoubleValue.h"

double DoubleValue::getDecodedValue() {
    return decodedValue;
}

void DoubleValue::setDecodedValue(double _decodedValue) {
    decodedValue = _decodedValue;
//    if (decodedValueInString != "")
//        decodedValueInString = "";
    decodedValueInString = to_string(decodedValue);
}

void DoubleValue::resetElementValue() {
    decodedValue = 0;
}

string DoubleValue::getDecodedDataInString() const {
//    if (decodedValueInString == "")
//        decodedValueInString = to_string(decodedValue);
    return decodedValueInString;
}

string *DoubleValue::getPointerToDecodedDataInString() {
//    if (decodedValueInString == "")
//        decodedValueInString = to_string(decodedValue);
    return &(decodedValueInString);
}

void to_json(json &j, const DoubleValue &value) {
    j = json{value.getDecodedDataInString()};
}

void from_json(const json &j, DoubleValue &value) {
    value.setDecodedValue(stod(j.get<string>()));
}