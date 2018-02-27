#ifndef DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER16BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER16BITVALUE_HPP_

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is an int16_t - 2 Bytes.
 * This class is used when the decoded value is a integer.
 */

class SignedInteger16bitValue : public ElementValue {
public:
    SignedInteger16bitValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Signed16bitIntegerValue;
    }

    SignedInteger16bitValue(DecodedValueObjectPoolManager *_ptrToManager, int16_t _decodedValue, RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~SignedInteger16bitValue() = default;

    int16_t getDecodedValue() const;

    void setDecodedValue(int16_t _decodedValue, RepresentationModeEnum _representationMode = RepresentationModeEnum::Signed16bitIntegerValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    int16_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER16BITVALUE_HPP_ */
