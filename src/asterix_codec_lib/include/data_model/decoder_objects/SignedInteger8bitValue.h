#ifndef DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER8BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER8BITVALUE_HPP_

#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;


/**
 * Store decoded data of an element.
 * The type of value is a int8_t - 1 Byte.
 * This class is used when the decoded value is a integer.
 */

class SignedInteger8bitValue : public ElementValue {
public:
    SignedInteger8bitValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Signed8bitIntegerValue;
    }

    SignedInteger8bitValue(DecodedValueObjectPoolManager *_ptrToManager, int8_t _decodedValue,
                           RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~SignedInteger8bitValue() = default;

    int8_t getDecodedValue() const;

    void setDecodedValue(int8_t _decodedValue,
                         RepresentationModeEnum _representationMode = RepresentationModeEnum::Signed8bitIntegerValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    int8_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER8BITVALUE_HPP_ */
