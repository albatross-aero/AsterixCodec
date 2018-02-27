#include "config/FacadeCommonVariables.h"

// Initialization of static field 'commonSingleton'
FacadeCommonVariables *FacadeCommonVariables::commonSingleton = nullptr;

FacadeCommonVariables::FacadeCommonVariables() {
    REF_decodingEnabled = false;
    SPF_decodingEnabled = false;

    REF_encodingEnabled = false;
    SPF_encodingEnabled = false;

    filter_decodingEnabled = false;
    filter_encodingEnabled = false;

    jsonOutputEnableForDecoder = false;
    dumpOnFileEnableForDecoder = false;
    astOutputEnableForEncoder = false;

    totalNumberOfThreads = 2;
    numberOfConsumerThreads = 1;
    numberOfProducerThreads = 1;
    numberOfEncoderThreads = 1;
    numberOfElementsInPreallocatedParsedRecord = 150;
    numberOfPreallocatedParsedRecord = 1;

    consumer = nullptr;
    producer = nullptr;

    maximumNumberOfDatablockToDecode = 1000;
    averageNumberOfRecordPerDatablock = 5;
}

FacadeCommonVariables *FacadeCommonVariables::getInstance() {
    if (!commonSingleton) {
        commonSingleton = new FacadeCommonVariables();
    }
    return commonSingleton;
}

void FacadeCommonVariables::deleteInstance() {
    delete commonSingleton;
}

FacadeCommonVariables::~FacadeCommonVariables() {
    if (!producer) {
        delete producer;
        producer = nullptr;
    }
    if (!consumer) {
        delete consumer;
        consumer = nullptr;
    }
}

//GETTERS AND SETTERS
bool FacadeCommonVariables::isRefDecodingEnabled() {
    return REF_decodingEnabled;
}

void FacadeCommonVariables::setRefDecodingEnabled(bool refDecodingEnabled) {
    REF_decodingEnabled = refDecodingEnabled;
}

bool FacadeCommonVariables::isSpfDecodingEnabled() {
    return SPF_decodingEnabled;
}

void FacadeCommonVariables::setSpfDecodingEnabled(bool spfDecodingEnabled) {
    SPF_decodingEnabled = spfDecodingEnabled;
}

bool FacadeCommonVariables::isFilterDecodingEnabled() {
    return filter_decodingEnabled;
}

void FacadeCommonVariables::setFilterDecodingEnabled(bool filterDecodingEnabled) {
    filter_decodingEnabled = filterDecodingEnabled;
}

bool FacadeCommonVariables::isFilterEncodingEnabled() {
    return filter_encodingEnabled;
}

void FacadeCommonVariables::setFilterEncodingEnabled(bool filterEncodingEnabled) {
    filter_encodingEnabled = filterEncodingEnabled;
}

bool FacadeCommonVariables::isRefEncodingEnabled() {
    return REF_encodingEnabled;
}

void FacadeCommonVariables::setRefEncodingEnabled(bool refEncodingEnabled) {
    REF_encodingEnabled = refEncodingEnabled;
}

bool FacadeCommonVariables::isSpfEncodingEnabled() {
    return SPF_encodingEnabled;
}

void FacadeCommonVariables::setSpfEncodingEnabled(bool spfEncodingEnabled) {
    SPF_encodingEnabled = spfEncodingEnabled;
}

unsigned short FacadeCommonVariables::getTotalNumberOfThread() {
    return totalNumberOfThreads;
}

void FacadeCommonVariables::setTotalNumberOfThread(unsigned short _numberOfThreads) {
    totalNumberOfThreads = _numberOfThreads;
}

unsigned short FacadeCommonVariables::getNumberOfProducerThreads() {
    return numberOfProducerThreads;
}

void FacadeCommonVariables::setNumberOfProducerThreads(unsigned short _numberOfThreads) {
    numberOfProducerThreads = _numberOfThreads;
}

unsigned short FacadeCommonVariables::getNumberOfDecodingThreads() {
    return numberOfConsumerThreads;
}

void FacadeCommonVariables::setNumberOfConsumerThreads(unsigned short _numberOfThreads) {
    numberOfConsumerThreads = _numberOfThreads;
}

unsigned short FacadeCommonVariables::getNumberOfEncoderThreads() {
    return numberOfEncoderThreads;
}

void FacadeCommonVariables::setNumberOfEncoderThreads(unsigned short _numberOfThreads) {
    numberOfEncoderThreads = _numberOfThreads;
}

unsigned int FacadeCommonVariables::getNumberOfElementsInPreallocatedParsedRecord() {
    return numberOfElementsInPreallocatedParsedRecord;
}

void FacadeCommonVariables::setNumberOfElementsInPreallocatedParsedRecord(unsigned int _elementObjectPoolSize) {
    numberOfElementsInPreallocatedParsedRecord = _elementObjectPoolSize;
}

unsigned int FacadeCommonVariables::getNumberOfPreallocatedParsedRecord() {
    if (numberOfPreallocatedParsedRecord < numberOfProducerThreads)
        numberOfPreallocatedParsedRecord = numberOfProducerThreads;

    return numberOfPreallocatedParsedRecord;
}

void FacadeCommonVariables::setNumberOfPreallocatedParsedRecord(unsigned int _numberOfObjectPool) {
    numberOfPreallocatedParsedRecord = _numberOfObjectPool;
}

unsigned int FacadeCommonVariables::getMaximumNumberOfDatablock() {
    return maximumNumberOfDatablockToDecode;
}

void FacadeCommonVariables::setMaximumNumberOfDatablock(unsigned int _maximumNumberOfDatablock) {
    maximumNumberOfDatablockToDecode = _maximumNumberOfDatablock;
}

unsigned int FacadeCommonVariables::getAverageNumberOfRecordPerDatablock() {
    return averageNumberOfRecordPerDatablock;
}

void FacadeCommonVariables::setAverageNumberOfRecordPerDatablock(unsigned int _averageNumberOfRecordPerDatablock) {
    averageNumberOfRecordPerDatablock = _averageNumberOfRecordPerDatablock;
}

ThreadPool *FacadeCommonVariables::getConsumerThreadPool() {
    return consumer;
}

void FacadeCommonVariables::setConsumerThreadPool(ThreadPool *consumer) {
    this->consumer = consumer;
}

ThreadPool *FacadeCommonVariables::getParserThreadPool() {
    return producer;
}

void FacadeCommonVariables::setProducerThreadPool(ThreadPool *producer) {
    this->producer = producer;
}

bool FacadeCommonVariables::isJsonOutputEnableForDecoder() {
    return jsonOutputEnableForDecoder;
}

void FacadeCommonVariables::setJsonOutputEnableForDecoder(bool _jsonOutputEnableForDecoder) {
    jsonOutputEnableForDecoder = _jsonOutputEnableForDecoder;
}

bool FacadeCommonVariables::isDumpOnFileEnableForDecoder() {
    return dumpOnFileEnableForDecoder;
}

bool FacadeCommonVariables::isAstOutputEnableForEncoder() {
    return astOutputEnableForEncoder;
}

void FacadeCommonVariables::setAstOutputEnableForEncoder(bool _astOutputEnableForEncoder) {
    astOutputEnableForEncoder = _astOutputEnableForEncoder;
}

void FacadeCommonVariables::setDumpOnFileEnableForDecoder(bool _dumpOnFileEnableForDecoder) {
    dumpOnFileEnableForDecoder = _dumpOnFileEnableForDecoder;
}

// OTHER METHODS
void FacadeCommonVariables::printCommonParameters() {

    std::cout << "\t\t filter_decodingEnabled: " << std::boolalpha << filter_decodingEnabled << endl << "\t\t filter_encodingEnabled: "
              << std::boolalpha
              << filter_encodingEnabled << endl << "\t\t REF_decodingEnabled: " << std::boolalpha
              << REF_decodingEnabled << endl << "\t\t SPF_decodingEnabled: " << std::boolalpha
              << SPF_decodingEnabled << endl << "\t\t REF_encodingEnabled: " << std::boolalpha << REF_encodingEnabled << endl
              << "\t\t SPF_encodingEnabled: "
              << std::boolalpha << SPF_encodingEnabled << endl << "\t\t jsonOutputEnableForDecoder: " << std::boolalpha << jsonOutputEnableForDecoder
              << endl
              << "\t\t dumpOnFileEnableForDecoder: " << std::boolalpha << dumpOnFileEnableForDecoder << endl << "\t\t astOutputEnableForEncoder: "
              << std::boolalpha << astOutputEnableForEncoder << endl << "\t\t numberOfProducerThreads: " << std::dec
              << numberOfProducerThreads << endl << "\t\t numberOfConsumerThreads: " << numberOfConsumerThreads << endl
              << "\t\t numberOfEncoderThreads: "
              << std::dec << numberOfEncoderThreads
              << endl << "\t\t numberOfElementsInPreallocatedParsedRecord: " << numberOfElementsInPreallocatedParsedRecord << endl
              << "\t\t numberOfPreallocatedParsedRecord: "
              << numberOfPreallocatedParsedRecord << endl << "\t\t maximumNumberOfDatablock: " << maximumNumberOfDatablockToDecode << endl
              << "\t\t averageNumberOfRecordPerDatablock: " << averageNumberOfRecordPerDatablock << endl;

}
