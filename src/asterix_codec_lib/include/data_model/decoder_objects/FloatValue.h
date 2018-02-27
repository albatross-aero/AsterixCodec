#ifndef DATA_MODEL_DECODER_OBJECTS_FLOATVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_FLOATVALUE_HPP_

#include <string>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a float - 4 Bytes.
 * This class is used when the decoded value is a number with 7 decimal digits of precision.
 */
class FloatValue : public ElementValue {
public:

    FloatValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0.0;
    }

    FloatValue(DecodedValueObjectPoolManager *_ptrToManager, float _decodedValue)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
    }

    virtual ~FloatValue() = default;

    float getDecodedValue();

    void setDecodedValue(float _decodedValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    float decodedValue;
};

void to_json(json &j, const FloatValue &value);

void from_json(const json &j, FloatValue &value);

#endif /* DATA_MODEL_DECODER_OBJECTS_FLOATVALUE_HPP_ */
