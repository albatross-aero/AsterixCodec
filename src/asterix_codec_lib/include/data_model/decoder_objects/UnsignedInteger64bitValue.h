#ifndef DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER64BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER64BITVALUE_HPP_

#include <cstdint>
#include <string>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a uint64_t - 8 Bytes.
 * This class is used when the decoded value is a integer.
 */

class UnsignedInteger64bitValue : public ElementValue {
public:
    UnsignedInteger64bitValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Unsigned64bitIntegerValue;
    }

    UnsignedInteger64bitValue(DecodedValueObjectPoolManager *_ptrToManager, uint64_t _decodedValue, RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~UnsignedInteger64bitValue() = default;

    uint64_t getDecodedValue() const;

    void setDecodedValue(uint64_t _decodedValue, RepresentationModeEnum _representationMode = RepresentationModeEnum::Unsigned64bitIntegerValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    uint64_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER64BITVALUE_HPP_ */
