#ifndef DATA_MODEL_DECODER_OBJECTS_INTEGERVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_INTEGERVALUE_HPP_

#include <data_model/decoder_objects/ElementValue.hpp>
#include <string>


/**
 * Store decoded data of an element.
 * The type of value is a int - 4 Bytes.
 * This class is used when the decoded value is a integer.
 */
class IntegerValue: public ElementValue {

	public:

		IntegerValue(DecodedValueObjectPoolManager* _ptrToManager)
				: ElementValue(_ptrToManager) {
			decodedValue = 0;
		}

		virtual ~IntegerValue() {

		}

		int getDecodedValue() {
			return decodedValue;
		}

		void setDecodedValue(int _decodedValue) {
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
		int decodedValue;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_INTEGERVALUE_HPP_ */
