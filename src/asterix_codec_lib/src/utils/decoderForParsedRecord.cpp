#include "utils/decoderForParsedRecord.h"

CommonVariables *common = CommonVariables::getInstance();
FacadeCommonVariables *facadeCommon = FacadeCommonVariables::getInstance();
FailureReport *decoderFailureReport = FailureReport::getInstance();

void decodeAllElementsOfRecord(vector<ParsedElement *> *allParsedElements, unsigned int parsedElementsSize, unsigned int categoryNumber,
                               DecodedValueObjectPoolManager *decodedValueOPManager, vector<string *> *decodedElementNames,
                               vector<ElementValue *> *decodedElementValues,
                               ofstream *outputFile) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    ParsedElement *parsedElement = nullptr;            // PARSED but not yet decoded

    string *elementName;
    FunctionNameEnum functionNameToDecode;
    RepresentationModeEnum representationModeForDecoding;
    Range *valueRanges;
    vector<double> *lsbValues;
    int elementLengthInBit;
    double multiplicationFactorToDecode = 1.0;
    unsigned char *parsedElementValue;
    string previousParsedElementValueInString;

    string parsedValueFirstBit;
    unsigned int indexInLsbValues = 0;

    bool debugEnabledForDecoder = common->isDebugEnabledForDecoder();
    bool dumpOnFileEnable = facadeCommon->isDumpOnFileEnableForDecoder();

    ElementValue *elementValue = nullptr;            // decoded element

    // Variables to access at the information of a element
    Item *itemDescription = nullptr;
    int indexInItemVectors = -1;

    for (unsigned int k = 0; k < parsedElementsSize; k++) {
        parsedElement = allParsedElements->at(k);

        itemDescription = parsedElement->getItemDescription();
        indexInItemVectors = parsedElement->getIndexInItemVectors();

        if (itemDescription == nullptr || indexInItemVectors == -1) {
            decoderFailureReport->addFailure(categoryNumber, 0, "", nullptr, "Element description not found, Element skipped!",
                                             ReturnCodes::DECODING_ERROR);
            continue;    // Skipping the current Element, but continue decoding the Record..
        }

        elementName = itemDescription->getElementNameAtIndex(indexInItemVectors);
        elementLengthInBit = itemDescription->getAllElementDescription()->at(indexInItemVectors);
        functionNameToDecode = itemDescription->getFunctionNameInEnumForDecodingAtIndex(indexInItemVectors);
        representationModeForDecoding = itemDescription->getRepresentationModeInEnumForDecodingAtIndex(indexInItemVectors);
        lsbValues = itemDescription->getLsbValuesAtIndex(indexInItemVectors);
        valueRanges = itemDescription->getValidRangeAtIndex(indexInItemVectors);

        parsedElementValue = parsedElement->getParsedValue();

        if (debugEnabledForDecoder)
            cout << "Raw data: \t0x" << parsedElement->getStringValue() << "\t\t" << parsedElement->getName() << endl;

        if (lsbValues->size() > 1 && k != 0) {
            previousParsedElementValueInString = allParsedElements->at(k - 1)->getStringValue();
            indexInLsbValues = stoi(previousParsedElementValueInString);
        } else {
            indexInLsbValues = 0;
        }

        multiplicationFactorToDecode = lsbValues->at(indexInLsbValues);

        switch (functionNameToDecode) {
            case FunctionNameEnum::toDecimal:
                elementValue = DecodingUtils::toDecimal(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                        &representationModeForDecoding,
                                                        valueRanges, categoryNumber, elementName, decodedValueOPManager);
                break;
            case FunctionNameEnum::toHexadecimal:
                elementValue = DecodingUtils::toHexadecimal(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                            &representationModeForDecoding, valueRanges, categoryNumber, elementName,
                                                            decodedValueOPManager);
                break;

            case FunctionNameEnum::toOctal:
                elementValue = DecodingUtils::toOctal(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                      &representationModeForDecoding,
                                                      valueRanges, categoryNumber, elementName, decodedValueOPManager);
                break;

            case FunctionNameEnum::CA2toDecimal:
                elementValue = DecodingUtils::CA2toDecimal(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                           &representationModeForDecoding, valueRanges, categoryNumber, elementName,
                                                           decodedValueOPManager);
                break;

            case FunctionNameEnum::CA2toHexadecimal:
                elementValue = DecodingUtils::CA2toHexadecimal(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                               &representationModeForDecoding, valueRanges, categoryNumber, elementName,
                                                               decodedValueOPManager);
                break;

            case FunctionNameEnum::toASCII:
                elementValue = DecodingUtils::toAscii(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                      &representationModeForDecoding,
                                                      categoryNumber, elementName, decodedValueOPManager);
                break;

            case FunctionNameEnum::toTargetId:
                elementValue = DecodingUtils::toTargetId(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                         &representationModeForDecoding,
                                                         categoryNumber, elementName, decodedValueOPManager);
                break;

            case FunctionNameEnum::toMBData:
                elementValue = DecodingUtils::toMBData(parsedElementValue, &elementLengthInBit, &multiplicationFactorToDecode,
                                                       &representationModeForDecoding,
                                                       categoryNumber, elementName, decodedValueOPManager);
                break;

            default:
                // Unknown function name to decode the element
                decoderFailureReport->addFailure(categoryNumber, 0, (*elementName), nullptr,
                                                 "For the given Element, an UNKNOWN decoding function name has been provided!",
                                                 ReturnCodes::DECODING_ERROR);
                break;
        }

        if (debugEnabledForDecoder) {
            cout << "Decoded data: \t" << elementValue->getDecodedDataInString() << " .::. " << parsedElement->getName() << endl;
        }
        if (dumpOnFileEnable) {
            (*outputFile) << *(elementName) << ":" << *(elementValue->getPointerToDecodedDataInString()) << "; ";
        }

        decodedElementNames->push_back(elementName);
        decodedElementValues->push_back(elementValue);
    }
}

RecordCollection *decodeParsedRecords(unsigned int numberOfConsumer, DecodedValueObjectPoolManager *decodedValueOPManager) {
    /************************
     * VARIABLES DEFINITION *
     ************************/
    pthread_t currentThreadId = 0;

    BlockingQueueManager *blockingQueuePtr = BlockingQueueManager::getInstance();

    ParsedRecord *elementsOP = nullptr;                // Object Pool where all the parsed Elements are stored AFTER parsing, BEFORE decoding
    vector<ParsedElement *> *allParsedElements = nullptr;        // store all parsed elements of a record
    unsigned int sizeParsedElements = 0;

    // Variables to store the consumer result
    RecordCollection *consumerResult = new RecordCollection();

    //Each vector, at the same index, contains values refer to the same record
    vector<vector<string *> *> *collectionOfDecodedElementNames = new vector<vector<string *> *>();
    vector<vector<ElementValue *> *> *collectionOfDecodedElementValues = new vector<vector<ElementValue *> *>();
    vector<unsigned char *> *collectionOfRawRecords = new vector<unsigned char *>();
    vector<unsigned int> *collectionOfRawRecordsLength = new vector<unsigned int>();
    vector<unsigned short int>* collectionOfRecordCategoryNumber = new vector<unsigned short int>();

    vector<string *> *decodedElementNames = nullptr;
    vector<ElementValue *> *decodedElementValues = nullptr;

    currentThreadId = pthread_self();

    string outputFileName = common->getOutputTxtPathForDecoder() + "/out_decoding" + to_string(numberOfConsumer) + ".txt";
    bool dumpOnFileEnable = facadeCommon->isDumpOnFileEnableForDecoder();
    ofstream outputFile;

    if (dumpOnFileEnable) {
        outputFile.open(outputFileName, ios::out | ios::app);      // "ios::app" to append at the end of the file
    }

    //Waiting for a ParsedElementsObjectPool from the BQManager
    while ((elementsOP = blockingQueuePtr->getObjectPoolForConsumer(currentThreadId)) != nullptr) {
        // Prepare vectors for store the decoded elements
        decodedElementNames = new vector<string *>();
        decodedElementValues = new vector<ElementValue *>();

        //Get parsed elements to decode
        allParsedElements = elementsOP->getAllParsedElementsPtr();
        sizeParsedElements = elementsOP->getObjectPoolCurrentSize();

        if (dumpOnFileEnable) {
            //Print the category number
            outputFile << "{" << elementsOP->getRecordCategoryNumber() << " [";
        }

        // ** DECODE ALL ELEMENTS OF RECORD **
        decodeAllElementsOfRecord(allParsedElements, sizeParsedElements, elementsOP->getRecordCategoryNumber(), decodedValueOPManager,
                                  decodedElementNames,
                                  decodedElementValues, &outputFile);

        // Save in the vectors the information to compose the DecoderResult
        collectionOfRawRecords->push_back(elementsOP->getRawRecord());
        collectionOfRawRecordsLength->push_back(elementsOP->getRawRecordLength());
        collectionOfRecordCategoryNumber->push_back(elementsOP->getRecordCategoryNumber());

        // We may now release the Object Pool, to let the Producer fill it with new parsed Records
        blockingQueuePtr->releaseObjectPoolForProducer(numberOfConsumer, elementsOP);

        collectionOfDecodedElementNames->push_back(decodedElementNames);
        collectionOfDecodedElementValues->push_back(decodedElementValues);

        if (dumpOnFileEnable) {
            outputFile << "]}\n\n";
        }
    }

    // Set collection of decoded elements, category numbers, raw records and length of raw records in the DecoderResult
    consumerResult->setCollectionOfElementName(collectionOfDecodedElementNames);
    consumerResult->setCollectionOfElementValue(collectionOfDecodedElementValues);
    consumerResult->setCollectionOfRawRecord(collectionOfRawRecords);
    consumerResult->setCollectionOfRawRecordLength(collectionOfRawRecordsLength);
    consumerResult->setCollectionOfRecordCategoryNumber(collectionOfRecordCategoryNumber);

    if (dumpOnFileEnable) {
        outputFile.close();
    }

    return consumerResult;
}