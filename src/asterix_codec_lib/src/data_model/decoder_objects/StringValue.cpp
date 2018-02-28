#include "data_model/decoder_objects/StringValue.h"

string StringValue::getDecodedValue() {
    return decodedValue;
}

void StringValue::setDecodedValue(string _decodedValue) {
    decodedValue = _decodedValue;
}

void StringValue::resetElementValue() {
    decodedValue = "";
}

string StringValue::getDecodedDataInString() const {
    return decodedValue;
}

string *StringValue::getPointerToDecodedDataInString() {
    return &(decodedValue);
}

void to_json(json &j, const StringValue &value) {
    j = {value.getDecodedDataInString()};
}

void from_json(const json &j, StringValue &value) {
    value.setDecodedValue(j.get<string>());
}