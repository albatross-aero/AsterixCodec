#ifndef DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER16BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER16BITVALUE_HPP_

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a uint16_t - 2 Bytes.
 * This class is used when the decoded value is a integer.
 */

class UnsignedInteger16bitValue : public ElementValue {
public:
    UnsignedInteger16bitValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Unsigned16bitIntegerValue;
    }

    UnsignedInteger16bitValue(DecodedValueObjectPoolManager *_ptrToManager, uint16_t _decodedValue,
                              RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~UnsignedInteger16bitValue() = default;

    uint16_t getDecodedValue() const;

    void setDecodedValue(uint16_t _decodedValue,
                         RepresentationModeEnum _representationMode = RepresentationModeEnum::Unsigned16bitIntegerValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    uint16_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER16BITVALUE_HPP_ */
