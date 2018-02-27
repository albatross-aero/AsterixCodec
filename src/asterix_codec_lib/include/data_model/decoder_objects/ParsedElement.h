#ifndef THREADING_PARSED_ELEMENT_H
#define THREADING_PARSED_ELEMENT_H

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "data_model/asterix_description/Item.h"

/**
 * Store information of a parsed element.
 */
class ParsedElement {

public:

    ParsedElement();

    ParsedElement(unsigned char *_value);

    ~ParsedElement();

    //GETTERS AND SETTERS
    string getName();

    unsigned char *getParsedValue();

    void setParsedValue(unsigned char *_value);

    string getStringValue();

    int getValueLengthInBit();

    void setValueLenghtInBit(int valueLength);

    Item *getItemDescription();

    void setItemDescription(Item *_itemInformation);

    int getIndexInItemVectors();

    void setIndexInItemVectors(int _index);

    /**
     * Release pointer to the elementValue, set to nullptr 'itemDescription' variable and to -1 'indexInItemVectors' variable
     */
    void freeMemory();

private:

    string parsedValueInString;
    int valueLengthInBit;

    unsigned char *parsedValue;    // used just in the parsing phase, to avoid the instantiation of an 'ElementValue', where the raw and the decoded values will be stored

    Item *itemDescription;        // used to get decoding information for current element - this class is not responsible for the deallocation of the pointer
    int indexInItemVectors;        // used to get information about this element inside 'elementsName' and 'elementsDescription' in Item class
};

#endif //THREADING_PARSED_ELEMENT_H
