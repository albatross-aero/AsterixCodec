#include "data_model/decoder_objects/IntegerValue.h"

int IntegerValue::getDecodedValue() {
    return decodedValue;
}

void IntegerValue::setDecodedValue(int _decodedValue) {
    decodedValue = _decodedValue;
//    if (decodedValueInString != "")
//        decodedValueInString = "";
    decodedValueInString = to_string(decodedValue);
}

void IntegerValue::resetElementValue() {
    decodedValue = 0;
}

string IntegerValue::getDecodedDataInString() const {
//    if (decodedValueInString == "")
//        decodedValueInString = to_string(IntegerValue::decodedValue);
    return decodedValueInString;
}

string *IntegerValue::getPointerToDecodedDataInString() {
//    if (decodedValueInString == "")
//        decodedValueInString = to_string(decodedValue);
    return &(decodedValueInString);
}

void to_json(json &j, const IntegerValue &value) {
    j = {value.getDecodedDataInString()};
}

void from_json(const json &j, IntegerValue &value) {
    value.setDecodedValue(stoi(j.get<string>()));
}