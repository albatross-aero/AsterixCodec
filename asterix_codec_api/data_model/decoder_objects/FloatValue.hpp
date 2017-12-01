#ifndef DATA_MODEL_DECODER_OBJECTS_FLOATVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_FLOATVALUE_HPP_

#include <data_model/decoder_objects/ElementValue.hpp>
#include <string>


/**
 * Store decoded data of an element.
 * The type of value is a float - 4 Bytes.
 * This class is used when the decoded value is a number with 7 decimal digits of precision.
 */
class FloatValue: public ElementValue {
	public:

		FloatValue(DecodedValueObjectPoolManager* _ptrToManager)
				: ElementValue(_ptrToManager) {
			decodedValue = 0.0;
		}

		FloatValue(DecodedValueObjectPoolManager* _ptrToManager, float _decodedValue)
				: ElementValue(_ptrToManager) {
			decodedValue = _decodedValue;
		}

		virtual ~FloatValue() {

		}

		float getDecodedValue() {
			return decodedValue;
		}

		void setDecodedValue(float _decodedValue) {
			decodedValue = _decodedValue;
			if (decodedValueInString != "")
				decodedValueInString = "";
		}

		void resetElementValue() {
			decodedValue = 0;
		}

		std::string getDecodedDataInString() {
			if (decodedValueInString == "")
				decodedValueInString = to_string(decodedValue);

			return decodedValueInString;
		}

		std::string* getPointerToDecodedDataInString() {
			if (decodedValueInString == "")
				decodedValueInString = to_string(decodedValue);

			return &(decodedValueInString);
		}

	private:
		float decodedValue;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_FLOATVALUE_HPP_ */
