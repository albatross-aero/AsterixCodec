#include "utils/ItemEncodingUtils.h"

FailureReport *ItemEncodingUtils::encoderFailureReport = FailureReport::getInstance();
CommonVariables *ItemEncodingUtils::commonVariables = CommonVariables::getInstance();

int ItemEncodingUtils::getIndexOfDecodedValueFromName(vector<string *> *decodedElementNames, string elementName) {
    vector<string *>::iterator it;
    vector<string *>::iterator it_begin = decodedElementNames->begin();

    for (it = it_begin; it != decodedElementNames->end(); it++) {
        if (*(*it) == elementName) {
            return it - it_begin;
        }
    }
    return -1;
}

void ItemEncodingUtils::removeDecodedElementNameAndValueFromCollections(unsigned int indexOfElement, vector<string *> *decodedElementNames,
                                                                        vector<ElementValue *> *decodedElementValues) {
    decodedElementNames->erase(decodedElementNames->begin() + indexOfElement);
    decodedElementValues->erase(decodedElementValues->begin() + indexOfElement);
}

unsigned int ItemEncodingUtils::calculateExtendedItemLength(vector<string *> *decodedElementNames, Item *itemToEncode, unsigned int categoryNumber) {
    // Information from the item description
    vector<string> *itemElementNames = itemToEncode->getAllElementNames();
    int numElements = itemElementNames->size();
    vector<int> *elementsSizes = itemToEncode->getAllElementDescription();

    // Local variable needed to iterate over elements
    int currentNumberOfBitsNeeded = 0;                    // Counter to measure the size of the Item in bits
    int finalCounterBitsNeeded = 0;                        // Counter to store the real size based on the presence of Elements
    string elementToSearch;

    unsigned short int numberOfElementsFound = 0;            // Store number of Elements found for current Item
    unsigned short int numberOfElementsNOTFound = 0;        // Store number of Elements NOT found for current Item

    // Iterate over the Elements into the Item Description
    for (int i = 0; i < numElements; i++) {
        elementToSearch = itemElementNames->at(i);

        if (elementToSearch.compare(kFX) == 0) {
            currentNumberOfBitsNeeded++;    // Sum the size needed to set the FX

            // If numberOfElementsNOTFound > 0:
            // 		1) if numberOfElementsFound == 0 -> no Elements have been found -> break the loop without error and return the Item length
            // 		2) if numberOfElementsFound > 0 -> some Elements have been found -> break the loop and exit with error
            // If numberOfElementsNOTFound == 0 -> Elements have been found ->
            // 		|-> current number of bits is stored in 'finalCounterBitsNeeded' in order to not compromise the result of this function during the loop for the next Elements
            if (numberOfElementsNOTFound > 0) {
                if (numberOfElementsFound == 0) {
                    // No Elements in this Extension have been found -> break the loop without error
                    break;

                } else {
                    // Some Elements have been found -> break the loop and exit with error
                    break;
                }
            } else {
                finalCounterBitsNeeded = currentNumberOfBitsNeeded;
            }

            //Initialize variables, new extension -> new story
            numberOfElementsFound = 0;
            numberOfElementsNOTFound = 0;

        } else if (elementToSearch.compare(kSpare) == 0) {
            currentNumberOfBitsNeeded += elementsSizes->at(i);    // Size needed to fits the Spare bits

        } else {
            // Search if 'elementToSearch' exists into the decodedElementName vector
            if (getIndexOfDecodedValueFromName(decodedElementNames, elementToSearch) != -1) {
                currentNumberOfBitsNeeded += elementsSizes->at(i);

                numberOfElementsFound++;

            } else {
                numberOfElementsNOTFound++;
            }
        }
    }

    // If numberOfElementsNOTFound > 0 && numberOfElementsFound == 0 -> Return the Item length, finalCounterBitsNeeded/8 -> 'finalCounterBitsNeeded' can be:
    //		1) 0: if the Item is no present -> NO log error
    // 		2) >0: if in the previous extension of the Item we found all the Items
    if (numberOfElementsNOTFound > 0 && numberOfElementsFound > 0) {
        string message = "One or more Elements for the Item " + itemToEncode->getName() + " were not found in the input data, Item was skipped!";
        encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr, message, ReturnCodes::ENCODING_ERROR);
        return 0;
    }

    return (finalCounterBitsNeeded /
            8);    // 'finalCounterBitsNeeded' MUST be exactly divisible by 8, because an Extended Length Item is built byte-by-byte
}

vector<unsigned char> *ItemEncodingUtils::encodeFixedLengthItem(vector<string *> *decodedElementNames, vector<ElementValue *> *decodedElementValues,
                                                                Item *itemToEncode, unsigned int categoryNumber) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    // Element information
    ElementValue *encodingElementValue = nullptr;
    int encodingElementValueIndex = -1;
    FunctionNameEnum functionNameUsedForDecoding;
    double multiplicationFactorUsedForDecoding = 1.0;
    int encodingElementLengthInBit = 0, encodingElementLengthInByte = 0;
    string currentElementName;
    Range *currentValidRange;

    // Current Item Elements
    vector<string> *elementNames = itemToEncode->getAllElementNames();
    vector<int> *elementLengthsInBit = itemToEncode->getAllElementDescription();
    vector<double> *lsbValuesOfCurrentElement = nullptr;
    unsigned int numberOfItemElements = elementNames->size();

    unsigned short int indexInLsbValues = 0;
    unsigned short int numberOfElementsFound = 0;            // Store number of Elements found for current Item during the encoding
    unsigned short int numberOfElementsNOTFound = 0;        // Store number of Elements NOT found for current Item during the encoding

    // Encoded values stored here
    unsigned char *encodedElement = nullptr;
    vector<unsigned char> *encodedItemBytestream = new vector<unsigned char>();

    // Temporary variables to save the exceed bits before composing an entire byte
    unsigned char remainingByte = 0x0;    // Byte with some bits not used
    int remainingBitCounter = 0;        // Bit not used of the 'remainingByte'
    int realBytesToBeWritten = 0;        // Number of bytes to be written in the vector
    int bytesWithInformation = 0;        // Real number of bytes filled of information
    int unusedRealBits = 0;

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << itemToEncode->getName() << endl;
    }

    /******************
     * START ENCODING *
     ******************/
    // Scanning each Element of current Item description
    for (unsigned int elementIndex = 0; elementIndex < numberOfItemElements; elementIndex++) {
        currentElementName = elementNames->at(elementIndex);

        // Get the i-th Element LENGTH in BIT
        encodingElementLengthInBit = elementLengthsInBit->at(elementIndex);
        // Calculate the i-th Element LENGTH in BYTE
        encodingElementLengthInByte =
                (encodingElementLengthInBit / 8) + 1;    // +1 | an extra byte is required to manage the Element encoding phase - it will be scaled
        if (encodingElementLengthInBit % 8 > 0) {
            encodingElementLengthInByte++;
        }

        //  There is no need to test the case [elementName == "FX"], because in FixedLength Item it is a value not allowed.
        if (currentElementName != kSpare) {
            // Get the INDEX of the i-th Element value
            encodingElementValueIndex = getIndexOfDecodedValueFromName(decodedElementNames, currentElementName);

            // In a FixedLength Item all elements MUST be present - if 'encodingElementValueIndex' == -1 &&:
            // 1) numberOfElementsFound > 0 ==> at least one Element has been found => ERROR: the  current Element of ITEM is missing
            // 2) numberOfElementsFound == 0 ==> count number of Element not found and continue -> looking for next Element -> At the end, if no Elements have been found -> Item no present (NO error)
            if (encodingElementValueIndex == -1) {
                if (numberOfElementsFound > 0) {
                    string message =
                            "One or more Elements for the Item " + itemToEncode->getName() + " were not found in the input data, Item was skipped!";
                    encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr, message, ReturnCodes::ENCODING_ERROR);
                    return nullptr;
                } else {
                    numberOfElementsNOTFound++;
                    continue;
                }
            }

            // Get the VALUE of i-th Element
            encodingElementValue = decodedElementValues->at(encodingElementValueIndex);
            if (encodingElementValue == nullptr) {
                cerr << "Internal error, 'encodingElementValueIndex' is != -1 but the 'encodingElementValue' of " << currentElementName
                     << " is not found in the input data!";
                return nullptr;
            }

            currentValidRange = itemToEncode->getValidRangeAtIndex(elementIndex);
            functionNameUsedForDecoding = itemToEncode->getFunctionNameInEnumForDecodingAtIndex(elementIndex);
            lsbValuesOfCurrentElement = itemToEncode->getLsbValuesAtIndex(elementIndex);

            if (lsbValuesOfCurrentElement->size() == 1)
                indexInLsbValues = 0;

            multiplicationFactorUsedForDecoding = lsbValuesOfCurrentElement->at(indexInLsbValues);

            switch (functionNameUsedForDecoding) {
                case FunctionNameEnum::toDecimal:
                case FunctionNameEnum::toOctal:
                case FunctionNameEnum::toHexadecimal:
                    encodedElement = ElementEncodingUtils::encodeDecimal(encodingElementValue, currentValidRange, encodingElementLengthInBit,
                                                                         multiplicationFactorUsedForDecoding, &currentElementName, categoryNumber);
                    // Use decoded value of current Element as a index of 'lsbValuesOfCurrentElement' in case of multiple lsb values
                    indexInLsbValues = ((UnsignedInteger8bitValue *) encodingElementValue)->getDecodedValue();

                    break;
                case FunctionNameEnum::CA2toDecimal:
                case FunctionNameEnum::CA2toHexadecimal:
                    encodedElement = ElementEncodingUtils::encodeDecimalToCA2(encodingElementValue, currentValidRange, encodingElementLengthInBit,
                                                                              multiplicationFactorUsedForDecoding, &currentElementName,
                                                                              categoryNumber);
                    break;
                case FunctionNameEnum::toASCII:
                    encodedElement = ElementEncodingUtils::encodeAscii(encodingElementValue, encodingElementLengthInBit);
                    break;
                case FunctionNameEnum::toTargetId:
                    encodedElement = ElementEncodingUtils::encodeTargetId(encodingElementValue, encodingElementLengthInBit);
                    break;
                case FunctionNameEnum::toMBData:
                    encodedElement = ElementEncodingUtils::encodeMBData(encodingElementValue, encodingElementLengthInBit);
                    break;
                default:
                    encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr,
                                                     "An unknown encoding function has been associated with '" + currentElementName +
                                                     "' Element to encode!",
                                                     ReturnCodes::UNKNOWN_DECODING_FUNCTION);
                    break;
            }

            if (encodedElement == nullptr) {
                string message = "No raw value has been returned by the encoding function for the '" + currentElementName + "' Element!";
                encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr, message,
                                                 ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
                return nullptr;
            }

            // The current Element has been consumed, so we are ready to remove it from the decoded Elements collection
            removeDecodedElementNameAndValueFromCollections(encodingElementValueIndex, decodedElementNames, decodedElementValues);

            numberOfElementsFound++;    //Element has been found -> if another Element will be not found -> ERROR

        } else {
            encodedElement = new unsigned char[encodingElementLengthInByte];    // TODO: improve in order not to allocate for a Spare Element
            memset(encodedElement, 0, encodingElementLengthInByte);
        }

        /****************************************************
         * How to manage the Elements NOT aligned to a Byte *
         ****************************************************/
        realBytesToBeWritten = 0;
        bytesWithInformation = 0;

        // Check if there is a remainder from the previous Element
        if (remainingBitCounter > 0) {
            // shiftRight the actual Element and bitwise-OR with the previous remainder
            BitUtils::shiftRight(encodedElement, encodingElementLengthInByte, remainingBitCounter);
            encodedElement[0] |= remainingByte;
        }

        // Check if the shiftRight operation has moved the 'encodedElement' in the additional Byte at the end of the Element (lowest part)
        unusedRealBits = ((encodingElementLengthInByte - 1) * 8) - encodingElementLengthInBit;
        if (unusedRealBits >= remainingBitCounter) {
            // Shift fits in [encodingElementLengthInByte - 1] Bytes
            bytesWithInformation = encodingElementLengthInByte - 1;
            encodingElementLengthInBit += remainingBitCounter;    // Adds the shift to the length in bit of the current Element
        } else {
            bytesWithInformation = encodingElementLengthInByte;    // Keeps the entire length of the Element (it includes also the additional 0x0 byte at the end)
            encodingElementLengthInBit += remainingBitCounter;    // Adds the shift to the length in bit of the current Element
        }

        // Writing in the vector if some bytes are completely filled
        realBytesToBeWritten = encodingElementLengthInBit / 8;
        for (int j = 0; j < realBytesToBeWritten; j++) {
            encodedItemBytestream->push_back(encodedElement[j]);
            remainingBitCounter = 0;
        }

        // The actual Element length in bit is not a multiple of byte
        if (encodingElementLengthInBit % 8 > 0) {
            remainingByte = encodedElement[bytesWithInformation - 1];                        // Keep last byte of the element for the next loop
            remainingBitCounter = (bytesWithInformation * 8) - encodingElementLengthInBit;    // For sure the result is between 0 and 8
            remainingBitCounter = 8 - remainingBitCounter;    // Save the complement -> number of remaining free bits into 'remainingByte'
        }

        delete[] encodedElement;
        encodedElement = nullptr;
    }

    // No Elements have been found -> Item no present
    if (numberOfElementsFound == 0)
        return nullptr;

    // One ore more Elements have been found and one ore more Elements have NOT been found -> ERROR
    if (numberOfElementsNOTFound > 0) {
        string message =
                "One or more Elements for the current Item " + itemToEncode->getName() + " were not found in the input data, Item was skipped!";
        encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr, message, ReturnCodes::ENCODING_ERROR);
        return nullptr;
    }

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << "\t Item raw data: ";
        for (unsigned int i = 0; i < encodedItemBytestream->size(); i++) {
            cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(encodedItemBytestream->at(i)) << " ";
        }
        cout << endl;
        cout << "\t Item Length: " << encodedItemBytestream->size() << endl;
    }
    return encodedItemBytestream;
}

vector<unsigned char> *
ItemEncodingUtils::encodeExtendedLengthItem(vector<string *> *decodedElementNames, vector<ElementValue *> *decodedElementValues,
                                            Item *itemToEncode, unsigned int categoryNumber) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    // Element information
    ElementValue *encodingElementValue = nullptr;
    int encodingElementValueIndex = -1;
    double multiplicationFactorUsedForDecoding = 0.0;
    int encodingElementLengthInBit = 0, encodingElementLengthInByte = 0;
    string currentElementName;
    Range *currentValidRange;
    FunctionNameEnum functionNameUsedForDecoding = FunctionNameEnum::InvalidValue;

    // Current Item Elements
    vector<string> *elementNames = itemToEncode->getAllElementNames();
    vector<int> *elementLengthsInBit = itemToEncode->getAllElementDescription();
    vector<double> *lsbValuesOfCurrentElement = nullptr;
    unsigned int numberOfElementsInItem = elementNames->size();
    unsigned int itemBitsStillToEncode = 0;

    unsigned int indexInLsbValues = 0;

    // Encoded values
    unsigned char *encodedElement = nullptr;
    vector<unsigned char> *encodedItemBytestream = new vector<unsigned char>();

    // Temporary variables to save the exceed bits before composing an entire byte
    unsigned char remainingByte = 0x0;    // Byte with some bits not used
    int remainingBitCounter = 0;    // Bit not used of the 'remainingByte'
    int realBytesToBeWritten = 0;    // Number of bytes to be written into the vector
    int bytesWithInformation = 0;    // Real number of bytes filled of information

    // Calculate the length in byte of the Item checking how many extensions there are in the decoded elements
    int itemLengthInByte = calculateExtendedItemLength(decodedElementNames, itemToEncode, categoryNumber);

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << itemToEncode->getName() << endl;
    }

    if (itemLengthInByte == 0) {
        //Error already logged by 'calculateExtendedItemLength' method
        return nullptr;
    }
    itemBitsStillToEncode = itemLengthInByte * 8;


    /******************
     * START ENCODING *
     ******************/

    // Scan each element of the current item description
    for (unsigned int elementIndex = 0; elementIndex < numberOfElementsInItem && itemBitsStillToEncode > 0; elementIndex++) {

        // Get information of the current element
        currentElementName = elementNames->at(elementIndex);

        encodingElementLengthInBit = elementLengthsInBit->at(elementIndex);
        lsbValuesOfCurrentElement = itemToEncode->getLsbValuesAtIndex(elementIndex);

        if (lsbValuesOfCurrentElement->size() == 1)
            indexInLsbValues = 0;

        functionNameUsedForDecoding = itemToEncode->getFunctionNameInEnumForDecodingAtIndex(elementIndex);
        multiplicationFactorUsedForDecoding = lsbValuesOfCurrentElement->at(indexInLsbValues);
        currentValidRange = itemToEncode->getValidRangeAtIndex(elementIndex);

        itemBitsStillToEncode -= encodingElementLengthInBit;    // Update the actual remaining size in bits still to encode

        // Update the LENGTH in BYTE of the element
        encodingElementLengthInByte =
                (encodingElementLengthInBit / 8) + 1;    // +1 just because the encoding phase of an element allocates one byte more the normal size
        if (encodingElementLengthInBit % 8 > 0)
            encodingElementLengthInByte++;

        if (currentElementName != kSpare && currentElementName != kFX) {
            // Get the index of the i-th Element VALUE
            encodingElementValueIndex = getIndexOfDecodedValueFromName(decodedElementNames, currentElementName);

            // All Elements MUST be present - if 'encodingElementValueIndex' is -1 -> error
            if (encodingElementValueIndex == -1) {
                cerr
                        << "Internal error, this point should be not reachable if 'calculateExtendedItemLength' calculate Item Length equal to 0. Error for Item "
                        << currentElementName << ".";
                return nullptr;
            }

            // Get the i-th Element VALUE
            encodingElementValue = decodedElementValues->at(encodingElementValueIndex);

            if (encodingElementValue == nullptr) {
                cerr << "Internal error, 'encodingElementValueIndex' is != -1 but the 'encodingElementValue' of " << currentElementName
                     << " is not found in the input data!";
                return nullptr;
            }

            switch (functionNameUsedForDecoding) {
                case FunctionNameEnum::toDecimal:
                case FunctionNameEnum::toOctal:
                case FunctionNameEnum::toHexadecimal:
                    encodedElement = ElementEncodingUtils::encodeDecimal(encodingElementValue, currentValidRange, encodingElementLengthInBit,
                                                                         multiplicationFactorUsedForDecoding, &currentElementName, categoryNumber);
                    // Use decoded value of current Element as a index of 'lsbValuesOfCurrentElement' in case of multiple lsb values
                    indexInLsbValues = ((UnsignedInteger8bitValue *) encodingElementValue)->getDecodedValue();

                    break;
                case FunctionNameEnum::CA2toDecimal:
                case FunctionNameEnum::CA2toHexadecimal:
                    encodedElement = ElementEncodingUtils::encodeDecimalToCA2(encodingElementValue, currentValidRange, encodingElementLengthInBit,
                                                                              multiplicationFactorUsedForDecoding, &currentElementName,
                                                                              categoryNumber);
                    break;
                case FunctionNameEnum::toASCII:
                    encodedElement = ElementEncodingUtils::encodeAscii(encodingElementValue, encodingElementLengthInBit);
                    break;

                case FunctionNameEnum::toTargetId:
                    encodedElement = ElementEncodingUtils::encodeTargetId(encodingElementValue, encodingElementLengthInBit);
                    break;

                case FunctionNameEnum::toMBData:
                    encodedElement = ElementEncodingUtils::encodeMBData(encodingElementValue, encodingElementLengthInBit);
                    break;

                default:
                    encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr,
                                                     "An unknown encoding function has been associated with '" + currentElementName +
                                                     "' Element to encode!",
                                                     ReturnCodes::UNKNOWN_DECODING_FUNCTION);
                    break;
            }

            if (encodedElement == nullptr) {
                string message = "No raw value has been returned by the encoding function for the '" + currentElementName + "' Element!";
                encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr, message,
                                                 ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
                return nullptr;
            }

            // The current Element has been consumed, so we are ready to remove it from the decoded Elements collection
            removeDecodedElementNameAndValueFromCollections(encodingElementValueIndex, decodedElementNames, decodedElementValues);

        } else if (currentElementName == kSpare) {
            encodedElement = new unsigned char[encodingElementLengthInByte];    // TODO: improve to do not allocate for a Spare element
            memset(encodedElement, 0, encodingElementLengthInByte);

        } else if (currentElementName == kFX) {
            encodedElement = new unsigned char[encodingElementLengthInByte];

            if (elementIndex == (numberOfElementsInItem - 1) || itemBitsStillToEncode == 0) {
                // The last Element of the current Item has been reached ==> Set FX bit = 0
                encodedElement[0] = 0x0;
            } else {
                // Set FX bit = 1
                encodedElement[0] = 0x80;
            }
        }

        /****************************************************
         * How to manage the Elements NOT aligned to a Byte *
         ****************************************************/
        realBytesToBeWritten = 0;
        bytesWithInformation = 0;

        // Checking if there is a remainder from the previous Element
        if (remainingBitCounter > 0) {
            // Shift right the actual element and bitwise-or with the previous remainder
            BitUtils::shiftRight(encodedElement, encodingElementLengthInByte, remainingBitCounter);
            encodedElement[0] |= remainingByte;
        }

        // Checking if the shift right operation move 'encodedElement' into the additional byte at the end of the element
        int unusedRealBits = ((encodingElementLengthInByte - 1) * 8) - encodingElementLengthInBit;
        if (unusedRealBits >= remainingBitCounter) {
            // Shift fits into [encodingElementLengthInByte - 1] Bytes
            bytesWithInformation = encodingElementLengthInByte - 1;
            encodingElementLengthInBit += remainingBitCounter;    // Adds the shift to the length in bit of the current element
        } else {
            bytesWithInformation = encodingElementLengthInByte;    // Keeps the entire length of the element (it includes also the additional 0x0 byte at the end)
            encodingElementLengthInBit += remainingBitCounter;    // Adds the shift to the length in bit of the current element
        }

        // Writing in the vector if some bytes are completely filled
        realBytesToBeWritten = encodingElementLengthInBit / 8;
        for (int j = 0; j < realBytesToBeWritten; j++) {
            encodedItemBytestream->push_back(encodedElement[j]);
            remainingBitCounter = 0;
        }

        // The actual element length in bit is not a multiple of byte
        if (encodingElementLengthInBit % 8 > 0) {
            remainingByte = encodedElement[bytesWithInformation - 1];    // Keep last byte of the element for the next loop
            remainingBitCounter = (bytesWithInformation * 8) - encodingElementLengthInBit;    // For sure the result is between 0 and 8
            remainingBitCounter = 8 - remainingBitCounter;    // Save the complement -> remaining number of free bits into 'remainingByte'
        }

        delete[] encodedElement;
        encodedElement = nullptr;
    }

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << "\t Item raw data: ";
        for (unsigned int i = 0; i < encodedItemBytestream->size(); i++) {
            cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(encodedItemBytestream->at(i)) << " ";
        }
        cout << endl;
        cout << "\t Item Length: " << std::dec << encodedItemBytestream->size() << endl;
    }

    return encodedItemBytestream;
}

vector<unsigned char> *
ItemEncodingUtils::encodeRepetitiveLengthItem(vector<string *> *decodedElementNames, vector<ElementValue *> *decodedElementValues,
                                              Item *itemToEncode, unsigned int categoryNumber) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    // Element information
    ElementValue *encodingElementValue = nullptr;
    int encodingElementValueIndex = -1;
    FunctionNameEnum functionNameForDecoding;
    int encodingElementLengthInBit = 0, encodingElementLengthInByte = 0;
    double multiplicationFactorUsedForDecoding = 1.0;
    string currentElementName;
    Range *currentValidRange;
    string suffixElementName;    // To be appended at the element name if the number of repetitions is grated than 1

    // Elements of current Item
    vector<string> *elementNames = itemToEncode->getAllElementNames();
    vector<int> *elementLengthsInBit = itemToEncode->getAllElementDescription();
    vector<double> *lsbValuesOfCurrentElement = nullptr;

    unsigned short int indexInLsbValues = 0;
    unsigned short int numberOfElementsFound = 0;            // Store number of Elements found for current Item during the encoding
    unsigned short int numberOfElementsNOTFound = 0;        // Store number of Elements NOT found for current Item during the encoding

    // Char* to save the encoded values
    unsigned char *encodedElement = nullptr;
    vector<unsigned char> *encodedItemBytestream = new vector<unsigned char>();

    // Temporary variables to save the exceed bits before composing an entire byte
    unsigned char remainingByte = 0x0;    // Byte with some bits not used
    int remainingBitCounter = 0;    // Bit not used of the 'remainingByte'
    int realBytesToBeWritten = 0;    // Number of bytes to be written into the vector
    int bytesWithInformation = 0;    // Real number of bytes filled of information

    // Numbers needed to calculate the size of the current Item
    unsigned int currentNumberOfRepetition = 0;
    unsigned int numberOfBaseElement = itemToEncode->getNumberOfElementsInTheModule();    // Index to loop into the element of a single repetition
    unsigned int numberOfRepetionInItemDescription = elementLengthsInBit->size() / numberOfBaseElement;
    unsigned int indexInItemDescription = 0;    // Index to loop into the total list of element of the item (multiple repetitions)

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << itemToEncode->getName() << endl;
    }

    /******************
     * START ENCODING *
     ******************/
    do {
        // Scan each element of the current Item Description
        for (unsigned int elementIndex = 0; elementIndex < numberOfBaseElement; elementIndex++, indexInItemDescription++) {

            // Get the elementName
            if (numberOfRepetionInItemDescription > currentNumberOfRepetition) {    // element name is present in the element description
                currentElementName = elementNames->at(indexInItemDescription);

            } else {    // Create the name: elementName base + "_"+#rep
                if (elementIndex == 0) {
                    indexInItemDescription = 0;
                    suffixElementName = string("_" + to_string(currentNumberOfRepetition));
                }
                currentElementName = elementNames->at(indexInItemDescription);
                if (currentNumberOfRepetition != 0)
                    currentElementName.append(suffixElementName);
            }

            // Get the LENGTH (in bit) of the i-th Element
            encodingElementLengthInBit = itemToEncode->getAllElementDescription()->at(elementIndex);
            // Update the LENGTH in BYTE of the element
            encodingElementLengthInByte = (encodingElementLengthInBit / 8) +
                                          1;    // +1 just because the encoding phase of an element allocates one byte more the normal size
            if (encodingElementLengthInBit % 8 > 0)
                encodingElementLengthInByte++;

            if (currentElementName != kSpare) {    //  no test elementName == "FX" -> in Repetitive item is not an allowed value
                // Get the index of the VALUE of i-th Element
                encodingElementValueIndex = getIndexOfDecodedValueFromName(decodedElementNames, currentElementName);

                // In a RepetitiveLengthItem all elements MUST be present - if 'encodingElementValueIndex' == -1 increment the counter 'numberOfElementsNOTFound' end, if no Elements have been found -> Item no present (NO error)
                if (encodingElementValueIndex == -1) {
                    numberOfElementsNOTFound++;
                    continue;
                }

                // Get the VALUE of i-th Element
                encodingElementValue = decodedElementValues->at(encodingElementValueIndex);

                if (encodingElementValue == nullptr) {
                    cerr << "Internal error, 'encodingElementValueIndex' is != -1 but the 'encodingElementValue' of " << currentElementName
                         << " is not found in the input data!";
                    return nullptr;
                }

                functionNameForDecoding = itemToEncode->getFunctionNameInEnumForDecodingAtIndex(elementIndex);
                lsbValuesOfCurrentElement = itemToEncode->getLsbValuesAtIndex(elementIndex);
                currentValidRange = itemToEncode->getValidRangeAtIndex(elementIndex);

                if (lsbValuesOfCurrentElement->size() == 1)
                    indexInLsbValues = 0;

                multiplicationFactorUsedForDecoding = lsbValuesOfCurrentElement->at(indexInLsbValues);

                switch (functionNameForDecoding) {
                    case FunctionNameEnum::toDecimal:
                    case FunctionNameEnum::toOctal:
                    case FunctionNameEnum::toHexadecimal:
                        encodedElement = ElementEncodingUtils::encodeDecimal(encodingElementValue, currentValidRange, encodingElementLengthInBit,
                                                                             multiplicationFactorUsedForDecoding, &currentElementName,
                                                                             categoryNumber);
                        // Use decoded value of current Element as a index of 'lsbValuesOfCurrentElement' in case of multiple lsb values
                        indexInLsbValues = ((UnsignedInteger8bitValue *) encodingElementValue)->getDecodedValue();

                        break;
                    case FunctionNameEnum::CA2toDecimal:
                    case FunctionNameEnum::CA2toHexadecimal:
                        encodedElement = ElementEncodingUtils::encodeDecimalToCA2(encodingElementValue, currentValidRange, encodingElementLengthInBit,
                                                                                  multiplicationFactorUsedForDecoding, &currentElementName,
                                                                                  categoryNumber);
                        break;
                    case FunctionNameEnum::toASCII:
                        encodedElement = ElementEncodingUtils::encodeAscii(encodingElementValue, encodingElementLengthInBit);
                        break;
                    case FunctionNameEnum::toTargetId:
                        encodedElement = ElementEncodingUtils::encodeTargetId(encodingElementValue, encodingElementLengthInBit);
                        break;
                    case FunctionNameEnum::toMBData:
                        encodedElement = ElementEncodingUtils::encodeMBData(encodingElementValue, encodingElementLengthInBit);
                        break;
                    default:
                        encoderFailureReport->addFailure(
                                categoryNumber, 0, itemToEncode->getName(), nullptr,
                                "An unknown encoding function has been associated with '" + currentElementName + "' Element to encode!",
                                ReturnCodes::UNKNOWN_DECODING_FUNCTION);
                        break;
                }

                if (encodedElement == nullptr) {
                    string message = "No raw value has been returned by the encoding function for the '" + currentElementName + "' Element!";
                    encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr, message,
                                                     ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
                    return nullptr;
                }

                // The current Element has been consumed, so we are ready to remove it from the decoded Elements collection
                removeDecodedElementNameAndValueFromCollections(encodingElementValueIndex, decodedElementNames, decodedElementValues);

                numberOfElementsFound++;    //Element has been found -> if another Element in this repetition will be not found -> ERROR

            } else {
                encodedElement = new unsigned char[encodingElementLengthInByte];    // TODO: improve to do not allocate for a Spare element
                memset(encodedElement, 0, encodingElementLengthInByte);
            }

            /****************************************************
             * How to manage the Elements NOT aligned to a Byte *
             ****************************************************/
            realBytesToBeWritten = 0;
            bytesWithInformation = 0;

            // Check if there is a remainder from the previous element
            if (remainingBitCounter > 0) {
                // Shift right the actual element and bitwise-or with the previous remainder
                BitUtils::shiftRight(encodedElement, encodingElementLengthInByte, remainingBitCounter);
                encodedElement[0] |= remainingByte;
            }

            // Check if the shift right operation move 'encodedElement' into the additional byte at the end of the element
            int unusedRealBits = ((encodingElementLengthInByte - 1) * 8) - encodingElementLengthInBit;
            if (unusedRealBits >= remainingBitCounter) {
                // Shift fits into (encodingElementLengthInByte-1) bytes
                bytesWithInformation = encodingElementLengthInByte - 1;
                encodingElementLengthInBit += remainingBitCounter;        // Adds the shift to the length in bit of the current element
            } else {
                bytesWithInformation = encodingElementLengthInByte;    // Keeps the entire length of the element (it includes also the additional 0x0 byte at the end)
                encodingElementLengthInBit += remainingBitCounter;        // Adds the shift to the length in bit of the current element
            }

            // Write in the vector if some bytes are completely filled
            realBytesToBeWritten = encodingElementLengthInBit / 8;
            for (int j = 0; j < realBytesToBeWritten; j++) {
                encodedItemBytestream->push_back(encodedElement[j]);
                remainingBitCounter = 0;
            }

            // The actual element length in bit is not a multiple of byte
            if (encodingElementLengthInBit % 8 > 0) {
                remainingByte = encodedElement[bytesWithInformation - 1];                    // Keeps last byte of the element for the next loop
                remainingBitCounter = (bytesWithInformation * 8) - encodingElementLengthInBit;    // For sure the result is between 0 and 8
                remainingBitCounter = 8 - remainingBitCounter;    // Saves the complement -> remaining number of free bits into 'remainingByte'
            }

            delete[] encodedElement;
            encodedElement = nullptr;
        }

        // One ore more Elements have been found and one ore more Elements have NOT been found -> ERROR
        if (numberOfElementsNOTFound > 0 && numberOfElementsFound > 0) {
            string message =
                    "One or more Elements for the current Item " + itemToEncode->getName() + " were not found in the input data, Item was skipped!";
            encoderFailureReport->addFailure(categoryNumber, 0, itemToEncode->getName(), nullptr, message, ReturnCodes::ENCODING_ERROR);
            return nullptr;
        }

        if (encodingElementValueIndex != -1) {
            currentNumberOfRepetition++;

            //Initialize counters for the new repetition
            numberOfElementsFound = 0;
            numberOfElementsNOTFound = 0;
        }

    } while ((encodingElementValue != nullptr) && (encodingElementValueIndex != -1));

    // Number of repetitions is 0 and NO Elements have been found -> Item no present
    if (currentNumberOfRepetition == 0 && numberOfElementsFound == 0)
        return nullptr;


    // The repetiton number is saved in front of the vector
    encodedItemBytestream->insert(encodedItemBytestream->begin(), (unsigned char) currentNumberOfRepetition);

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << "\t Item raw data: ";
        for (unsigned int i = 0; i < encodedItemBytestream->size(); i++) {
            cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(encodedItemBytestream->at(i)) << " ";
        }
        cout << endl;
        cout << "\t Item Length: " << encodedItemBytestream->size() << endl;
    }

    return encodedItemBytestream;
}


vector<unsigned char> *
ItemEncodingUtils::encodeCompoundLengthItem(vector<string *> *decodedElementNames, vector<ElementValue *> *decodedElementValues,
                                            Item *itemToEncodeDescription, vector<Item> *compoundSubItemsDescription, unsigned int categoryNumber) {
    /************************
     * VARIABLES DEFINITION *
     ************************/
    // SubItems information of current Item
    vector<string> *subItemsNames = itemToEncodeDescription->getAllElementNames();
    vector<int> *subItemsIndexes = itemToEncodeDescription->getAllElementDescription();
    unsigned int numberOfSubItem = subItemsNames->size();

    // SubItem information
    int subItemIndex = 0;    // Index of current subItem in 'compoundSubItemsDescription' vector
    unsigned int subitemSize = 0;
    Item *subItem = nullptr;
    ItemTypeEnum subitemType;
    string currentSubItemName;

    // Encoded values stored here
    vector<unsigned char> *subitemBytestream = nullptr;
    vector<unsigned char> *encodedItemBytestream = new vector<unsigned char>();

    // Items indicator
    int maxSizeOfItemsIndicator = 0;
    unsigned int itemsIndicatorCurrentIndex = 0;
    unsigned char *itemsIndicator = nullptr;    // Used to store the presence or absence of the items
    unsigned int itemsIndicatorSize = 0;
    unsigned int itemsIndicatorCurrentNumberOfBits = 0;

    maxSizeOfItemsIndicator = (subItemsNames->size() / 8) + 1;
    itemsIndicator = new unsigned char[maxSizeOfItemsIndicator];

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << itemToEncodeDescription->getName() << endl;
    }

    // Get description for each subItem
    for (unsigned int i = 0; i < numberOfSubItem; i++) {
        // Get index from 'elementsDescription' for get the subItem from 'compoundSubItemsDescription' and then get the description
        subItemIndex = (*subItemsIndexes)[i];
        subItem = &(*compoundSubItemsDescription)[subItemIndex];
        currentSubItemName = subItem->getName();

        if (currentSubItemName == kSpare) {
            itemsIndicator[itemsIndicatorCurrentIndex] = itemsIndicator[itemsIndicatorCurrentIndex] << 1 | 0x0;
        } else {
            // If subItem is present in the decodedResult -> encodedSubItem != nullptr
            subitemType = subItem->getItemType();

            switch (subitemType) {
                case ItemTypeEnum::FixedLength:
                    subitemBytestream = encodeFixedLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;

                case ItemTypeEnum::ExtendedLength:
                    subitemBytestream = encodeExtendedLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;

                case ItemTypeEnum::RepetitiveLength:
                    subitemBytestream = encodeRepetitiveLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;

                default:
                    encoderFailureReport->addFailure(categoryNumber, 0, currentSubItemName, nullptr, "An unknown subitem type has been provided!",
                                                     ReturnCodes::ENCODING_ERROR);
                    break;
            }

            if (subitemBytestream != nullptr) {
                // The current sub item is present -> 1 in the FSPEC
                itemsIndicator[itemsIndicatorCurrentIndex] = itemsIndicator[itemsIndicatorCurrentIndex] << 1 | 0x1;
                subitemSize = subitemBytestream->size();
                // Save each byte of current sub-item into the collection for the current item
                for (unsigned int j = 0; j < subitemSize; j++) {
                    encodedItemBytestream->push_back(subitemBytestream->at(j));
                }
                // Free memory used by the current sub-item
                delete subitemBytestream;
                subitemBytestream = nullptr;
            } else {
                // The current sub item is not present in the decoded value -> put 0 in the FSPEC
                itemsIndicator[itemsIndicatorCurrentIndex] = itemsIndicator[itemsIndicatorCurrentIndex] << 1 | 0x0;
            }
        }
        // Update the number of bits actually set into items indicator bytes (FSPEC)
        itemsIndicatorCurrentNumberOfBits++;

        // When itemsIndicatorCurrentIndex is %7==0 -> add 1 to the Items indicator, that means FX=1
        if (itemsIndicatorCurrentNumberOfBits % 7 == 0) {
            itemsIndicator[itemsIndicatorCurrentIndex] = itemsIndicator[itemsIndicatorCurrentIndex] << 1 | 1;
            itemsIndicatorCurrentIndex++;
        }
    }

    // No encoded sub items because this item is not present in the decoded value given by the user -> It is not an ERROR
    if (encodedItemBytestream->size() == 0) {
        return nullptr;
    }

    // Counting the Field Extension number
    int fieldExtensionCounter = itemsIndicatorCurrentNumberOfBits / 7;
    itemsIndicatorCurrentNumberOfBits += fieldExtensionCounter;

    itemsIndicatorSize = itemsIndicatorCurrentIndex + 1;

    // Completing the last byte of itemsIndicator with 0s - if necessary [itemsIndicatorCurrentNumberOfBits < (itemsIndicatorCurrentIndex + 1) * 8]
    for (unsigned int i = itemsIndicatorCurrentNumberOfBits; i < itemsIndicatorSize * 8; i++) {
        itemsIndicator[itemsIndicatorCurrentIndex] = itemsIndicator[itemsIndicatorCurrentIndex] << 1 | 0x0;
    }

    // Saving the items indicator into the vector, start from index 0
    for (int j = itemsIndicatorCurrentIndex; j >= 0; j--) {
        encodedItemBytestream->insert(encodedItemBytestream->begin(), itemsIndicator[j]);
    }

    // Deleting the itemsIndicator, already saved into the final vector
    delete[] itemsIndicator;
    itemsIndicator = nullptr;

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << "\t FSPEC-compound:" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(encodedItemBytestream->at(0))
             << endl;
        cout << "\t Item Length: " << std::dec << encodedItemBytestream->size() << endl;
    }
    return encodedItemBytestream;
}


vector<unsigned char> *
ItemEncodingUtils::encodeREFItem(vector<string *> *decodedElementNames, vector<ElementValue *> *decodedElementValues, Item *itemToEncode,
                                 vector<Item> *compoundSubItemsDescription, vector<Item> *refItemDescription, unsigned int categoryNumber) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    // Informations of the sub-items of REF
    unsigned int numberOfElementsInItem = itemToEncode->getAllElementNames()->size();

    // Sub-item informations
    Item *subItem;
    ItemTypeEnum subitemType;
    unsigned int subItemSizeInByte = 0;        // Size in byte updated by the encoding functions over sub-items

    // Variables needed to save the encoded values
    vector<unsigned char> *subitemBytestream = nullptr;
    vector<unsigned char> *encodedItemBytestream = new vector<unsigned char>();
    char FSPEC = 0;        // Sub-items indicator of RE
    unsigned int itemSize = 0;

    /******************
     * START ENCODING *
     ******************/
    // Loop over the subItems found in the Item Description
    for (unsigned int i = 0; i < numberOfElementsInItem; i++) {
        subItem = &(refItemDescription->at(i));    // Get i-th sub item of REF
        subitemType = subItem->getItemType();

        if (subItem->isItemToBeEncoded()) {    // If the subItem is NOT to be encoded (setup coming from the JSON filters file) -> skip the encoding for that subItem

            switch (subitemType) {
                case ItemTypeEnum::FixedLength:
                    subitemBytestream = encodeFixedLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;

                case ItemTypeEnum::ExtendedLength:
                    subitemBytestream = encodeExtendedLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;

                case ItemTypeEnum::RepetitiveLength:
                    subitemBytestream = encodeRepetitiveLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;
                case ItemTypeEnum::CompoundLength:
                    subitemBytestream = encodeCompoundLengthItem(decodedElementNames, decodedElementValues, subItem, compoundSubItemsDescription,
                                                                 categoryNumber);
                    break;

                default:
                    encoderFailureReport->addFailure(categoryNumber, 0, subItem->getName(), nullptr, "The given REF subitem type is unknown!",
                                                     ReturnCodes::ENCODING_ERROR);
                    break;
            }
        }

        if (subitemBytestream != nullptr) {
            // Get the size of the encoded sub-item -> length of the vector
            subItemSizeInByte = subitemBytestream->size();
            itemSize += subItemSizeInByte;    // Update total size of the item

            // Raw data of the sub-item saved into the vector
            for (unsigned int b = 0; b < subItemSizeInByte; b++) {
                encodedItemBytestream->push_back(subitemBytestream->at(b));
            }

            // Free the memory used by the sub-item
            delete subitemBytestream;
            subitemBytestream = nullptr;

        } else {
            subItemSizeInByte = 0;
        }

        // Update Items indicator of RE -> simili-FSPEC
        if (subItemSizeInByte > 0) {
            FSPEC <<= 1;
            FSPEC |= 0x1;
        } else {
            FSPEC <<= 1;
            FSPEC |= 0x0;
        }
    }

    if (itemSize == 0) {
        // Item no present -> it is NOT an error
        return nullptr;
    }

    // 1 byte -> LEN
    // 1 byte -> FSPEC of RE
    itemSize += 2;

    // If the one-octet FSPEC of RE is not completely filled, a left-shift is needed
    if (numberOfElementsInItem < 8) {
        FSPEC <<= (8 - numberOfElementsInItem);
    }

    encodedItemBytestream->insert(encodedItemBytestream->begin(), FSPEC);        // Append FSPEC at the beginning of the vector
    encodedItemBytestream->insert(encodedItemBytestream->begin(), itemSize);    // Append also the total size

    return encodedItemBytestream;
}


vector<unsigned char> *
ItemEncodingUtils::encodeSPFItem(vector<string *> *decodedElementNames, vector<ElementValue *> *decodedElementValues, Item *itemToEncode,
                                 vector<Item> *compoundSubItemsDescription, vector<Item> *spfItemDescription, unsigned int categoryNumber) {

    /************************
     * VARIABLES DEFINITION *
     ************************/

    // Informations of the sub-items of SPF  -  Elements details of the current Item description
    unsigned int numberOfElementsInItem = itemToEncode->getAllElementNames()->size();

    // Sub-item informations
    Item *subItem;
    ItemTypeEnum subitemType;
    unsigned int subItemSizeInByte = 0;    // Size in byte updated by the encoding functions called over sub-items

    // Char* to save the encoded values
    vector<unsigned char> *subitemBytestream = nullptr;
    vector<unsigned char> *encodedItemBytestream = new vector<unsigned char>();
    unsigned int itemLengthInByte = 0;

    /******************
     * START ENCODING *
     ******************/

    for (unsigned int i = 0; i < numberOfElementsInItem; i++) {
        subItem = &(spfItemDescription->at(i));    // Get i-th sub item of SPF
        subitemType = subItem->getItemType();

        if (subItem->isItemToBeEncoded()) {    // If the subItem is NOT to be encoded (setup coming from the JSON filters file) -> skip the encoding for that subItem

            switch (subitemType) {
                case ItemTypeEnum::FixedLength:
                    subitemBytestream = encodeFixedLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;

                case ItemTypeEnum::ExtendedLength:
                    subitemBytestream = encodeExtendedLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;

                case ItemTypeEnum::RepetitiveLength:
                    subitemBytestream = encodeRepetitiveLengthItem(decodedElementNames, decodedElementValues, subItem, categoryNumber);
                    break;
                case ItemTypeEnum::CompoundLength:
                    subitemBytestream = encodeCompoundLengthItem(decodedElementNames, decodedElementValues, subItem, compoundSubItemsDescription,
                                                                 categoryNumber);
                    break;
                default:
                    encoderFailureReport->addFailure(categoryNumber, 0, subItem->getName(), nullptr, "The given SPF subitem type is unknown!",
                                                     ReturnCodes::ENCODING_ERROR);
                    break;
            }

            if (subitemBytestream != nullptr) {
                // Get the size of the encoded sub-item -> length of the vector
                subItemSizeInByte = subitemBytestream->size();
                itemLengthInByte += subItemSizeInByte;    // Update total size of the item

                // Raw data of the sub-item saved into a temporary vector
                for (unsigned int b = 0; b < subItemSizeInByte; b++) {
                    encodedItemBytestream->push_back(subitemBytestream->at(b));
                }

                // Free the memory used by the sub-item
                delete subitemBytestream;
                subitemBytestream = nullptr;
            } else {
                subItemSizeInByte = 0;
            }
        }
    }

    if (itemLengthInByte == 0) {
        // Item no present -> it is NOT an error
        return nullptr;
    }

    // 1 byte -> LEN
    itemLengthInByte += 1;
    encodedItemBytestream->insert(encodedItemBytestream->begin(), itemLengthInByte);    // Append the size of the SPF
    return encodedItemBytestream;
}


vector<unsigned char> *
ItemEncodingUtils::encodeExplicitLengthItem(vector<string *> *decodedElementNames, vector<ElementValue *> *decodedElementValues,
                                            Item *itemToEncode, vector<Item> *compoundSubItemsDescription, vector<Item> *refItemDescription,
                                            vector<Item> *spfItemDescription,
                                            unsigned int categoryNumber) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    FacadeCommonVariables *facadeCommon = FacadeCommonVariables::getInstance();
    string itemName = itemToEncode->getName();

    // Char* to save the encoded values
    vector<unsigned char> *encodedItemBytestream = nullptr;

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << itemToEncode->getName() << endl;
    }

    // Call the right encoding function based on the name of the item
    if (itemName == kREF && facadeCommon->isRefEncodingEnabled()) {
        encodedItemBytestream = encodeREFItem(decodedElementNames, decodedElementValues, itemToEncode, compoundSubItemsDescription,
                                              refItemDescription,
                                              categoryNumber);
    } else if (itemName == kSPF && facadeCommon->isSpfEncodingEnabled()) {
        encodedItemBytestream = encodeSPFItem(decodedElementNames, decodedElementValues, itemToEncode, compoundSubItemsDescription,
                                              spfItemDescription,
                                              categoryNumber);
    }

    // No raw value because this Item is not present in the values given by the user -> It is NOT an ERROR
    if (encodedItemBytestream == nullptr) {
        return nullptr;
    }

    if (commonVariables->isEncodingDebugEnabled()) {
        cout << "\t Item Length: " << std::dec << encodedItemBytestream->size() << endl;
    }

    return encodedItemBytestream;
}
