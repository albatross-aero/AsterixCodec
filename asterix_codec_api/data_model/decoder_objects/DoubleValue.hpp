#ifndef DATA_MODEL_DECODER_OBJECTS_DOUBLEVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_DOUBLEVALUE_HPP_

#include <data_model/decoder_objects/ElementValue.hpp>
#include <string>

/**
 * Store decoded data of an element.
 * The type of value is a double - 8 Bytes.
 * This class is used when the decoded value is a number with 15 decimal digits of precision.
 */
class DoubleValue: public ElementValue {
	public:

		DoubleValue(DecodedValueObjectPoolManager* _ptrToManager)
				: ElementValue(_ptrToManager) {
			decodedValue = 0;
		}

		DoubleValue(DecodedValueObjectPoolManager* _ptrToManager, double _decodedValue)
				: ElementValue(_ptrToManager) {
			decodedValue = _decodedValue;
		}

		virtual ~DoubleValue() {
		}

		double getDecodedValue() {
			return decodedValue;
		}

		void setDecodedValue(double _decodedValue) {
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
		double decodedValue;

};

#endif /* DATA_MODEL_DECODER_OBJECTS_DOUBLEVALUE_HPP_ */
