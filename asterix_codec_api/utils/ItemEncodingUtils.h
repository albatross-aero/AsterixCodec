#ifndef UTILS_ITEMENCODINGUTILS_H_
#define UTILS_ITEMENCODINGUTILS_H_

#include <config/CommonVariables.h>
#include <config/Constants.h>
#include <config/enum/FunctionNameEnum.hpp>
#include <config/enum/ItemTypeEnum.hpp>
#include <config/FacadeCommonVariables.h>
#include <vector>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>

#include "data_model/decoder_objects/ElementValue.hpp"
#include "data_model/asterix_description/Item.h"
#include "facade/reports/FailureReport.h"
#include "utils/BitUtils.h"
#include "utils/ElementEncodingUtils.h"

class ItemEncodingUtils {

	public:

		/**
		 * It encodes a fixed length Item, given the original Item description and the vectors with the information of the decoded elements.
		 * @param decodedElementNames Pointer to a vector containing the Element names of the current Record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 * @param itemToEncode Pointer to the description to the Item to be encoded
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Pointer to encoded Item bytestream
		 */
		static std::vector<unsigned char>* encodeFixedLengthItem(vector<string*>* decodedElementNames, vector<ElementValue*>* decodedElementValues,
				Item* itemToEncode, unsigned int categoryNumber);

		/**
		 * It encodes an extended length Item, given the original Item description and the vectors with the information of the decoded elements.
		 * @param decodedElementNames Pointer to a vector containing the Element names of the current Record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 * @param itemToEncode Pointer to the description to the Item to be encoded
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Pointer to encoded Item bytestream
		 */
		static std::vector<unsigned char>* encodeExtendedLengthItem(vector<string*>* decodedElementNames, vector<ElementValue*>* decodedElementValues,
				Item* itemToEncode, unsigned int categoryNumber);

		/**
		 * It encodes a repetitive length Item, given the vectors of decoded elements, the description of current Item and the description of all compound Items.
		 * This method calculate the size of the current Item and store the value in ItemSize.
		 * @param decodedElementNames Pointer to a vector containing the Element names of the current Record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 * @param itemToEncode Pointer to the description to the Item to encode
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Pointer to encoded Item bytestream
		 */
		static std::vector<unsigned char>* encodeRepetitiveLengthItem(vector<string*>* decodedElementNames, vector<ElementValue*>* decodedElementValues,
				Item* itemToEncode, unsigned int categoryNumber);

		/**
		 * It encodes a compound Item, given the vectors of decoded elements, the description of current Item and the description of all compound Items.
		 * This method calculate the size of the current Item and store the value in ItemSize.
		 * @param decodedElementNames Pointer to vector that contains names of decoded elements of current record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 * @param itemToEncodeDescription Pointer to the description to the Item to encode
		 * @param compoundSubItemsDescription Pointer to a vector of description of compound Items
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Pointer to encoded Item bytestream
		 */
		static std::vector<unsigned char>* encodeCompoundLengthItem(vector<string*>* decodedElementNames, vector<ElementValue*>* decodedElementValues,
				Item* itemToEncodeDescription, std::vector<Item> *compoundSubItemsDescription, unsigned int categoryNumber);

		/**
		 * It encodes a RE Item, given the original Item descriptions and the vectors with the information of the decoded elements.
		 * @param decodedElementNames Pointer to a vector containing the Element names of the current Record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 * @param itemToEncode Pointer to the RE Item description
		 * @param compoundSubItemsDescription Pointer to the description of the compound Items
		 * @param refItemDescription Pointer to the description of the sub Items of the RE Item
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Pointer to encoded Item bytestream
		 */
		static std::vector<unsigned char>* encodeREFItem(vector<string*>* decodedElementNames, vector<ElementValue*>* decodedElementValues, Item* itemToEncode,
				std::vector<Item> *compoundSubItemsDescription, std::vector<Item> *refItemDescription, unsigned int categoryNumber);

		/**
		 * It encodes an SP Item, given the original Item descriptions and the vectors with the information of the decoded elements.
		 * @param decodedElementNames Pointer to a vector containing the Element names of the current Record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 * @param itemToEncode Pointer to the SPF Item description
		 * @param compoundSubItemsDescription Pointer to the compound Item descriptions
		 * @param spfItemDescription Pointer to the description of the SPF Item subItems
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Pointer to encoded Item bytestream
		 */
		static std::vector<unsigned char>* encodeSPFItem(vector<string*>* decodedElementNames, vector<ElementValue*>* decodedElementValues, Item* itemToEncode,
				std::vector<Item> *compoundSubItemsDescription, std::vector<Item> *spfItemDescription, unsigned int categoryNumber);

		/**
		 * It encodes an explicit length Item (REF or SPF), given all the original Item descriptions and the vectors with the information of the decoded elements.
		 * @param decodedElementNames Pointer to a vector containing the Element names of the current Record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 * @param itemToEncode Pointer to the description of the explicit length Item
		 * @param compoundSubItemsDescription Pointer to the description of the compound Items
		 * @param refItemDescription Pointer to the description of the REF Item subItems
		 * @param spfItemDescription Pointer to the description of the SPF Item subItems
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Pointer to encoded Item bytestream
		 */
		static std::vector<unsigned char>* encodeExplicitLengthItem(vector<string*>* decodedElementNames, vector<ElementValue*>* decodedElementValues,
				Item* itemToEncode, std::vector<Item> *compoundSubItemsDescription, std::vector<Item> *refItemDescription,
				std::vector<Item> *spfItemDescription, unsigned int categoryNumber);

	private:
		/**
		 * Constructor.<br>
		 * This class has only static fields and functions therefore this method is private.
		 */
		ItemEncodingUtils();

		/**
		 * Virtual destructor.
		 */
		virtual ~ItemEncodingUtils();

		/**
		 * It returns the index in decodedElementNames vector to the Element name that contains the decoded value of a given Element,
		 * specified via its name. If the elementName is not in the 'decodedElementNames' vector, the method return -1.
		 * @param decodedElementNames Pointer to vector that contains the decoded Element names of current Record
		 * @param elementName Element to be searched
		 * @return Index in decodedElementValues, if it exists
		 */
		static int getIndexOfDecodedValueFromName(vector<string*>* decodedElementNames, string elementName);

		/**
		 * Remove decoded element value and name from the vectors.
		 * @param indexOfElement Index of element to remove from the vectors
		 * @param decodedElementNames Pointer to vector that contains the decoded Element names of current Record
		 * @param decodedElementValues Pointer to a vector containing the decoded Element values of the current Record
		 */
		static void removeDecodedElementNameAndValueFromCollections(unsigned int indexOfElement, vector<string*>* decodedElementNames,
				vector<ElementValue*>* decodedElementValues);

		/**
		 * It calculates the length in byte of an extended length Item checking how many extensions there are in the vector of decoded elements.
		 * Return 0 if the Item is not present in the decoded values.
		 * @param decodedElementNames Pointer to a vector containing the Element names of the current Record
		 * @param itemToEncode Pointer to the description to the Item to be encoded
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @return Length in byte of the current extended Item
		 */
		static unsigned int calculateExtendedItemLength(vector<string*>* decodedElementNames, Item* itemToEncode, unsigned int categoryNumber);

		static FailureReport *encoderFailureReport;

		static CommonVariables* commonVariables;
};

#endif /* UTILS_ITEMENCODINGUTILS_H_ */
