#ifndef DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER8BITVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER8BITVALUE_HPP_

#include <data_model/decoder_objects/ElementValue.hpp>
#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>


/**
 * Store decoded data of an element.
 * The type of value is a int8_t - 1 Byte.
 * This class is used when the decoded value is a integer.
 */

class SignedInteger8bitValue: public ElementValue {
	public:
		SignedInteger8bitValue(DecodedValueObjectPoolManager* _ptrToManager)
				: ElementValue(_ptrToManager) {
			decodedValue = 0;
			representationMode = RepresentationModeEnum::Signed8bitIntegerValue;
		}

		SignedInteger8bitValue(DecodedValueObjectPoolManager* _ptrToManager, int8_t _decodedValue, RepresentationModeEnum _representationMode)
				: ElementValue(_ptrToManager) {
			decodedValue = _decodedValue;
			representationMode = _representationMode;
		}

		virtual ~SignedInteger8bitValue() {

		}

		int8_t getDecodedValue() const {
			return decodedValue;
		}

		void setDecodedValue(int8_t _decodedValue, RepresentationModeEnum _representationMode = RepresentationModeEnum::Signed8bitIntegerValue) {
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
		int8_t decodedValue;
		RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_SIGNEDINTEGER8BITVALUE_HPP_ */
