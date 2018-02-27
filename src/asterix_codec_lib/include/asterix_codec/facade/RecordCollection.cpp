#include "asterix_codec/RecordCollection.h"

RecordCollection::RecordCollection() {
    collectionOfElementName = new vector<vector<string *> *>();
    collectionOfElementValue = new vector<vector<ElementValue *> *>();
    collectionOfRawRecord = new vector<unsigned char *>();
    collectionOfRawRecordLength = new vector<unsigned int>();
    collectionOfRecordCategoryNumber = new vector<unsigned short int>();
}

RecordCollection::~RecordCollection() {
    unsigned int sizeOfVector = collectionOfElementName->size();

    for (unsigned int i = 0; i < sizeOfVector; i++) {

        delete collectionOfElementName->at(i);
        collectionOfElementName->at(i) = nullptr;

        delete collectionOfElementValue->at(i);
        collectionOfElementValue->at(i) = nullptr;

        delete[] collectionOfRawRecord->at(i);
        collectionOfRawRecord->at(i) = nullptr;
    }

    delete collectionOfElementName;
    delete collectionOfElementValue;
    delete collectionOfRawRecord;
    delete collectionOfRawRecordLength;
    delete collectionOfRecordCategoryNumber;

    collectionOfElementName = nullptr;
    collectionOfElementValue = nullptr;
    collectionOfRawRecord = nullptr;
    collectionOfRawRecordLength = nullptr;
    collectionOfRecordCategoryNumber = nullptr;
}

// ELEMENT NAME
void RecordCollection::setCollectionOfElementName(vector<vector<string *> *> *_collectionOfElementName) {
    collectionOfElementName = _collectionOfElementName;
}

vector<string *> *RecordCollection::getElementNamesOfARecord(unsigned int recordNumber) {
    return collectionOfElementName->at(recordNumber);
}

vector<string *> *RecordCollection::makeACopyOfRecordElementNames(unsigned int recordNumber) {
    vector<string *> *originalVectorOfElementName = collectionOfElementName->at(recordNumber);
    vector<string *> *copyOfCollectionOfElementName = new vector<string *>(originalVectorOfElementName->size());

    std::copy(originalVectorOfElementName->begin(), originalVectorOfElementName->end(), copyOfCollectionOfElementName->begin());

    return copyOfCollectionOfElementName;
}

// ELEMENT VALUE
void RecordCollection::setCollectionOfElementValue(vector<vector<ElementValue *> *> *_collectionOfElementValue) {
    collectionOfElementValue = _collectionOfElementValue;
}

vector<ElementValue *> *RecordCollection::getElementValuesOfARecord(unsigned int recordNumber) {
    return collectionOfElementValue->at(recordNumber);
}

vector<ElementValue *> *RecordCollection::makeACopyOfRecordElementValues(unsigned int recordNumber) {
    vector<ElementValue *> *originalVectorOfElementValue = collectionOfElementValue->at(recordNumber);
    vector<ElementValue *> *copyOfCollectionOfElementValue = new vector<ElementValue *>(originalVectorOfElementValue->size());

    std::copy(originalVectorOfElementValue->begin(), originalVectorOfElementValue->end(), copyOfCollectionOfElementValue->begin());

    return copyOfCollectionOfElementValue;
}

ElementValue *RecordCollection::getElementValueOfRecord(unsigned int recordNumber, string elementName) {
    vector<string *> *elementsNameOfRecord = collectionOfElementName->at(recordNumber);
    vector<ElementValue *> *elementsValueOfRecord = collectionOfElementValue->at(recordNumber);

    vector<string *>::iterator it_begin = elementsNameOfRecord->begin();
    vector<string *>::iterator it;
    int indexOfElementValue = 0;

    for (it = it_begin; it != elementsNameOfRecord->end(); it++) {
        if (*(*it) == elementName) {
            indexOfElementValue = it - it_begin;
            return elementsValueOfRecord->at(indexOfElementValue);
        }
    }
    return nullptr;
}

// RAW RECORD
vector<unsigned char *> *RecordCollection::getCollectionOfRawRecord() {
    return collectionOfRawRecord;
}

void RecordCollection::setCollectionOfRawRecord(vector<unsigned char *> *_collectionOfRawRecord) {
    collectionOfRawRecord = _collectionOfRawRecord;
}

unsigned char *RecordCollection::getRawRecord(unsigned int recordNumber) {
    return collectionOfRawRecord->at(recordNumber);
}

string RecordCollection::getRawRecordInString(unsigned int recordNumber) {
    unsigned int sizeOfRecord = collectionOfRawRecordLength->at(recordNumber);
    unsigned char *rawRecord = collectionOfRawRecord->at(recordNumber);

    stringstream rawRecordString;

    for (unsigned int i = 0; i < sizeOfRecord; i++) {
        rawRecordString << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rawRecord[i]);
    }

    return rawRecordString.str();
}

// RAW RECORD LENTH
vector<unsigned int> *RecordCollection::getCollectionOfRawRecordLength() {
    return collectionOfRawRecordLength;
}

void RecordCollection::setCollectionOfRawRecordLength(vector<unsigned int> *_collectionOfRawRecordLength) {
    collectionOfRawRecordLength = _collectionOfRawRecordLength;
}

// CATEGORY NUMBER
vector<unsigned short int>* RecordCollection::getCollectionOfRecordCategoryNumber() {
    return collectionOfRecordCategoryNumber;
}

void
RecordCollection::setCollectionOfRecordCategoryNumber(vector<unsigned short int>* _collectionOfRecordCategoryNumber) {
    collectionOfRecordCategoryNumber = _collectionOfRecordCategoryNumber;
}

unsigned short int RecordCollection::getCategoryNumberOfARecord(unsigned short int recordNumber) {
    return collectionOfRecordCategoryNumber->at(recordNumber);
}

// OTHER FUNCTIONS
void RecordCollection::addRecordsInCollection(RecordCollection *recordsToAdd) {
    vector<vector<string *> *> *otherCollectionOfElementName = recordsToAdd->collectionOfElementName;
    collectionOfElementName->insert(collectionOfElementName->end(), otherCollectionOfElementName->begin(),
                                    otherCollectionOfElementName->end());


    vector<vector<ElementValue *> *> *otherCollectionOfElementValue = recordsToAdd->collectionOfElementValue;
    collectionOfElementValue->insert(collectionOfElementValue->end(), otherCollectionOfElementValue->begin(),
                                     otherCollectionOfElementValue->end());


    vector<unsigned char *> *otherCollectionOfRawRecord = recordsToAdd->getCollectionOfRawRecord();
    collectionOfRawRecord->insert(collectionOfRawRecord->end(), otherCollectionOfRawRecord->begin(), otherCollectionOfRawRecord->end());

    vector<unsigned int> *otherCollectionOfRawRecordLength = recordsToAdd->getCollectionOfRawRecordLength();
    collectionOfRawRecordLength->insert(collectionOfRawRecordLength->end(), otherCollectionOfRawRecordLength->begin(),
                                        otherCollectionOfRawRecordLength->end());

    vector<unsigned short int>* otherCollectionOfRecordCategoryNumber = recordsToAdd->getCollectionOfRecordCategoryNumber();
    collectionOfRecordCategoryNumber->insert(collectionOfRecordCategoryNumber->end(), otherCollectionOfRecordCategoryNumber->begin(),
                                             otherCollectionOfRecordCategoryNumber->end());

    if (otherCollectionOfElementName != nullptr)
        delete otherCollectionOfElementName;

    if (otherCollectionOfElementValue != nullptr)
        delete otherCollectionOfElementValue;

    if (otherCollectionOfRawRecord != nullptr)
        delete otherCollectionOfRawRecord;

    if (otherCollectionOfRawRecordLength != nullptr)
        delete otherCollectionOfRawRecordLength;

    if (otherCollectionOfRecordCategoryNumber != nullptr)
        delete otherCollectionOfRecordCategoryNumber;
}

int RecordCollection::getIndexOfElementValueOfRecord(unsigned int recordNumber, string elementName) {
    vector<string *> *elementsNameOfRecord = collectionOfElementName->at(recordNumber);
    vector<string *>::iterator it_begin = elementsNameOfRecord->begin();
    vector<string *>::iterator it;

    for (it = it_begin; it != elementsNameOfRecord->end(); it++) {
        if (*(*it) == elementName) {
            return it - it_begin;
        }
    }
    return -1;
}

unsigned int RecordCollection::getTotalNumberOfRecords() {
    return collectionOfElementName->size();
}
