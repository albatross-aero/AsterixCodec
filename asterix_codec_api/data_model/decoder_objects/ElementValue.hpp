#ifndef DATA_MODEL_DECODER_OBJECTS_ELEMENTVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_ELEMENTVALUE_HPP_

#include <config/enum/RepresentationModeEnum.hpp>
#include <string>


using namespace std;

class DecodedValueObjectPoolManager;
//It is a way to allow that two classes refer to each other (Inside DecodedValueObjectPoolManager class, there are include files that include this class)

/**
 * Abstract class, is a base type for all possible decoded value.
 * His children contain decoded value.
 * There is one child for each 'representationModeForDecoding' value in the 'asterixConfiguration' JSON file.
 */
class ElementValue {
	public:

		ElementValue(DecodedValueObjectPoolManager* _ptrToManager) {
			ptrToManager = _ptrToManager;
		}

		virtual ~ElementValue() {
			ptrToManager = nullptr;
		}

		virtual void resetElementValue() = 0;

		virtual std::string getDecodedDataInString() = 0;

		virtual std::string* getPointerToDecodedDataInString() = 0;

		DecodedValueObjectPoolManager* getPointerToManager() {
			return ptrToManager;
		}

	private:
		DecodedValueObjectPoolManager* ptrToManager;	// this class is not responsible for the deallocation of the pointer

	protected:
		string decodedValueInString;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_ELEMENTVALUE_HPP_ */
