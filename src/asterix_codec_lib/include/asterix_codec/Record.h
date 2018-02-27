#ifndef FACADE_RECORD_H_
#define FACADE_RECORD_H_

#include <map>
#include <vector>

#include "config/CommonVariables.h"
#include "asterix_codec/facade/reports/FailureReport.h"
#include "data_model/decoder_objects/ElementValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger8bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger16bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger32bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger64bitValue.h"
#include "data_model/decoder_objects/SignedInteger8bitValue.h"
#include "data_model/decoder_objects/SignedInteger16bitValue.h"
#include "data_model/decoder_objects/SignedInteger32bitValue.h"
#include "data_model/decoder_objects/SignedInteger64bitValue.h"
#include "data_model/decoder_objects/FloatValue.h"
#include "data_model/decoder_objects/DoubleValue.h"
#include "data_model/decoder_objects/StringValue.h"

/**
 *\brief
 *Provides access to an ASTERIX record.<br>
 *
 *This class contains:
 * 	- categoryNumber: the category number of this record
 *
 * 	- elements: map of Elements, the key is the Element name and the value is a pointer to an ElementValue, that contains the value of the Element
 *
 *<br><br>
 *
 *To fill the 'elements' field during the construction of the Record object:<br>
 * 	- The user should provide a vector of pair<string,string> -> the first string is the Element name and second one is the Element value
 *
 */

class Record {
public:

    /**
     *  Default constructor.<br>
     */
    Record();

    /**
     * Constructor.
     * @param _categoryNumber Category number of the Record
     * @param _elements Vector of Elements information - in the pair, the first string is the Element name and second one is the Element value
     */
    Record(unsigned int _categoryNumber, vector<pair<string, string>> _elements);

    /**
     * Gets the category number of the Record.
     * @return Category number of the Record
     */
    unsigned short int getCategoryNumber();

    /**
     * Sets the  category number of the Record.
     * @param _categoryNumber Category number of the Record
     */
    void setCategoryNumber(unsigned short int _categoryNumber);

    /**
     * Gets the information about the Record Elements.
     * @return Pointer to a map that contains the information about the Record Elements
     */
    map<string, ElementValue *> *getElements();

    /**
     * Sets the pointer to a map, that contains the information about the Record Elements.
     * @param elements Pointer to a map that contains the information about the Record Elements
     */
    void setElements(map<string, ElementValue *> elements);

private:
    unsigned short int categoryNumber;
    map<string, ElementValue *> elements;
};

#endif /* FACADE_RECORD_H_ */
