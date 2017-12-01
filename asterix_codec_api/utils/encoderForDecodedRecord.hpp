#ifndef UTILS_ENCODERFORDECODEDRECORD_H_
#define UTILS_ENCODERFORDECODEDRECORD_H_

#include <config/CommonVariables.h>
#include <config/enum/ItemTypeEnum.hpp>
#include <facade/RecordCollection.h>
#include <string>
#include <vector>

#include "data_model/asterix_description/CategoryDescription.h"
#include "data_model/asterix_description/Item.h"
#include "data_model/decoder_objects/ElementValue.hpp"
#include "utils/ItemEncodingUtils.h"

using namespace std;

// COMMON VARIABLES
FailureReport *encoderFailureReport = FailureReport::getInstance();

/**
 * It encodes and adds the Category number and the Record length in the encoded Record bytestream.
 * @param _categoryNumber Category number of the current Record
 * @param _datablockSize Length in Byte of the current Record
 * @param encodedRecord Pointer to the encoded Record to update with the FSPEC value
 */
void encodeAndSetCatAndLength(unsigned int _categoryNumber, unsigned int _datablockSize, vector<unsigned char>* encodedRecord) {
	unsigned char tmpBytestream = 0x0;    // working temp variable

	// Building the bytestream output for LEN value and placing it in the first vector position
	unsigned int datablockSizeTmp = _datablockSize;
	tmpBytestream |= datablockSizeTmp;    // ..taking just the 8 lowest bits of the Datablock size..
	encodedRecord->insert(encodedRecord->begin(), tmpBytestream);

	tmpBytestream = 0x0;	// reset

	datablockSizeTmp >>= 8;		// ..throwing away the 8 lowest bit of the Datablock size, already written..
	tmpBytestream |= datablockSizeTmp;    // ..taking the 8 highest bits of the Datablock size..
	encodedRecord->insert(encodedRecord->begin(), tmpBytestream);

	// Build raw data for CAT information and insert it in front position of the vector
	tmpBytestream = 0x0;
	tmpBytestream |= _categoryNumber;
	encodedRecord->insert(encodedRecord->begin(), tmpBytestream);
}

/**
 * It encodes all elements of items that are in decoderResult[recordNumber] and it builds the FSPEC.
 * @param decoderResult Pointer to a DecoderResult object, that contains one or more decoded Record
 * @param recordNumber Number of record to encode in the DecoderResult object
 * @param categoryDescription Description of category of the current Record
 * @param categoryNumber Category number of the current Record
 * @return Pointer to a vector of raw data of the encoded Record
 */
vector<unsigned char>* encodeAllItemsFromRecordCollection(RecordCollection* decoderResult, unsigned int recordNumber, CategoryDescription* categoryDescription,
		unsigned int categoryNumber) {
	// Decoded Elements
	vector<string*>* decodedElementNames = nullptr;
	vector<ElementValue*>* decodedElementValues = nullptr;

	// Items information
	Item* itemToEncodeDescription = nullptr;
	unordered_map<int, CategoryDescription*>::iterator categoryDescription_it;
	vector<Item> *itemsDescription = nullptr;
	vector<Item> *compoundSubItemsDescription = nullptr;
	vector<Item> *refItemDescription = nullptr;
	vector<Item> *spfItemDescription = nullptr;

	unsigned int numberOfItems = 0;
	unsigned int lastFRNset = 0;    // To store the last FSPEC position set to 1
	int FX_bitCounter = 0;
	ItemTypeEnum itemType;

	vector<unsigned char>* recordBytestream = new vector<unsigned char>();    // Raw Items inside a record
	vector<unsigned char>* itemBytestream = nullptr;

	unsigned int itemLengthInByte = 0;
	unsigned char* FSPEC = nullptr;
	unsigned int FSPEC_lengthInByte = 0;
	unsigned int FSPEC_currentLengthInBits = 0;
	unsigned int FSPEC_index = 0;    // It moves inside a FSPEC Byte, between position 0 and 7

	// Getting a copy of the decoded Elements vectors, so we may free them back to the Producer (parsing)
	decodedElementNames = decoderResult->makeACopyOfRecordElementNames(recordNumber);
	decodedElementValues = decoderResult->makeACopyOfRecordElementValues(recordNumber);

	// Loading the proper Category description for the current Record
	itemsDescription = categoryDescription->getItemCollection();
	compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();
	refItemDescription = categoryDescription->getRefItemDescription();
	spfItemDescription = categoryDescription->getSpfItemDescription();

	numberOfItems = itemsDescription->size();

	// Now it's time to start building up the FSPEC!
	FSPEC = new unsigned char[(numberOfItems / 8) + 1 + 1];
	// Note that: '(numberOfItems / 8)+1' is the maximum FSPEC size, to which we add another byte to host the terminator '\0'

	// FSPEC initialization to 0
	memset(FSPEC, 0, ((numberOfItems / 8) + 1));

	// Scan each Item in CategoryDescription
	// Note that: if 'decodedElementNames' is found empty, this means that no more Element are waiting to be encoded
	// This happens because when an Element has been successfully encoded, it gets removed from the collection.
	for (unsigned int i = 0; i < numberOfItems && decodedElementNames->size() > 0; i++) {
		// The current Item description gets loaded..
		itemToEncodeDescription = &(itemsDescription->at(i));

		// ..if the Item is not a "Spare" and 'isItemToEncode' field is true (-> setup coming from the JSON filter file)
		if (itemToEncodeDescription->getName() != "-" && itemToEncodeDescription->isItemToBeEncoded()) {
			itemType = itemToEncodeDescription->getItemType();
			switch (itemType) {

			case ItemTypeEnum::FixedLength:
				itemBytestream = ItemEncodingUtils::encodeFixedLengthItem(decodedElementNames, decodedElementValues, itemToEncodeDescription,
						categoryNumber);
				break;

			case ItemTypeEnum::ExtendedLength:
				itemBytestream = ItemEncodingUtils::encodeExtendedLengthItem(decodedElementNames, decodedElementValues, itemToEncodeDescription,
						categoryNumber);
				break;

			case ItemTypeEnum::RepetitiveLength:
				itemBytestream = ItemEncodingUtils::encodeRepetitiveLengthItem(decodedElementNames, decodedElementValues, itemToEncodeDescription,
						categoryNumber);
				break;

			case ItemTypeEnum::CompoundLength:
				itemBytestream = ItemEncodingUtils::encodeCompoundLengthItem(decodedElementNames, decodedElementValues, itemToEncodeDescription,
						compoundSubItemsDescription, categoryNumber);
				break;

			case ItemTypeEnum::ExplicitLength:
				itemBytestream = ItemEncodingUtils::encodeExplicitLengthItem(decodedElementNames, decodedElementValues, itemToEncodeDescription,
						compoundSubItemsDescription, refItemDescription, spfItemDescription,
						categoryNumber);
				break;

			default:
				encoderFailureReport->addFailure(categoryNumber, 0, itemToEncodeDescription->getName(), nullptr, "Unknown Item type!",
						ReturnCodes::ENCODING_ERROR);
				break;
			}
		}

		// if itemBytestream != nullptr -> item is present in the decoded value and the encoded value is in itemBytestream
		if (itemBytestream != nullptr && itemBytestream->size() != 0) {
			itemLengthInByte = itemBytestream->size();
			lastFRNset = i + 1;    // Traces last Item position to cut FSPEC at the right byte

			// 1) Writing the Item bytestream to the Record bytestream..
			// Saving each Item Byte in the Records vector
			for (unsigned int itemByteIndex = 0; itemByteIndex < itemLengthInByte; itemByteIndex++) {
				recordBytestream->push_back(itemBytestream->at(itemByteIndex));
			}

			// 2) Setting the proper FSPEC bit to 1, to mark the Item presence..
			// Note that: each FSPEC Byte gets built bit-by-bit, from right to left ==> this is why, before setting the bit to 1, you shift left by 1.
			FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x1;

			// 3) Resetting 'itemBytestream'..
			delete itemBytestream;
			itemBytestream = nullptr;
		} else {
			//Item is not present in the values to encode -> put to 0 the correspondent position in the FSPEC
			FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x0;

			// Logging a failure
			if (itemToEncodeDescription->isItemMandatory() == true) {
				encoderFailureReport->addFailure(categoryNumber, 0, itemToEncodeDescription->getName(), nullptr,
						"A Mandatory Item for the current Category was missing!", ReturnCodes::MISSING_MANDATORY_ITEM);
			}
		}
		FSPEC_currentLengthInBits++;

		//When (FSPEC_currentIndex % 7 == 0), it's time to mark to 1 the Field Extension bit.
		if (FSPEC_currentLengthInBits % 7 == 0) {
			FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x1;
			FSPEC_index++;
		}
	}

	// Calculating the FX bits to FSPEC_currentLengthInBits and lastItemPresencePosition variables [???]
	// Note that: "FSPEC_currentLengthInBits" has skipped - until now - the FX bits set.
	FX_bitCounter = FSPEC_currentLengthInBits / 7;
	FSPEC_currentLengthInBits += FX_bitCounter;    // Now "FSPEC_currentLengthInBits" considers also the number of FX bits set.
	// ** //
	FX_bitCounter = lastFRNset / 7;		// "FX_bitCounter" on final FSPEC array
	unsigned short int buildingFSPEC_lengthInBits = lastFRNset + FX_bitCounter;

	// Calculating the FSPEC length
	FSPEC_lengthInByte = buildingFSPEC_lengthInBits / 8;
	if (buildingFSPEC_lengthInBits % 8 > 0) {
		FSPEC_lengthInByte++;
	}

	// Filling the very last Byte of FSPEC with 0s - if necessary [FSPEC_currentLengthInBits < FSPEC_lengthInByte * 8]
	for (unsigned int i = FSPEC_currentLengthInBits; i < FSPEC_lengthInByte * 8; i++)
		FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x0;

	// Removing extra Byte in the FSPEC and putting the last FX to 0
	if (FSPEC_lengthInByte > 0)
		FSPEC[FSPEC_lengthInByte - 1] = FSPEC[FSPEC_lengthInByte - 1] & 0xfe;

	for (int i = FSPEC_lengthInByte - 1; i >= 0; i--) {
		recordBytestream->insert(recordBytestream->begin(), FSPEC[i]);
	}

	// Free memory used to calcolate the FSPEC
	delete[] FSPEC;
	FSPEC = nullptr;

	if (CommonVariables::getInstance()->isEncodingDebugEnabled()) {
		cout << "FSPEC: " << endl;
		for (unsigned int i = 0; i < FSPEC_lengthInByte; i++) {
			cout << " " << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(recordBytestream->at(i));
		}
		cout << endl;
	}
	return recordBytestream;
}

/**
 * Encodes all Elements of Items that are in recordToEncode and build the FSPEC.

 * @param recordToEncode Pointer to a Record object, that contains one Record
 * @param categoryDescription Description of category of the current record
 * @param categoryNumber Number of category
 * @return Pointer to a vector of raw data of the encoded record
 */
vector<unsigned char>* encodeAllItemsFromSingleRecord(Record* recordToEncode, CategoryDescription* categoryDescription, unsigned short int categoryNumber) {
	vector<unsigned char>* recordBytestream = new vector<unsigned char>();    // Raw Items inside a record
	vector<unsigned char>* itemBytestream = nullptr;

	// Category description
	vector<Item>* itemsDescription = categoryDescription->getItemCollection();
	vector<Item>* compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();
	vector<Item>* refItemDescription = categoryDescription->getRefItemDescription();
	vector<Item>* spfItemDescription = categoryDescription->getSpfItemDescription();
	// Item (to encode) description
	Item* itemToEncodeDescription = nullptr;

	// FSPEC
	unsigned char* FSPEC = nullptr;
	unsigned int FSPEC_lengthInByte = 0;
	unsigned int FSPEC_currentLengthInBits = 0;
	unsigned int FSPEC_index = 0;    // It moves inside a FSPEC Byte, between position 0 and 7
	unsigned int numberOfItems = 0;
	unsigned short int lastFRNset = 0;
	unsigned int FX_bitCounter = 0;

	// Record content
	vector<string*>* elementNames = new vector<string*>();
	vector<ElementValue*>* elementValues = new vector<ElementValue*>();
	string elName;
	ElementValue* ev1 = nullptr;

	//Create two vectors 'elementNames' and 'elementValues' from the map of Element given by Record object. These vectors are used during the encoding phase
	for (map<string, ElementValue*>::iterator element_it = recordToEncode->getElements()->begin(); element_it != recordToEncode->getElements()->end();
			element_it++) {
		ev1 = element_it->second;
		elementNames->push_back(new string(element_it->first.c_str()));
		elementValues->push_back(ev1);
	}

	numberOfItems = itemsDescription->size();

	// Now it's time to start building up the FSPEC!
	FSPEC = new unsigned char[(numberOfItems / 8) + 1 + 1];
	// Note that: '(numberOfItems / 8)+1' is the maximum FSPEC size, to which we add another byte to host the terminator '\0'

	// FSPEC initialization to 0
	memset(FSPEC, 0, ((numberOfItems / 8) + 1));

	// In the following loop, 'i' is holding the expression (FRN - 1)
	for (unsigned int i = 0; i < numberOfItems && elementNames->size() > 0; i++) {
		// The current Item description gets loaded..
		itemToEncodeDescription = &(itemsDescription->at(i));

		if (itemToEncodeDescription->getName() != "-" && itemToEncodeDescription->isItemToBeEncoded()) {

			switch (itemToEncodeDescription->getItemType()) {
			case ItemTypeEnum::FixedLength:
				itemBytestream = ItemEncodingUtils::encodeFixedLengthItem(elementNames, elementValues, itemToEncodeDescription, categoryNumber);
				break;

			case ItemTypeEnum::ExtendedLength:
				itemBytestream = ItemEncodingUtils::encodeExtendedLengthItem(elementNames, elementValues, itemToEncodeDescription, categoryNumber);
				break;

			case ItemTypeEnum::RepetitiveLength:
				itemBytestream = ItemEncodingUtils::encodeRepetitiveLengthItem(elementNames, elementValues, itemToEncodeDescription, categoryNumber);
				break;

			case ItemTypeEnum::CompoundLength:
				itemBytestream = ItemEncodingUtils::encodeCompoundLengthItem(elementNames, elementValues, itemToEncodeDescription,
						compoundSubItemsDescription, categoryNumber);
				break;

			case ItemTypeEnum::ExplicitLength:
				itemBytestream = ItemEncodingUtils::encodeExplicitLengthItem(elementNames, elementValues, itemToEncodeDescription,
						compoundSubItemsDescription, refItemDescription, spfItemDescription,
						categoryNumber);
				break;

			default:
				encoderFailureReport->addFailure(categoryNumber, 0, itemToEncodeDescription->getName(), nullptr, "Unknown Item type!",
						ReturnCodes::ENCODING_ERROR);
				break;
			}
		}

		// if itemBytestream != nullptr -> item is present in the decoded value and the encoded value is in itemBytestream
		if (itemBytestream != nullptr && itemBytestream->size() != 0) {
			lastFRNset = i + 1;    // 'i' is holding the expression (FRN - 1)

			// 1) Writing the Item bytestream to the Record bytestream..
			// Saving each Item Byte in the Records vector
			for (unsigned int itemByteIndex = 0; itemByteIndex < itemBytestream->size(); itemByteIndex++) {
				recordBytestream->push_back(itemBytestream->at(itemByteIndex));
			}

			// 2) Setting the proper FSPEC bit to 1, to mark the Item presence..
			// Note that: each FSPEC Byte gets built bit-by-bit, from right to left ==> this is why, before setting the bit to 1, you shift left by 1.
			FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x1;

			// 3) Resetting 'itemBytestream'..
			delete itemBytestream;
			itemBytestream = nullptr;
		} else {
			//Item is not present in the decoded value -> put to 0 the correspondent position in the FSPEC
			FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x0;

			// Logging a failure
			if (itemToEncodeDescription->isItemMandatory() == true) {
				encoderFailureReport->addFailure(categoryNumber, 0, itemToEncodeDescription->getName(), nullptr,
						"A Mandatory Item for the current Category was missing!", ReturnCodes::MISSING_MANDATORY_ITEM);
			}
		}
		FSPEC_currentLengthInBits++;

		//When (FSPEC_currentIndex % 7 == 0), it's time to mark to 1 the Field Extension bit.
		if (FSPEC_currentLengthInBits % 7 == 0) {
			FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x1;
			FSPEC_index++;
		}
	}

	// Note that: "FSPEC_currentLengthInBits" has skipped - until now - the FX bits set.

	// Calculating the number of FX bit set. This number should be added to 'FSPEC_currentLengthInBits' and 'lastFRNset' variables
	FX_bitCounter = FSPEC_currentLengthInBits / 7;
	FSPEC_currentLengthInBits += FX_bitCounter;    // Now "FSPEC_currentLengthInBits" considers also the number of FX bits set.
	// ** //
	FX_bitCounter = lastFRNset / 7;		// "FX_bitCounter" on final FSPEC array
	unsigned short int buildingFSPEC_lengthInBits = lastFRNset + FX_bitCounter;

	// Calculating the FSPEC length
	FSPEC_lengthInByte = buildingFSPEC_lengthInBits / 8;
	if (buildingFSPEC_lengthInBits % 8 > 0) {
		FSPEC_lengthInByte++;
	}

	// Filling the very last Byte of FSPEC with 0s - if necessary [FSPEC_currentLengthInBits < FSPEC_lengthInByte * 8]
	for (unsigned int i = FSPEC_currentLengthInBits; i < FSPEC_lengthInByte * 8; i++)
		FSPEC[FSPEC_index] = FSPEC[FSPEC_index] << 1 | 0x0;

	// Removing extra Byte in the FSPEC and putting the last FX to 0
	if (FSPEC_lengthInByte > 0)
		FSPEC[FSPEC_lengthInByte - 1] = FSPEC[FSPEC_lengthInByte - 1] & 0xfe;

	for (int i = FSPEC_lengthInByte - 1; i >= 0; i--) {
		recordBytestream->insert(recordBytestream->begin(), FSPEC[i]);
	}

	// Free memory used to calcolate the FSPEC
	delete[] FSPEC;
	FSPEC = nullptr;

	if (CommonVariables::getInstance()->isEncodingDebugEnabled()) {
		cout << "FSPEC: " << endl;
		for (unsigned int i = 0; i < FSPEC_lengthInByte; i++) {
			cout << " " << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(recordBytestream->at(i));
		}
		cout << endl;
	}
	return recordBytestream;
}

/**
 * Given a RecordCollection (bunch of Records) and the Record number, the function encodes the Record and puts it in a Datablock (1 Record per Datablock).
 * @param recordsToEncode Pointer to a RecordCollection object, holding one or more decoded Record
 * @param recordIndex Index of the Record to be encoded in the 'recordsToEncode' collection
 * @return Pointer to the encoded Record bytestream
 */
unsigned char* encodeForDecodedRecord(RecordCollection* recordsToEncodeCollection, unsigned int recordIndex) {
	/************************
	 * VARIABLES DEFINITION *
	 ************************/
	CommonVariables* common = CommonVariables::getInstance();
	unsigned char* encodedRecord = nullptr; 		// To store the final encoding result

	// To manage Category descriptions and SAC/SIC Element values
	unordered_map<int, CategoryDescription*>* categoriesDescription = common->getCategoriesDescription();
	unordered_map<int, CategoryDescription*>::iterator categoryDescription_it;
	CategoryDescription* categoryDescription = nullptr;
	ElementValue* elementValue = nullptr; 		// To temporarily store the fetched Element value
	int sacSicKey = 0;

	// Record information
	vector<unsigned char>* recordBytestream = nullptr; 		// To store encoded Bytes

	// Datablock information
	unsigned int categoryNumber = 0;
	unsigned int datablockSize = 0;

	/********************************
	 * Category description LOADING *
	 ********************************/
	// GET information of the encoding Record
	categoryNumber = recordsToEncodeCollection->getCategoryNumberOfARecord(recordIndex);

	sacSicKey = categoryNumber * 1000 * 1000;    // CAT number move to left to fit SAC (3 digits) and SIC (3 digits)

	// GET Source Identifier Elements
	// {SAC}
	elementValue = recordsToEncodeCollection->getElementValueOfRecord(recordIndex, "SAC");
	if (elementValue != nullptr) {
		int sac = atoi(elementValue->getDecodedDataInString().c_str());
		sacSicKey += (sac * 1000);
	}

	elementValue = nullptr;
	// {SIC}
	elementValue = recordsToEncodeCollection->getElementValueOfRecord(recordIndex, "SIC");
	if (elementValue != nullptr) {
		int sic = atoi(elementValue->getDecodedDataInString().c_str());
		sacSicKey += sic;
	}

	// Lookup of the Cateogry description key
	//categoryDescription_it = categoriesDescription->find(stoi(categoryDescriptionKey.str()));
	categoryDescription_it = categoriesDescription->find(sacSicKey);
	// Resetting the stringstream..

	// If the proper description has NOT been found..
	if (categoryDescription_it == categoriesDescription->end()) {
		// ..we look for the DEFAULT Category description (not SourceId-specific)
		categoryDescription_it = categoriesDescription->find(categoryNumber);
	}

	// If not even the default Category description has been found, throw an error!
	if (categoryDescription_it == categoriesDescription->end()) {
		encoderFailureReport->addFailure(categoryNumber, 0, "", nullptr, "Unknown Category number!", ReturnCodes::UNKNOWN_CATEGORY);
		return nullptr;
	}

	// GET the Category description from the 'categoryDescription_it' iterator
	categoryDescription = categoryDescription_it->second;

	// Check if the Category is to decode
	if(!categoryDescription->isToBeEncoded()){
		return nullptr;
	}

	/******************
	 * START ENCODING *
	 ******************/
	recordBytestream = encodeAllItemsFromRecordCollection(recordsToEncodeCollection, recordIndex, categoryDescription, categoryNumber);

	if (recordBytestream == nullptr || recordBytestream->size() == 0) {
		// No Failure to report here, because they have already been set by the encoding functions.
		return nullptr;
	}

	// Get the total size of the Datablock to build (1 Record in 1 Datablock)
	datablockSize = recordBytestream->size() + 3;    // 1 Byte {CAT} + 2 Byte {LEN}

	// Add CAT and LEN at the beginning of the vector
	encodeAndSetCatAndLength(categoryNumber, datablockSize, recordBytestream);

	// Initialization of 'encodedRecord'
	encodedRecord = new unsigned char[datablockSize];
	memset(encodedRecord, 0, datablockSize);

	// Saving the whole bytestream just built in the 'encodedRecord' variable..
	std::copy(recordBytestream->begin(), recordBytestream->end(), encodedRecord);

	// Free memory used by the Record bytestream
	delete recordBytestream;
	recordBytestream = nullptr;

	return encodedRecord;
}

/**
 * Given a Record, the function encodes the Record and puts it in a Datablock.
 * @param recordToEncode Pointer to a Record object, holding one Record
 * @return Pointer to the encoded Record bytestream
 */
unsigned char* encodeForSingleRecord(Record* recordToEncode) {
	CommonVariables* common = CommonVariables::getInstance();
	unsigned char* encodedDatablock = nullptr;		// Final encoding result
	vector<unsigned char>* recordBytestream = nullptr;		// Record bytestream building up
	unsigned short int categoryNumber = recordToEncode->getCategoryNumber();
	unsigned int datablockLengthInByte = 0;

	// To manage Category descriptions and SAC/SIC Element values
	unordered_map<int, CategoryDescription*>* categoriesDescription = common->getCategoriesDescription();
	unordered_map<int, CategoryDescription*>::iterator categoryDescription_it;
	CategoryDescription* categoryDescription = nullptr;
	stringstream categoryDescriptionKey;
	string sacValue, sicValue;
	map<string, ElementValue*>* elementsMap = nullptr;
	map<string, ElementValue*>::iterator element_it;

	elementsMap = recordToEncode->getElements();

	if ((elementsMap == nullptr) || (elementsMap->size() == 0)) {
		encoderFailureReport->addFailure(categoryNumber, 0, "", nullptr, "Record to be encoded is empty!", ReturnCodes::ENCODING_ERROR);
		return nullptr;
	}

	// Build the key for Category description
	element_it = elementsMap->find("SAC");
	if (element_it != elementsMap->end()) {
		sacValue = (*element_it).second->getDecodedDataInString();
	}

	element_it = elementsMap->find("SIC");
	if (element_it != elementsMap->end()) {
		sicValue = (*element_it).second->getDecodedDataInString();
	}

	categoryDescriptionKey << categoryNumber;
	categoryDescriptionKey << sacValue;
	categoryDescriptionKey << sicValue;

	// Lookup of the Category description key
	categoryDescription_it = categoriesDescription->find(stoi(categoryDescriptionKey.str()));
	// Resetting the stringstream..
	categoryDescriptionKey.str(string());
	categoryDescriptionKey.clear();

	// If the proper description has NOT been found..
	if (categoryDescription_it == categoriesDescription->end()) {
		// ..we look for the DEFAULT Category description (not SourceId-specific)
		categoryDescription_it = categoriesDescription->find(categoryNumber);
	}

	// If not even the default Category description has been found, throw an error!
	if (categoryDescription_it == categoriesDescription->end()) {
		encoderFailureReport->addFailure(categoryNumber, 0, "", nullptr, "Unknown Category number!", ReturnCodes::UNKNOWN_CATEGORY);
		return nullptr;
	}

	categoryDescription = categoryDescription_it->second;

	// Check if the Category is to decode
	if(!categoryDescription->isToBeEncoded()){
		return nullptr;
	}

	/******************
	 * START ENCODING *
	 ******************/
	recordBytestream = encodeAllItemsFromSingleRecord(recordToEncode, categoryDescription, categoryNumber);

	if (recordBytestream == nullptr || recordBytestream->size() == 0) {
		// No Failure to report here, because they have already been set by the encoding functions.
		return nullptr;
	}

	// Get the total size of the Datablock to build (1 Record in 1 Datablock)
	datablockLengthInByte = recordBytestream->size() + 3;    // 1 Byte {CAT} + 2 Byte {LEN}

	// Add CAT and LEN at the beginning of the vector
	encodeAndSetCatAndLength(categoryNumber, datablockLengthInByte, recordBytestream);

	// Initialization of 'encodedRecord'
	encodedDatablock = new unsigned char[datablockLengthInByte];
	memset(encodedDatablock, 0, datablockLengthInByte);

	// Saving the whole bytestream just built in the 'encodedRecord' variable..
	copy(recordBytestream->begin(), recordBytestream->end(), encodedDatablock);

	// Free memory used by the Record bytestream
	delete recordBytestream;
	recordBytestream = nullptr;

	return encodedDatablock;
}

#endif /* UTILS_ENCODERFORDECODEDRECORD_H_ */
