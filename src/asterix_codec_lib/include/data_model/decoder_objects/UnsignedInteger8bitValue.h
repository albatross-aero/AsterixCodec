#ifndef DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER8BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER8BITVALUE_HPP_

#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a uint8_t - 1 Byte.
 * This class is used when the decoded value is a integer.
 */

class UnsignedInteger8bitValue : public ElementValue {
public:
    UnsignedInteger8bitValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Unsigned8bitIntegerValue;
    }

    UnsignedInteger8bitValue(DecodedValueObjectPoolManager *_ptrToManager, uint8_t _decodedValue,
                             RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~UnsignedInteger8bitValue() = default;

    uint8_t getDecodedValue() const;

    void setDecodedValue(uint8_t _decodedValue,
                         RepresentationModeEnum _representationMode = RepresentationModeEnum::Unsigned8bitIntegerValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    uint8_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER8BITVALUE_HPP_ */
