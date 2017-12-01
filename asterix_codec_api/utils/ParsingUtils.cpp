#include "ParsingUtils.h"

// Declaration of static fields  -  We can't initialize here the BlockingQueueManger because we need a parameter from command line for allocate the right number of obejct
BlockingQueueManager* ParsingUtils::bqManager = nullptr;
CommonVariables* ParsingUtils::commonVariables = CommonVariables::getInstance();
FacadeCommonVariables* ParsingUtils::facadeCommonVariables = FacadeCommonVariables::getInstance();
FailureReport *ParsingUtils::failureReport = FailureReport::getInstance();

void ParsingUtils::parseElement(int lengthInBits, int offset, int octetsSize, unsigned char *octets) {
	// Variables definition
	int bytePerElem = 0, shift = 0, remainder = 0;
	unsigned char *masks;

	masks = commonVariables->getMasks();

	bytePerElem = lengthInBits / 8;

	if (lengthInBits % 8 > 0)
		bytePerElem++;

	// (octetsSize * 8): number of bits of octets where the element is located
	shift = (octetsSize * 8) - (lengthInBits + offset);
	if (shift > 0)
		BitUtils::shiftRight(octets, octetsSize, shift);

	if (octetsSize > bytePerElem)
		memcpy(octets, octets + (1), (size_t) octetsSize - bytePerElem);

	// Every unnecessary bit is dropped through an AND operation with a specific mask
	remainder = lengthInBits % 8;

	if (remainder != 0) {
		octets[0] = octets[0] & masks[remainder];
	}
}

unsigned long long ParsingUtils::parseDatablock(unsigned char *ASTERIX_datablock, unsigned short int datablockCategory, int datablockLength,
		unordered_map<int, CategoryDescription*>* categoriesDescription, int bufferIndex) {

	unsigned long long recordNumber = 0;				// Number of record that have been parsed in the current datablock

	bqManager = BlockingQueueManager::getInstance();

	if (commonVariables->isDebugEnabledForDecoder())
		std::cout << "CAT: " << datablockCategory << std::endl;

	unsigned long int threadId = pthread_self();

	recordNumber += parseAllRecords(ASTERIX_datablock, bufferIndex, datablockCategory, datablockLength, categoriesDescription, threadId);
	return recordNumber;
}

int ParsingUtils::parseAllRecords(unsigned char *ASTERIX_datablock, int bufferIndex, int datablockCategory, int datablockLength,
		unordered_map<int, CategoryDescription*>* categoriesDescription, unsigned long int threadId) {

	/************************
	 * VARIABLES DEFINITION *
	 ************************/
	int bytesRead = 0;    				// Bytes that have already been read inside the current datablock
	int recordStartIndex = 0;    				// Bytes that have already been read inside the current datablock
	int recordSizeInByte = 0;
	int FRN_listSize = -1;    			// Size of the Item list, where all the Item present in the current Record are stored
	unsigned int FRN = 0;    					// Index to access the Item list, where all the Item present in the current Record are stored
	int itemLengthInByte = 0;
	unsigned long long recordNumber = 0;
	unsigned char* rawRecord = nullptr;

	ItemTypeEnum itemType;
	Item* itemToParse = nullptr;
	CategoryDescription* categoryDescription;

	ParsedRecord* objectPool = nullptr;

	bqManager = BlockingQueueManager::getInstance();

	// Key build with <CAT><SAC><SIC> to lookup the Category Description map
	int sacSicKey = 0;

	/***************************
	 * COLLECTIONS DECLARATION *
	 ***************************/
	std::vector<int> FRN_list = std::vector<int>();
	std::vector<Item> *itemsDescription = nullptr;
	std::vector<Item> *compoundSubItemsDescription = nullptr;
	std::vector<Item> *refItemDescription = nullptr;
	std::vector<Item> *spfItemDescription = nullptr;
	std::unordered_map<int, CategoryDescription*>::iterator categoryDescription_it;

	// Reading the datablock Category number (1 Byte) and Length value (2 Bytes)
	bytesRead = 3;

	while (bytesRead < datablockLength) {
		objectPool = bqManager->getObjectPoolForProducer(threadId);

		/**********************************************************
		 * Looking for the category and SAC/SIC to lookup in the  *
		 * Asterix Category description map how to parse Datablock*
		 **********************************************************/

		/******************
		 * RECORD parsing *
		 ******************/
		recordStartIndex = bytesRead;

		bytesRead += readFspec(ASTERIX_datablock, bufferIndex + bytesRead, &FRN_list);
		FRN_listSize = FRN_list.size();

		// Parsing SAC/SIC to build up the key to lookup in the Category description
		// list if datasource has its own definition: <Cat number><SAC in Hex><SIC in Hex>
		// If it has not a defined Category Description, the default will be search

		sacSicKey = datablockCategory * 1000 * 1000;    // CAT number move to left to fit SAC (3 digits) and SIC (3 digits)
		sacSicKey += ParsingUtils::parseFirstTwoBytes(ASTERIX_datablock, bufferIndex + bytesRead);

		categoryDescription_it = categoriesDescription->find(sacSicKey);

		// Checking if the Datasource has a description that differs from the default
		if (categoryDescription_it == categoriesDescription->end()) {
			// Looking for the default decoder of the category
			categoryDescription_it = categoriesDescription->find(datablockCategory);
		}

		// If 'datablockCategory' is not inside the 'categoriesDescription' collection => the datablock is skipped
		if (categoryDescription_it == categoriesDescription->end() || FRN_listSize == 0) {
			bqManager->releaseObjectPoolForProducer(-1, objectPool);    // Release object pool not used

			// Report the failure
			unsigned char *datablockToReport = new unsigned char[datablockLength]();
			memcpy(datablockToReport, (ASTERIX_datablock + bufferIndex), datablockLength);
			failureReport->addFailure(datablockCategory, bufferIndex + bytesRead, "", datablockToReport, "Unknown Category number, Datablock skipped!",
					ReturnCodes::UNKNOWN_CATEGORY);
			return 0;
		}

		// Get description of category from 'categoryDescription_it'
		categoryDescription = categoryDescription_it->second;

		// Check if the Category is to decode
		if(!categoryDescription->isToBeDecoded()){
			bqManager->releaseObjectPoolForProducer(-1, objectPool);    // Release object pool not used
			continue;
		}

		// Get descriptions of all Items of a Category
		itemsDescription = categoryDescription->getItemCollection();
		compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();
		refItemDescription = categoryDescription->getRefItemDescription();
		spfItemDescription = categoryDescription->getSpfItemDescription();

		// Checking if the mandatory Items are present in the current parsing RECORD
		for (unsigned short mandatoryFRN_toCheck : *(categoryDescription->getMandatoryItemFRNs())) {
			if (find(FRN_list.begin(), FRN_list.end(), mandatoryFRN_toCheck) == FRN_list.end()) {
				// Log failure
				bqManager->releaseObjectPoolForProducer(-1, objectPool);

				unsigned char *entireDatablock = new unsigned char[datablockLength]();
				memcpy(entireDatablock, (ASTERIX_datablock + bufferIndex), datablockLength);
				failureReport->addFailure(datablockCategory, bufferIndex + bytesRead, itemsDescription->at(mandatoryFRN_toCheck).getName(), entireDatablock,
						"Mandatory Item for the current Category was missing, Datablock skipped!", ReturnCodes::MISSING_MANDATORY_ITEM);
				return 0;
			}
		}

		// Please mind that the first Item is at index #1, to match its index with the FRN
		for (int i = 0; i < FRN_listSize; i++) {

			FRN = FRN_list[i];

			if (FRN > itemsDescription->size()) {
				bqManager->releaseObjectPoolForProducer(-1, objectPool);

				unsigned char *entireDatablock = new unsigned char[datablockLength]();
				memcpy(entireDatablock, (ASTERIX_datablock + bufferIndex), datablockLength);
				failureReport->addFailure(datablockCategory, bufferIndex + bytesRead, "", entireDatablock, "Unknown FRN, Datablock skipped!",
						ReturnCodes::UNKNOWN_FRN);
				return 0;
			}

			// From the "ASTERIX knowledge", the Item details are loaded, ready to be accessed while parsing/decoding..
			itemToParse = &(itemsDescription->at(FRN - 1));
			itemType = itemToParse->getItemType();

			switch (itemType) {
			case ItemTypeEnum::FixedLength:
				itemLengthInByte = parseFixedLengthItem(ASTERIX_datablock, itemToParse, bufferIndex + bytesRead, itemToParse->isItemToBeDecoded(),
						objectPool);

				break;

			case ItemTypeEnum::ExtendedLength:
				itemLengthInByte = parseExtendedLengthItem(ASTERIX_datablock, itemToParse, bufferIndex + bytesRead, itemToParse->isItemToBeDecoded(),
						objectPool);
				break;

			case ItemTypeEnum::CompoundLength:
				itemLengthInByte = parseCompoundLengthItem(ASTERIX_datablock, itemToParse, bufferIndex + bytesRead, itemToParse->isItemToBeDecoded(),
						compoundSubItemsDescription, datablockCategory, objectPool);
				break;

			case ItemTypeEnum::RepetitiveLength:
				itemLengthInByte = parseRepetitiveLengthItem(ASTERIX_datablock, itemToParse, bufferIndex + bytesRead, itemToParse->isItemToBeDecoded(),
						objectPool);
				break;

			case ItemTypeEnum::ExplicitLength:
				itemLengthInByte = parseExplicitLengthItem(ASTERIX_datablock, itemToParse, bufferIndex + bytesRead, itemToParse->isItemToBeDecoded(),
						refItemDescription, compoundSubItemsDescription, spfItemDescription, datablockCategory,
						objectPool);
				break;

			default: {
				//bqManager->releaseObjectPoolForProducer(-1, objectPool);

				// Unknown Item type, failure added to the report variable
				unsigned char *datablockToReport = nullptr;
				memcpy(datablockToReport, (ASTERIX_datablock + bufferIndex), datablockLength);
				failureReport->addFailure(datablockCategory, bufferIndex + bytesRead, itemToParse->getName(), datablockToReport, "Unknown Item type!",
						ReturnCodes::PARSING_ERROR);
				itemLengthInByte = -1;    // This is going to stop the outer loop
			}
			break;
			}
			if (itemLengthInByte < 0) {
				bqManager->releaseObjectPoolForProducer(-1, objectPool);
				return 0;
			}
			bytesRead += itemLengthInByte;
		}
		recordSizeInByte = bytesRead - recordStartIndex;

		rawRecord = (unsigned char*) new unsigned char[recordSizeInByte + 1];
		memcpy(rawRecord, ASTERIX_datablock + recordStartIndex, recordSizeInByte);
		rawRecord[recordSizeInByte] = '\0';

		recordNumber++;

		objectPool->setRecordCategoryNumber(datablockCategory);
		objectPool->setRawRecord(rawRecord);
		objectPool->setRawRecordLength(recordSizeInByte);

		bqManager->setObjectPoolForConsumer(objectPool);
	}
	return recordNumber;
}

int ParsingUtils::readFspec(unsigned char *ASTERIX_datablock, int bufferIndex, std::vector<int>* FRN_list) {
	// Variables definition
	int FRN = 1;    // starting from '1' as defined on ASTERIX Syntax
	int index = bufferIndex;
	bool FX = true;				// if FX=1: there is another byte to read
	unsigned char mask = 0x80;    // mask: 1000 0000

	FRN_list->clear();
	bqManager = BlockingQueueManager::getInstance();

	while (FX) {
		while (mask != 0x01) {
			if (ASTERIX_datablock[index] & mask)
				FRN_list->push_back(FRN);

			mask = mask >> 1;                        // Shift one position to right
			FRN++;
		}
		if (!(ASTERIX_datablock[index] & mask))        // Check FX value, mask: 0x01
			FX = false;

		index++;
		mask = 0x80;
	}
	return (index - bufferIndex);    // returns the FSPEC length in bytes
}

int ParsingUtils::parseFirstTwoBytes(unsigned char *ASTERIX_record, int index) {
	//stringstream firstTwoBytesValue;

	int sac = static_cast<unsigned int>(ASTERIX_record[index]);
	int sic = static_cast<unsigned int>(ASTERIX_record[index + 1]);

	sac = sac * 1000;    // Move SAC 3 positions to the left to fit SIC in the first 3 digits
	sac += sic;

	//firstTwoBytesValue << std::dec << static_cast<unsigned int>(ASTERIX_record[index]) << static_cast<unsigned int>(ASTERIX_record[index + 1]);
	//return firstTwoBytesValue.str();
	return sac;
}

int ParsingUtils::parseFixedLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded, ParsedRecord* objectPool) {

	// Variables definition
	std::vector<std::string>* elementsName = itemToParse->getAllElementNames();
	std::vector<int>* elementsLength = itemToParse->getAllElementDescription();
	int lengthInBits = 0;
	int itemLengthInByte = 0;
	int bytePerElem = 0;
	int parsedBits = 0;
	int offset = 0;
	int elementsVectorSize = 0;
	int elementValueSizeInByte;

	itemLengthInByte = itemToParse->getLengthInByte();

	if (!isItemToBeDecoded)
		return itemLengthInByte;

	elementsVectorSize = (*elementsLength).size();

	unsigned char *elementValue;

	if (commonVariables->isDebugEnabledForDecoder())
		cout << endl << itemToParse->getName() << endl;

	bqManager = BlockingQueueManager::getInstance();

	for (int i = 0; i < elementsVectorSize; i++) {
		lengthInBits = (*elementsLength)[i];

		if ((*elementsName)[i] != kSpare) {
			bytePerElem = lengthInBits / 8;
			if (lengthInBits % 8 > 0)
				bytePerElem++;

			// If Element length (module 8) + offset is greater than 1B -> element is between 2 octects
			if ((lengthInBits % 8) + offset > 8) {
				bytePerElem++;
			}

			elementValueSizeInByte = ((size_t) bytePerElem * (sizeof *elementValue));

			elementValue = new unsigned char[elementValueSizeInByte + 1];
			memcpy(elementValue, ASTERIX_datablock + (index + (parsedBits / 8)), elementValueSizeInByte);
			elementValue[elementValueSizeInByte] = '\0';

			parseElement(lengthInBits, offset, bytePerElem, elementValue);

			// Let's save the parsed Element in the objectPool
			objectPool->setParsedElement(itemToParse, i, elementValue);

			if (commonVariables->isDebugEnabledForDecoder())
				PrintingUtils::printElement((*elementsName)[i], lengthInBits, elementValue);
		}

		// Offset is the starting point of element inside an octect, when it is equal to 8 -> it became (offset %= 8)
		offset += lengthInBits;
		offset %= 8;

		parsedBits += lengthInBits;
	}

	return itemLengthInByte;
}

int ParsingUtils::parseExtendedLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded, ParsedRecord* objectPool) {
	// Variables definition
	std::vector<std::string>* elementsName = itemToParse->getAllElementNames();
	std::vector<int>* elementsLength = itemToParse->getAllElementDescription();

	int lengthInBits = 0, lengthInByte = 0, bytePerElem = 0, bitsParsed = 0, offset = 0, sizeElementValue = 0;
	unsigned int elementsVectorSize = 0;
	unsigned char *elementValue;

	lengthInByte = ParsingUtils::calculateExtendedItemLength(ASTERIX_datablock, index, itemToParse->getLengthInByte());

	if (!isItemToBeDecoded)
		return lengthInByte;

	elementsVectorSize = elementsLength->size();

	if (commonVariables->isDebugEnabledForDecoder())
		cout << endl << itemToParse->getName() << endl;

	bqManager = BlockingQueueManager::getInstance();

	for (unsigned int i = 0; i < elementsVectorSize; i++) {
		lengthInBits = (*elementsLength)[i];

		if ((*elementsName)[i] != kFX && (*elementsName)[i] != kSpare) {
			bytePerElem = lengthInBits / 8;
			if (lengthInBits % 8 > 0) {
				bytePerElem++;
			}
			// If Element length (module 8) + offset is greater than 1B -> element is between 2 octects
			if ((lengthInBits % 8) + offset > 8) {
				bytePerElem++;
			}
			sizeElementValue = ((size_t) bytePerElem * (sizeof *elementValue));

			elementValue = (unsigned char*) new unsigned char[sizeElementValue + 1];
			memcpy(elementValue, ASTERIX_datablock + (index + (bitsParsed / 8)), sizeElementValue);
			elementValue[sizeElementValue] = '\0';

			parseElement(lengthInBits, offset, bytePerElem, elementValue);

			// Save parsed element
			objectPool->setParsedElement(itemToParse, i, elementValue);

			if (commonVariables->isDebugEnabledForDecoder())
				PrintingUtils::printElement((*elementsName)[i], lengthInBits, elementValue);
		}
		// Offset is the starting point of element inside an octect, when it is equal to 8 -> it became 0 (offset %= 8)
		offset += lengthInBits;
		offset %= 8;

		bitsParsed += lengthInBits;

		if ((bitsParsed / 8) == lengthInByte)
			break;    // It breaks out of the FOR loop
	}

	return lengthInByte;
}

int ParsingUtils::parseRepetitiveLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded, ParsedRecord* objectPool) {
	// Variables definition
	vector<string>* elementsName = itemToParse->getAllElementNames();
	vector<int>* elementsLength = itemToParse->getAllElementDescription();

	int lengthInBits = 0, moduleLength = 0, bytePerElem = 0, bitsParsed = 0, offset = 0, elementsVectorSize = 0, sizeElementValue = 0;
	unsigned short int numberOfElementsInTheModule = -1, numberOfElementsInCurrentItem = 0;
	unsigned int nRep;
	unsigned char *elementValue;

	moduleLength = itemToParse->getLengthInByte();
	elementsVectorSize = (*elementsLength).size();

	bqManager = BlockingQueueManager::getInstance();

	nRep = (unsigned int) ASTERIX_datablock[index++];

	if (!isItemToBeDecoded)
		return (nRep * moduleLength) + 1;

	if (commonVariables->isDebugEnabledForDecoder())

		cout << endl << itemToParse->getName() << endl;

	numberOfElementsInTheModule = itemToParse->getNumberOfElementsInTheModule();
	numberOfElementsInCurrentItem = nRep * numberOfElementsInTheModule;

	if (elementsVectorSize < numberOfElementsInCurrentItem) {    // resize item description vectors
		itemToParse->addRepetitionInVectors(elementsVectorSize, numberOfElementsInCurrentItem);
		elementsVectorSize = (*elementsLength).size();
	}

	for (unsigned int i = 0; i < numberOfElementsInCurrentItem; i++) {
		lengthInBits = (*elementsLength)[i];

		if ((*elementsName)[i] != kSpare) {
			bytePerElem = lengthInBits / 8;

			if (lengthInBits % 8 > 0) {
				bytePerElem++;
			}
			// If Element length (module 8) + offset is greater than 1B -> element is between 2 octects
			if ((lengthInBits % 8) + offset > 8) {
				bytePerElem++;
			}
			sizeElementValue = ((size_t) bytePerElem * (sizeof *elementValue));

			elementValue = (unsigned char*) new unsigned char[sizeElementValue + 1];
			memcpy(elementValue, ASTERIX_datablock + (index + (bitsParsed / 8)), sizeElementValue);
			elementValue[sizeElementValue] = '\0';

			parseElement(lengthInBits, offset, bytePerElem, elementValue);

			// Save parsed element
			objectPool->setParsedElement(itemToParse, i, elementValue);

			if (commonVariables->isDebugEnabledForDecoder()) {
				PrintingUtils::printElement((*elementsName)[i], lengthInBits, elementValue);
			}
		}

		// Offset is the starting point of element inside an octect, when it is equal to 8 -> it became 0 (offset %= 8)
		offset += lengthInBits;
		offset %= 8;

		bitsParsed += lengthInBits;
	}
	return (nRep * moduleLength) + 1;    // +1: byte for number of repetition
}

int ParsingUtils::parseCompoundLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded,
		std::vector<Item> *compoundSubItemsDescription, unsigned int categoryNumber, ParsedRecord* objectPool) {

	// Variables definition
	std::vector<int> itemsIndicator;
	std::vector<int>* elementsDescription = itemToParse->getAllElementDescription();

	Item* subitem;
	ItemTypeEnum subitemType;
	unsigned int bytesRead = 0, subitemIndex = 0, itemIndicator = 0;
	short int itemLength = 0;

	bool FX = true;		 			// if FX=1: there is another byte to read
	unsigned char mask = 0x80;      // mask: 1000 0000b

	bqManager = BlockingQueueManager::getInstance();

	if (commonVariables->isDebugEnabledForDecoder() && isItemToBeDecoded)
		cout << endl << itemToParse->getName() << endl;

	// Read Primary Subfield and save into 'itemsIndicator' which subItems are present
	while (FX) {
		while (mask != 0x01) {
			if (ASTERIX_datablock[index + bytesRead] & mask) {
				itemsIndicator.push_back(itemIndicator);
			}
			mask = mask >> 1;                        // Shift one position to right
			itemIndicator++;
		}
		if (!(ASTERIX_datablock[index + bytesRead] & mask)) {      // Check FX value, mask: 0x01
			FX = false;
		}
		bytesRead++;
		mask = 0x80;
	}

	for (unsigned int j = 0; j < itemsIndicator.size(); j++) {
		// Get index from 'elementsDescription' for get the subItem from 'compoundSubItemsDescription'
		subitemIndex = (*elementsDescription)[itemsIndicator[j]];

		if (subitemIndex >= compoundSubItemsDescription->size()) {
			return -1;
		}

		subitem = &(*compoundSubItemsDescription)[subitemIndex];
		subitemType = subitem->getItemType();

		switch (subitemType) {
		case ItemTypeEnum::FixedLength:
			itemLength = parseFixedLengthItem(ASTERIX_datablock, subitem, index + bytesRead, isItemToBeDecoded, objectPool);

			break;

		case ItemTypeEnum::ExtendedLength:
			itemLength = parseExtendedLengthItem(ASTERIX_datablock, subitem, index + bytesRead, isItemToBeDecoded, objectPool);

			break;

		case ItemTypeEnum::RepetitiveLength:
			itemLength = parseRepetitiveLengthItem(ASTERIX_datablock, subitem, index + bytesRead, isItemToBeDecoded, objectPool);
			break;

		default: {
			// Unknown Item type, failure added to the report variable
			failureReport->addFailure(categoryNumber, index + bytesRead, subitem->getName(), nullptr, "Unknown Compound subitem type!",
					ReturnCodes::PARSING_ERROR);
			itemLength = -1;    // This will stop the outer loop
		}
		break;
		}
		if (itemLength < 0) {
			return -1;
		}
		bytesRead += itemLength;
	}

	return bytesRead;
}

int ParsingUtils::parseExplicitLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded,
		std::vector<Item>* refItemDescription, std::vector<Item>* subItemsDescription, std::vector<Item>* spfItemDescription, unsigned int categoryNumber,
		ParsedRecord* objectPool) {

	bqManager = BlockingQueueManager::getInstance();

	if (itemToParse->getName() == kREF) {

		// Get size of REF Item
		int itemLength = ASTERIX_datablock[index];

		if (facadeCommonVariables->isRefDecodingEnabled() && refItemDescription->size() > 0) {
			// Variables definition
			std::vector<int> itemsIndicator;
			std::vector<int>* elementsDescription = itemToParse->getAllElementDescription();

			int counter = 0, itemIndex = 0, bytesRead = 1;

			unsigned char mask = 0x80;      // mask: 1000 0000b
			ItemTypeEnum REF_itemType;
			Item* REF_item;

			if (commonVariables->isDebugEnabledForDecoder())
				cout << endl << itemToParse->getName() << endl;

			bqManager = BlockingQueueManager::getInstance();

			// Reading the first Byte, that acts as Item Specification [FSPEC], telling which
			while (mask != 0x00) {    // until the whole FSPEC has not been parsed bit-by-bit..
				if (ASTERIX_datablock[index + bytesRead] & mask) {
					itemsIndicator.push_back(counter);
				}
				mask >>= 1;    // Shift the mask one bit to the right, to analyze the next right (lower) bit..
				counter++;
			}
			bytesRead++;	// updates the 'bytesRead' counter..

			// Start parsing REF items
			for (unsigned int i = 0; i < itemsIndicator.size(); i++) {
				// Get index from 'elementsDescription' for get the subItem from 'reItemDescription'
				itemIndex = (*elementsDescription)[itemsIndicator[i]];
				REF_item = &(*refItemDescription)[itemIndex];
				REF_itemType = REF_item->getItemType();

				switch (REF_itemType) {
				case ItemTypeEnum::FixedLength:
					bytesRead += parseFixedLengthItem(ASTERIX_datablock, REF_item, index + bytesRead, REF_item->isItemToBeDecoded(), objectPool);

					break;

				case ItemTypeEnum::ExtendedLength:
					bytesRead += parseExtendedLengthItem(ASTERIX_datablock, REF_item, index + bytesRead, REF_item->isItemToBeDecoded(), objectPool);

					break;

				case ItemTypeEnum::RepetitiveLength:
					bytesRead += parseRepetitiveLengthItem(ASTERIX_datablock, REF_item, index + bytesRead, REF_item->isItemToBeDecoded(), objectPool);
					break;

				case ItemTypeEnum::CompoundLength:
					bytesRead += parseCompoundLengthItem(ASTERIX_datablock, REF_item, index + bytesRead, REF_item->isItemToBeDecoded(), subItemsDescription,
							categoryNumber, objectPool);
					break;

				case ItemTypeEnum::ExplicitLength:
					bytesRead += parseExplicitLengthItem(ASTERIX_datablock, REF_item, index + bytesRead, REF_item->isItemToBeDecoded(), refItemDescription,
							subItemsDescription, spfItemDescription, categoryNumber, objectPool);

					break;
				default:
					// Unknown Item type, failure added to the report variable
					failureReport->addFailure(categoryNumber, index + bytesRead, REF_item->getName(), nullptr, "Unknown REF Item type!",
							ReturnCodes::PARSING_ERROR);
					itemLength = -1;    // Out condition that stops the outer loop
					break;
				}
				if (itemLength < 0) {
					return -1;
				}
			}
		}
		return itemLength;

	} else if (itemToParse->getName() == kSPF) {
		// Get size of SPF Item
		int itemLength = 0, itemIndex = -1;
		itemLength = ASTERIX_datablock[index];
		int bytesRead = 1;    // 'itemLength' (1Byte) has just been read
		Item *SPF_item;
		ItemTypeEnum SPF_itemType;

		if (facadeCommonVariables->isSpfDecodingEnabled() && spfItemDescription->size() > 0) {
			std::vector<int>* elementsDescription = itemToParse->getAllElementDescription();

			unsigned int elementsDescriptionSize = elementsDescription->size();

			// ..parsing..
			for (unsigned int i = 0; i < elementsDescriptionSize; i++) {
				itemIndex = (*elementsDescription)[i];
				SPF_item = &(*spfItemDescription)[itemIndex];
				SPF_itemType = SPF_item->getItemType();

				switch (SPF_itemType) {
				case ItemTypeEnum::FixedLength:
					bytesRead += parseFixedLengthItem(ASTERIX_datablock, SPF_item, index + bytesRead, SPF_item->isItemToBeDecoded(), objectPool);
					break;
				case ItemTypeEnum::ExtendedLength:
					bytesRead += parseExtendedLengthItem(ASTERIX_datablock, SPF_item, index + bytesRead, SPF_item->isItemToBeDecoded(), objectPool);
					break;
				case ItemTypeEnum::CompoundLength:
					bytesRead += parseCompoundLengthItem(ASTERIX_datablock, SPF_item, index + bytesRead, SPF_item->isItemToBeDecoded(), subItemsDescription,
							categoryNumber, objectPool);
					break;
				case ItemTypeEnum::RepetitiveLength:
					bytesRead += parseRepetitiveLengthItem(ASTERIX_datablock, SPF_item, index + bytesRead, SPF_item->isItemToBeDecoded(), objectPool);
					break;
				case ItemTypeEnum::ExplicitLength:
					bytesRead += parseExplicitLengthItem(ASTERIX_datablock, SPF_item, index + bytesRead, SPF_item->isItemToBeDecoded(), refItemDescription,
							subItemsDescription, spfItemDescription, categoryNumber, objectPool);
					break;
				default:
					// Unknown Item type, failure added to the report variable
					failureReport->addFailure(categoryNumber, index + bytesRead, SPF_item->getName(), nullptr, "Unknown SPF Item type!",
							ReturnCodes::PARSING_ERROR);
					itemLength = -1;
					return itemLength;
				}
			}
		}
		// ..if SPF parsing disabled, it returns just the 'itemLength', to let the Item being skipped [mind that here its content is DROPPED!]
		return itemLength;
	}

	// Unknown Explicit Length Item type, failure added to the report variable
	failureReport->addFailure(categoryNumber, index, itemToParse->getName(), nullptr, "Unknown Explicit Length Item - only REF and SPF are permitted.",
			ReturnCodes::PARSING_ERROR);
	return -1;
}

int ParsingUtils::calculateExtendedItemLength(unsigned char *ASTERIX_datablock, int index, int primarySubfieldLength) {
	int lengthToReturn = primarySubfieldLength;    // At least..

	unsigned char mask = 0x01;    // mask: 0000 0001b
	while ((ASTERIX_datablock[index + lengthToReturn - 1] & mask))    // Check FX value, mask: 0x01
		lengthToReturn++;

	return lengthToReturn;
}
