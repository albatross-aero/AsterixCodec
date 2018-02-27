#ifndef FACADE_RECORDCOLLECTION_H_
#define FACADE_RECORDCOLLECTION_H_

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iterator>

#include "asterix_codec/facade/reports/FailureReport.h"
#include "data_model/decoder_objects/ElementValue.hpp"
#include "Record.h"

using namespace std;

/**
 *\brief
 *Provides access to ASTERIX Records.<br>
 *
 *This class contains five vectors:
 * 	- vector<vector<string*>*>*: the external vector collects the Records; the internal vector contains names of Elements.
 *
 * 	- vector<vector<ElementValue*>*>*: the external vector collects the Records, the internal vector contains values of the Elements
 *
 * 	- vector<unsigned char*>*: the vector contains the raw data for each Record
 *
 * 	- vector<unsigned int>*: the vector contains the length in byte of each raw Record
 *
 * 	- vector<unsigned int>*: the vector contains the category number of each Record
 *
 *<br><br>
 *
 *To access a complete Record information:<br>
 * 	- The user should provide an index 'i' (to select the i-th Record):
 *
 * 		- getCollectionOfRawRecord()[i], getCollectionOfRawRecordLength()[i], getCollectionOfElementName()[i], getCollectionOfElementValue()[i], getCollectionOfRecordCategoryNumber()[i] ->
 * 		represent information related to a Record with its raw value, its category number, the length in byte of the raw value, two collections with
 * 		the Element names and the Element values
 *
 * 	- The user should provide two indexes 'i'(for Record) and 'j'(for element)
 *
 * 		- getElementNamesOfARecord(i)[j], getElementValuesOfARecord(i)[j] -> represent the name and the value of a Element in a Record
 */

class RecordCollection {

public:

    /**
     * Default constructor.<br>
     */
    RecordCollection();

    /**
     * Destructor.
     */
    ~RecordCollection();


    // ELEMENT NAME
    /**
     * Sets the collection of Element name of all Record.
     * @param _collectionOfElementName Collection of Element name
     */
    void setCollectionOfElementName(vector<vector<string *> *> *_collectionOfElementName);

    /**
     * Gets a vector of Element names for a given Record.
     * @param recordNumber Index to point to a Record
     * @return Collection of Element names
     */
    vector<string *> *getElementNamesOfARecord(unsigned int recordNumber);

    /**
     * Makes a copy of the vector of Element names for a given Record.
     * @param recordNumber Index to point to a Record
     * @return Copy of the collection of Element name
     */
    vector<string *> *makeACopyOfRecordElementNames(unsigned int recordNumber);


    // ELEMENT VALUE
    /**
     * Sets the collection of Element value for all Record.
     * @param _collectionOfElementValue Collection of Element value
     */
    void setCollectionOfElementValue(vector<vector<ElementValue *> *> *_collectionOfElementValue);

    /**
     * Gets a vector of Element values for a given Record.
     * @param recordNumber Index to point to a Record
     * @return Collection of Element values
     */
    vector<ElementValue *> *getElementValuesOfARecord(unsigned int recordNumber);

    /**
     * Makes a copy of the vector of Element values for a given Record.
     * @param recordNumber Index to point to a Record
     * @return Copy of the collection of Element value
     */
    vector<ElementValue *> *makeACopyOfRecordElementValues(unsigned int recordNumber);

    /**
     * Gets the Element value inside a Record given the index of the Record and the Element name to be searched.
     * @param recordNumber Index to point to a Record
     * @param elementName Name of the element
     * @return Pointer to an ElementValue
     */
    ElementValue *getElementValueOfRecord(unsigned int recordNumber, string elementName);


    // RAW RECORD
    /**
     * Gets the collection of raw data for all Records.
     * @return The collection of raw data
     */
    vector<unsigned char *> *getCollectionOfRawRecord();

    /**
     * Sets the collection of raw data for all Record.
     * @param _collectionOfRawRecord Collection of raw data
     */
    void setCollectionOfRawRecord(vector<unsigned char *> *_collectionOfRawRecord);

    /**
     * Gets a single Record for a given index.
     * @param recordNumber Index to point to a Record
     * @return Pointer to the raw data of the Record
     */
    unsigned char *getRawRecord(unsigned int recordNumber);

    /**
     * Gets a single Record for a given index in string format.
     * @param recordNumber Index to point to a Record
     * @return String representing the raw data of the Record
     */
    string getRawRecordInString(unsigned int recordNumber);


    // RAW RECORD LENTH
    /**
     * Gets the collection of length in bytes of the raw Records.
     * @return Collection of all length in bytes
     */
    vector<unsigned int> *getCollectionOfRawRecordLength();

    /**
     * Sets the collection of length in bytes of the raw Records.
     * @param _collectionOfRawRecordLength Collection of all length in bytes
     */
    void setCollectionOfRawRecordLength(vector<unsigned int> *_collectionOfRawRecordLength);


    // CATEGORY NUMBER
    /**
     * Gets the collection of category number of the Records.
     * @return Collection of all category numbers
     */
    vector<unsigned short int>* getCollectionOfRecordCategoryNumber();

    /**
     * Sets the collection of category numbers of the raw Records.
     * @param _collectionOfRecordCategoryNumber Collection of all category numbers
     */
    void setCollectionOfRecordCategoryNumber(vector<unsigned short int>* _collectionOfRecordCategoryNumber);

    /**
     * Gets a category number for a given index.
     * @param recordNumber Index to point to a Record
     * @return Category number of the Record
     */
    unsigned short int getCategoryNumberOfARecord(unsigned short int recordNumber);


    // OTHER FUNCTIONS
    /**
     * Adds Records in this object.
     * @param recordsToAdd Object from witch results are copied
     */
    void addRecordsInCollection(RecordCollection *recordsToAdd);

    /**
     * Gets the index in collectionOfElementValue vector of an Element inside a Record given the index of the Record and the name of the Element to be searched.
     * @param recordNumber Index to point to a Record
     * @param elementName Name of the element
     * @return Value of the index in collectionOfElementValue vector
     */
    int getIndexOfElementValueOfRecord(unsigned int recordNumber, string elementName);

    /**
     * Gives the number of Records.
     * @return Number of Records
     */
    unsigned int getTotalNumberOfRecords();

private:

    /**
     * Collection of Record with their Element names.
     */
    vector<vector<string *> *> *collectionOfElementName;    // this class is not responsible for the deallocation of the string*

    /**
     * Collection of Record with their Element values.
     */
    vector<vector<ElementValue *> *> *collectionOfElementValue;    // this class is not responsible for the deallocation of the ElementValue*

    /**
     * Collection of raw data of all Record.
     */
    vector<unsigned char *> *collectionOfRawRecord;    // this class is not responsible for the deallocation of the unsigned char*

    /**
     * Collection of raw data length of all Record.
     */
    vector<unsigned int>* collectionOfRawRecordLength;

    /**
     * Collection of Record number.
     */
    vector<unsigned short int>* collectionOfRecordCategoryNumber;
};

#endif /* FACADE_RECORDCOLLECTION_H_ */
