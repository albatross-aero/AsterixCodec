#include "asterix_codec/DecoderInterface.h"

/**
 * Singleton that manage access to the blocking queue for both producer side and consumer side. It is thread safe.
 */
BlockingQueueManager *bqManager;        // Pointer to the Singleton

// Initialization of the static field 'singletonDecoder'
DecoderInterface *DecoderInterface::singletonDecoder = nullptr;

DecoderInterface::DecoderInterface() {
    common = CommonVariables::getInstance();
    facadeCommon = FacadeCommonVariables::getInstance();
    bqManager = nullptr;
    ASTERIX_bytestream = nullptr;
    inputFile_sizeInByte = 0;
}

DecoderInterface::~DecoderInterface() {
    if (!common) {
        common->deleteInstance();
        common = nullptr;
    }
    if (!facadeCommon) {
        facadeCommon->deleteInstance();
        facadeCommon = nullptr;
    }
    if (!bqManager) {
        bqManager->deleteInstance();
        bqManager = nullptr;
    }
    if (!singletonDecoder) {
        singletonDecoder->deleteInstance();
        singletonDecoder = nullptr;
    }

    unsigned int sizeOfVector = decodedValueOPManagerVector.size();
    DecodedValueObjectPoolManager *tmp = nullptr;

    for (unsigned int i = 0; i < sizeOfVector; i++) {
        tmp = decodedValueOPManagerVector.at(i);
        if (tmp != nullptr) {
            delete tmp;
            tmp = nullptr;
        }
    }
}

DecoderInterface *DecoderInterface::getInstance() {
    if (!singletonDecoder) {
        singletonDecoder = new DecoderInterface();
    }
    return singletonDecoder;
}

void DecoderInterface::deleteInstance() {
    if (singletonDecoder != nullptr)
        delete singletonDecoder;

    singletonDecoder = nullptr;
}

//** OTHER FUNCTIONS **
void DecoderInterface::startDecoderThreads() {
    if (decodedValueOPManagerVector.empty()) {
        // For EACH CONSUMER, creating an OP to store the decoded values
        for (unsigned int i = 0; i < facadeCommon->getNumberOfDecodingThreads(); i++) {
            decodedValueOPManagerVector.push_back(new DecodedValueObjectPoolManager());
        }
    }

    ThreadPool *decoderTP = facadeCommon->getConsumerThreadPool();

    for (unsigned int i = 0; i < facadeCommon->getNumberOfDecodingThreads(); i++) {

        // For EACH CONSUMER, getting the OP to store the decoded values
        decodingResults.push_back(decoderTP->enqueue(decodeParsedRecords, i, decodedValueOPManagerVector.at(i)));
    }
}

RecordCollection *DecoderInterface::waitForConsumer() {
    RecordCollection *tmpResult = nullptr;
    RecordCollection *decodedRecords = new RecordCollection();

    for (auto &&result : decodingResults) {
        tmpResult = result.get();

        if (tmpResult != nullptr) {
            decodedRecords->addRecordsInCollection(tmpResult);
        }
    }

    // All futures in the vector have been used, now we can clear the vector
    decodingResults.clear();

    return decodedRecords;
}

void DecoderInterface::releaseAllDecodedRecords(RecordCollection *decoderResult) {
    for (vector<DecodedValueObjectPoolManager *>::iterator it = decodedValueOPManagerVector.begin();
         it != decodedValueOPManagerVector.end(); it++) {
        (*it)->releaseAllElementValueToOP();
    }

    delete decoderResult;
    decoderResult = nullptr;
}

void DecoderInterface::setSourceDatastream(unsigned char *_ASTERIX_datastream, unsigned long _ASTinput_fileSize) {
    ASTERIX_bytestream = _ASTERIX_datastream;
    inputFile_sizeInByte = _ASTinput_fileSize;
}

RecordCollection *
DecoderInterface::startDecodingDatablocks(unsigned char *ASTERIX_datastream, unsigned int *lengthOfBytestreamToParse) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    ThreadPool *parserTP = nullptr;
    RecordCollection *decodedRecords = nullptr;
    unsigned long long recordNumber = 0;        // Number of record that have been parsed in the current datablock TODO: modify to be atomic variable

    unsigned short int datablockCategory = 0;       // Datablock CAT field value
    unsigned short int datablockLength = 0;         // Datablock LEN field value
    unsigned long bufferIndex = 0;                  // Main index to move around the 'ASTERIX_bytestream' buffer

    //Do not move this line!
    if (bqManager == nullptr)
        bqManager = BlockingQueueManager::getInstance();

    unordered_map<int, CategoryDescription *> *categoriesDescription = common->getCategoriesDescription();

    bqManager->openAllBlockingQueues();
    startDecoderThreads();

    parserTP = facadeCommon->getParserThreadPool();

    //TODO: Check current number of datablocks if is less or equal to common->getMaximumDatablock

    // There are bytes to read while 'bufferIndex' is less than 'ASTERIX_inputFileSize'
    while (bufferIndex < (*lengthOfBytestreamToParse)) {

        // Reading the datablock Category number (1 Byte)
        datablockCategory = (unsigned short int) ASTERIX_datastream[bufferIndex];
        // Reading the datablock Length value (2 Bytes)
        datablockLength = (unsigned short int) (ASTERIX_datastream[bufferIndex + 2] +
                                                ((ASTERIX_datastream[bufferIndex + 1] << 8) & 0xFF00));

        if ((bufferIndex + datablockLength) <= (*lengthOfBytestreamToParse)) {
            parsingResults.push_back(
                    parserTP->enqueue(ParsingUtils::parseDatablock, ASTERIX_datastream, datablockCategory,
                                      datablockLength, categoriesDescription,
                                      bufferIndex));
        } else {
            // Last Datablock is not complete, return to the caller a message error in the FailureReport
            string message = "The Datablock provided starting at index " + to_string(bufferIndex) +
                             " in the data stream is not complete. The declared size is "
                             + to_string(datablockLength) + " but the data stream ended at " +
                             to_string(bufferIndex + datablockLength) +
                             " position.";
            FailureReport::getInstance()->addFailure(datablockCategory, bufferIndex, "", nullptr, message,
                                                     ReturnCodes::PARSING_ERROR);
        }

        bufferIndex += datablockLength;
    }

    // Waits for the completion of all parsing threads
    for (auto &&result : parsingResults) {
        recordNumber += result.get();
    }

    // Close the blocking queue to notify the consumer to finish data and return
    bqManager->closeAllBlockingQueues();

    parsingResults.clear();

    decodedRecords = waitForConsumer();

    if (facadeCommon->isJsonOutputEnableForDecoder())
        printAllDecodedRecordsOnJson(decodedRecords);

    return decodedRecords;
}

unsigned int DecoderInterface::setupDecodingFromFile() {
    /************************
     * VARIABLES DEFINITION *
     ************************/
    unsigned int numberOfDatablocksToParse = facadeCommon->getMaximumNumberOfDatablock();
    unsigned int currentNumberOfDatablocks = 0;

    unsigned int asterixBytestream_index_start = asterixBytestream_index;
    unsigned short int datablock_lengthInByte = 0;            // Datablock LEN field value
    unsigned int asterixBytestream_toParse_lengthInByte = 0;

    // There are bytes to read until 'asterixBytestream_index' is less than 'inputFile_sizeInByte'
    //if currentNumberOfDatablocks == numberOfDatablocksToParse -> we have collect enough datablocks -> we can decode
    while ((asterixBytestream_index < inputFile_sizeInByte) &&
           (currentNumberOfDatablocks < numberOfDatablocksToParse)) {
        // Reading the datablock Length value (2 Bytes)
        datablock_lengthInByte = (unsigned short int) (ASTERIX_bytestream[asterixBytestream_index + 2]
                                                       + ((ASTERIX_bytestream[asterixBytestream_index + 1] << 8) &
                                                          0xFF00));    //1111 1111 0000 0000b

        if ((asterixBytestream_index + datablock_lengthInByte) <= inputFile_sizeInByte) {
            currentNumberOfDatablocks++;
            asterixBytestream_index += datablock_lengthInByte;
        } else {
            // Last Datablock is not complete -> it will be dropped
            asterixBytestream_index = inputFile_sizeInByte;
            // The 'asterixBytestream_index' is placed at the end of the file to stop the outer loop: EOF reached.
        }
    }

    if (currentNumberOfDatablocks == 0) {
        return 0;
    }
    asterixBytestream_toParse_lengthInByte = asterixBytestream_index - asterixBytestream_index_start;
    return asterixBytestream_toParse_lengthInByte;
}

RecordCollection *DecoderInterface::decodeFromFile() {
    unsigned int asterixBytestream_initial_offset = asterixBytestream_index;
    unsigned int asterixBytestream_toParse_lengthInByte = setupDecodingFromFile();

    return startDecodingDatablocks(ASTERIX_bytestream + asterixBytestream_initial_offset,
                                   &asterixBytestream_toParse_lengthInByte);
}

void DecoderInterface::printAllDecodedRecordsOnJson(RecordCollection *recordsToPrint) {
    unsigned int recordsToPrintSize = 0;
//    unsigned int maximumNumberOfRecordInJson = 0;
//    unsigned int number_of_elements = 0;

//    ElementValue *elementValue = nullptr;
    string *elementName = nullptr;
    unsigned int totalNumRecordsToPrint = 0;

    unsigned int actualNumberOfFiles = 0;

    vector<string *> *elementNames_toPrint = nullptr;
    vector<ElementValue *> *elementValues_toPrint = nullptr;

    recordsToPrintSize = recordsToPrint->getTotalNumberOfRecords();
//    maximumNumberOfRecordInJson = common->getMaximumNumberOfRecordInJSON();

    std::string originalFileName, fileName;

    originalFileName = common->getOutputJsonPathForDecoder() + "/out_decoding.json";

    // JSON obj used later
    json jsonDocument = json::array();
    json jsonRecord;
    json jsonElement;

    for (unsigned int i = 0; i < recordsToPrintSize; i++) {
        elementNames_toPrint = recordsToPrint->getElementNamesOfARecord(i);
        elementValues_toPrint = recordsToPrint->getElementValuesOfARecord(i);

        jsonRecord = json::object();

        // put in the DOM all elements
        for (unsigned int j = 0; j < elementValues_toPrint->size(); j++) {
            elementName = elementNames_toPrint->at(j);
//            elementValue = elementValues_toPrint->at(j);

            // ...
            //jsonElement[elementName->c_str()] = elementValue;
            jsonElement[elementName->c_str()] = "ciao";
            jsonRecord.emplace_back(jsonElement);
        }

        totalNumRecordsToPrint++;
        // ... add jsonRecord to external object
        jsonDocument.emplace_back(jsonRecord);

//        if (totalNumRecordsToPrint > maximumNumberOfRecordInJson) {
//
//            fileName = originalFileName;
//            actualNumberOfFiles = common->getActualNumberOfOutFiles();
//            fileName += "." + std::to_string(actualNumberOfFiles);
//
//            std::remove(fileName.c_str());
//
//            // document is written on a file
//            ofstream ofs(fileName);
//
//            // ... print the external JSON object to 'ofs' file stream
//
//            totalNumRecordsToPrint = 0;    // Reset all counters
//            // ... empty the external JSON object
//        }
    }

    // Check if document has something else to print on file
    if (jsonDocument > 0) {
        fileName = originalFileName;
        actualNumberOfFiles = common->getActualNumberOfOutFiles();
        fileName += "." + std::to_string(actualNumberOfFiles);

        std::remove(fileName.c_str());

        // document is written on a file
        ofstream ofs(fileName);
        ofs << std::setw(4) << jsonDocument << endl;

        totalNumRecordsToPrint = 0;            // Reset all counters
    }
}
