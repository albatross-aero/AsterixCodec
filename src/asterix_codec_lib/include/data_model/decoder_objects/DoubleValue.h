#ifndef DATA_MODEL_DECODER_OBJECTS_DOUBLEVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_DOUBLEVALUE_HPP_

#include <string>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;


/**
 * Store decoded data of an element.
 * The type of value is a double - 8 Bytes.
 * This class is used when the decoded value is a number with 15 decimal digits of precision.
 */
class DoubleValue : public ElementValue {
public:

    DoubleValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
    }

    DoubleValue(DecodedValueObjectPoolManager *_ptrToManager, double _decodedValue)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
    }

    virtual ~DoubleValue() = default;

    double getDecodedValue();

    void setDecodedValue(double _decodedValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    double decodedValue;

};

#endif /* DATA_MODEL_DECODER_OBJECTS_DOUBLEVALUE_HPP_ */
