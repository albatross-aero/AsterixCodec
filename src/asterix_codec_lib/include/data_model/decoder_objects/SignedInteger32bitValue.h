#ifndef DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER32BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER32BITVALUE_HPP_

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a int32_t - 4 Bytes.
 * This class is used when the decoded value is a integer.
 */

class SignedInteger32bitValue : public ElementValue {
public:
    SignedInteger32bitValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Signed32bitIntegerValue;
    }

    SignedInteger32bitValue(DecodedValueObjectPoolManager *_ptrToManager, int32_t _decodedValue,
                            RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~SignedInteger32bitValue() = default;

    int32_t getDecodedValue() const;

    void setDecodedValue(int32_t _decodedValue,
                         RepresentationModeEnum _representationMode = RepresentationModeEnum::Signed32bitIntegerValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    int32_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER32BITVALUE_HPP_ */
