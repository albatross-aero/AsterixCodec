#ifndef DATAMODEL_ITEM_H
#define DATAMODEL_ITEM_H

#include <config/enum/FunctionNameEnum.hpp>
#include <config/enum/ItemTypeEnum.hpp>
#include <config/enum/RepresentationModeEnum.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"
#include "lib/boost/serialization/base_object.hpp"
#include "lib/boost/serialization/vector.hpp"
#include "utils/ConversionFromStringToEnum.hpp"
#include "data_model/decoder_objects/ElementValue.hpp"

using namespace std;

/**
 * Data structure that represent the valid range with lower bound and upper bound
 * of a specific Element.
 * @param minValue Lower bound. Default 0.
 * @param isMinIncluded True indicates that the 'minValue' is included in the range, otherwise false. Default true.
 * @param maxValue Upper bound. Default 0.
 * @param isMaxIncluded True indicates that the 'maxValue' is included in the range, otherwise false. Default true.
 * @param isEmptyStruct True indicates that there are limits for the current Element. Default false.
 */
struct Range {
		double minValue = 0;
		double maxValue = 0;

		bool isMinIncluded = true;    	// if true, 'currentValue' >= minValue. Otherwise >
		bool isMaxIncluded = true;    	// if true, 'currentValue' <= maxValue. Otherwise <

		// Set to FALSE if it is holding value to be taken into account.
		// TRUE by default, since it is more common the case where NO range has to be defined for an Element
		bool isEmpty = true;

//		Range(double _minValue, double _maxValue)
//				: minValue(_minValue), maxValue(_maxValue) {
//		}
//
//		Range(double _minValue, bool _isMinIncluded, double _maxValue, bool _isMaxIncluded)
//				: minValue(_minValue), maxValue(_maxValue), isMinIncluded(_isMinIncluded), isMaxIncluded(_isMaxIncluded) {
//		}

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & minValue;
			ar & isMinIncluded;
			ar & maxValue;
			ar & isMaxIncluded;
			ar & isEmpty;
		}
};

/**
 * Store the description of an Item.
 * There are some vectors that contains information for the parsing and the decoding of elements.
 */
class Item {

	public:

		Item();

		Item(string _name, int _lengthInByte, ItemTypeEnum _itemType);

		//GETTERS AND SETTERS
		string getName();

		void setName(string name);

		bool isItemMandatory();

		void setItemMandatory(bool _mandatory);

		void setType(ItemTypeEnum _type);

		int getLengthInByte();

		void setLengthInByte(int length);

		vector<string>* getAllElementNames();

		void setAllElementNames(vector<string> _elementsName);

		ItemTypeEnum getItemType();

		vector<int>* getAllElementDescription();

		void setAllElementDescription(vector<int> _elementsLength);

		vector<string>* getAllFunctionNamesForDecodingInString();

		void setAllFunctionNamesForDecodingInString(vector<string> _functionNameForDecoding);

		vector<string>* getAllRepresentationModesInStringForDecoding();

		void setAllRepresentationModesInStringForDecoding(vector<string> _representationModeForDecoding);

		vector<vector<double>>* getLsbValues();

		void setAllLsbValues(vector<vector<double>> _lsbValues);

		vector<Range>* getAllValidRanges();

		void setAllValidRanges(vector<Range> ranges);

		bool isItemToBeDecoded();

		void setToBeDecoded(bool _boolean);

		bool isItemToBeEncoded();

		void setToBeEncoded(bool _boolean);

		vector<RepresentationModeEnum>* getAllRepresentationModesInEnumForDecoding();

		vector<FunctionNameEnum>* getAllFunctionNamesInEnumForDecoding();

		void setAllRepresentationModesInEnumForDecoding(vector<RepresentationModeEnum> _representationModeForDecoding);

		void setAllFunctionNamesInEnumForDecoding(vector<FunctionNameEnum> _functionNameForDecoding);

		//OTHER FUNCTIONS
		int getIndexofElement(string elementName);

		RepresentationModeEnum getRepresentationModeInEnumForDecodingAtIndex(int index);

		int getElementDescriptionAtIndex(int index);

		FunctionNameEnum getFunctionNameInEnumForDecodingAtIndex(int index);

		Range* getValidRangeAtIndex(int index);

		void convertRepresentationModeForDecodingInStringToEnum();

		void convertFunctionNameForDecodingInStringToEnum();

		void convertItemTypeFromStringToEnum();

		//OTHER FUNCTIONS
		string* getElementNameAtIndex(int index);

		string getFunctionNameForDecodingInStringAtIndex(int index);

		string getRepresentationModeInStringForDecodingAtIndex(int index);

		vector<double>* getLsbValuesAtIndex(int index);

		void createRepetitions();

		/**
		 * Method used for Repetitive Item.
		 * Gets the Elements that compose the Item and make copies of them.
		 * The number of copies is calculated as follow: (sizeOfFinalVector - startDimensionOfVector) / (number of Element in the Item module)
		 * For the 'elementNames' field, the new name is created adding a numbered suffix (e.g. '_1', '_2')
		 */
		void addRepetitionInVectors(unsigned short startDimensionOfVector, unsigned short sizeOfFinalVector);

		/**
		 * Calculate the total length in bits of all Elements
		 * @return Length in bits all of elements
		 */
		int getElementsTotalSize();

		unsigned short int getNumberOfElementsInTheModule();

		// Serialization Using Boost of the object
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & name;
			ar & itemType;
			ar & lengthInByte;

			ar & elementNames;
			ar & elementDescriptions;
			ar & functionNameForDecoding;
			ar & representationModeForDecoding;
			ar & lsbValues;
			ar & toBeDecoded;

			ar & validRanges;

			ar & functionNameForDecodingInString;
			ar & representationModeForDecodingInString;

			ar & numberOfElementsInTheModule;
			ar & toBeEncoded;
			ar & mandatory;
		}

	private:
		bool toBeDecoded;bool toBeEncoded;bool mandatory = false;

		string name;

		ItemTypeEnum itemType = ItemTypeEnum::InvalidValue;
		int lengthInByte;

		vector<string> elementNames;
		vector<int> elementDescriptions;
		vector<FunctionNameEnum> functionNameForDecoding;
		vector<RepresentationModeEnum> representationModeForDecoding;
		vector<vector<double>> lsbValues;
		vector<Range> validRanges;

		vector<string> representationModeForDecodingInString;    // used for the JSON Validation
		vector<string> functionNameForDecodingInString;    // used for the JSON Validation

		unsigned short int numberOfElementsInTheModule = 0;		// used for repetitive item to count number of element [default value is 0]
};

#endif //DATAMODEL_ITEM_H
