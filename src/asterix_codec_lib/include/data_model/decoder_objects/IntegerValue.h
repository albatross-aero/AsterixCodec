#ifndef DATA_MODEL_DECODER_OBJECTS_INTEGERVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_INTEGERVALUE_HPP_

#include <string>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a int - 4 Bytes.
 * This class is used when the decoded value is a integer.
 */
class IntegerValue : public ElementValue {

public:

    IntegerValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
    }

    virtual ~IntegerValue() = default;

    int getDecodedValue();

    void setDecodedValue(int _decodedValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    int decodedValue;
};

void to_json(json &j, const IntegerValue &value);

void from_json(const json &j, IntegerValue &value);

#endif /* DATA_MODEL_DECODER_OBJECTS_INTEGERVALUE_HPP_ */
