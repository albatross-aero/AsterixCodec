#ifndef DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER64BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER64BITVALUE_HPP_

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a int64_t - 8 Bytes.
 * This class is used when the decoded value is a integer.
 */

class SignedInteger64bitValue : public ElementValue {
public:
    SignedInteger64bitValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Signed64bitIntegerValue;
    }

    SignedInteger64bitValue(DecodedValueObjectPoolManager *_ptrToManager, int64_t _decodedValue, RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }


    virtual ~SignedInteger64bitValue() = default;

    int64_t getDecodedValue() const;

    void setDecodedValue(int64_t _decodedValue, RepresentationModeEnum _representationMode = RepresentationModeEnum::Signed64bitIntegerValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    int64_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER64BITVALUE_HPP_ */
