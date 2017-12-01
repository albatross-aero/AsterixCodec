#ifndef DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER32BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER32BITVALUE_HPP_

#include <data_model/decoder_objects/ElementValue.hpp>
#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>


/**
 * Store decoded data of an element.
 * The type of value is a uint32_t - 4 Bytes.
 * This class is used when the decoded value is a integer.
 */

class UnsignedInteger32bitValue: public ElementValue {
	public:
		UnsignedInteger32bitValue(DecodedValueObjectPoolManager* _ptrToManager)
				: ElementValue(_ptrToManager) {
			decodedValue = 0;
			representationMode = RepresentationModeEnum::Unsigned32bitIntegerValue;
		}

		UnsignedInteger32bitValue(DecodedValueObjectPoolManager* _ptrToManager, uint32_t _decodedValue, RepresentationModeEnum _representationMode)
				: ElementValue(_ptrToManager) {
			decodedValue = _decodedValue;
			representationMode = _representationMode;
		}

		virtual ~UnsignedInteger32bitValue() {

		}

		uint32_t getDecodedValue() const {
			return decodedValue;
		}

		void setDecodedValue(uint32_t _decodedValue, RepresentationModeEnum _representationMode = RepresentationModeEnum::Unsigned32bitIntegerValue) {
			decodedValue = _decodedValue;
			if (decodedValueInString != "")
				decodedValueInString = "";
			representationMode = _representationMode;
		}

		void resetElementValue() {
			decodedValue = 0;
		}

		std::string getDecodedDataInString() {
			if (decodedValueInString == "") {
				if (representationMode == RepresentationModeEnum::OctalValue) {
					stringstream ss;
					ss << std::oct << (int) decodedValue;
					decodedValueInString = ss.str();
				} else if (representationMode == RepresentationModeEnum::HexadecimalValue) {
					stringstream ss;
					ss << "0x" << std::hex << (int) decodedValue;
					decodedValueInString = ss.str();
				} else
					decodedValueInString = to_string(decodedValue);
			}
			return decodedValueInString;
		}

		std::string* getPointerToDecodedDataInString() {
			if (decodedValueInString == "") {
				if (representationMode == RepresentationModeEnum::OctalValue) {
					stringstream ss;
					ss << std::oct << (int) decodedValue;
					decodedValueInString = ss.str();
				} else if (representationMode == RepresentationModeEnum::HexadecimalValue) {
					stringstream ss;
					ss << "0x" << std::hex << (int) decodedValue;
					decodedValueInString = ss.str();
				} else
					decodedValueInString = to_string(decodedValue);
			}

			return &(decodedValueInString);
		}
	private:
		uint32_t decodedValue;
		RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_UNSIGNEDINTEGER32BITVALUE_HPP_ */
