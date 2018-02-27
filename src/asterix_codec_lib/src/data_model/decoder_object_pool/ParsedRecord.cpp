#include "data_model/decoder_object_pool/ParsedRecord.h"

ParsedRecord::ParsedRecord(unsigned int _poolSize) {
    objectPoolCurrentSize = 0;
    membershipQueue = 0;

    rawRecord = nullptr;
    rawRecordLength = 0;

    recordCategoryNumber = 0;

    // Create ParsedElement objects for initialize the object pool
    for (unsigned int i = 0; i < _poolSize; i++)
        parsedElementsObjectPool.push_back(new ParsedElement());
}

ParsedRecord::~ParsedRecord() {

    //Release all ParsedElement objects in the object pool
    unsigned int objectPoolSize = parsedElementsObjectPool.size();
    for (unsigned int i = 0; i < objectPoolSize; i++)
        delete parsedElementsObjectPool[i];

    parsedElementsObjectPool.clear();
}

void ParsedRecord::setParsedElement(Item *itemInformation, int index, unsigned char *parsedValue) {
    ParsedElement *parsedElement = nullptr;

    if (objectPoolCurrentSize < parsedElementsObjectPool.size()) {
        parsedElement = parsedElementsObjectPool[objectPoolCurrentSize];
    } else {
        //cout << "No enough space for the element" << endl;

        parsedElement = new ParsedElement();

        parsedElementsObjectPool.resize(objectPoolCurrentSize + 1);
        parsedElementsObjectPool[objectPoolCurrentSize] = parsedElement;
    }

    parsedElement->setItemDescription(itemInformation);
    parsedElement->setIndexInItemVectors(index);
    parsedElement->setParsedValue(parsedValue);

    objectPoolCurrentSize++;
}

std::vector<ParsedElement *> *ParsedRecord::getAllParsedElementsPtr() {
    return &parsedElementsObjectPool;
}

unsigned char *ParsedRecord::getRawRecord() {
    return rawRecord;
}

void ParsedRecord::setRawRecord(unsigned char *_rawRecord) {
    rawRecord = _rawRecord;
}

unsigned short int ParsedRecord::getRawRecordLength() {
    return rawRecordLength;
}

void ParsedRecord::setRawRecordLength(unsigned short int _lengthRawRecord) {
    rawRecordLength = _lengthRawRecord;
}

unsigned short int ParsedRecord::getRecordCategoryNumber() {
    return recordCategoryNumber;
}

void ParsedRecord::setRecordCategoryNumber(unsigned short int _recordCategoryNumber) {
    recordCategoryNumber = _recordCategoryNumber;
}

unsigned short int ParsedRecord::getObjectPoolCurrentSize() {
    return objectPoolCurrentSize;
}

void ParsedRecord::setFreeAllElements() {
    ParsedElement *parsedElement = nullptr;

    // Release data saved in ParsedElement and not the ParsedElement pointer
    for (unsigned int i = 0; i < objectPoolCurrentSize; i++) {
        parsedElement = parsedElementsObjectPool[i];
        parsedElement->freeMemory();
    }
    objectPoolCurrentSize = 0;
}

void ParsedRecord::setMembershipQueue(int index) {
    membershipQueue = index;
}

int ParsedRecord::getMembershipQueue() {
    return membershipQueue;
}
