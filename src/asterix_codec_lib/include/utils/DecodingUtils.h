#ifndef UTILS_DECODINGUTILS_H_
#define UTILS_DECODINGUTILS_H_

#include "config/CommonVariables.h"
#include "config/enum/RepresentationModeEnum.h"
#include "data_model/decoder_objects/ElementValue.hpp"
#include "asterix_codec/facade/reports/FailureReport.h"
#include "data_model/decoder_object_pool/DecodedValueObjectPoolManager.h"
#include "utils/BitUtils.h"

using namespace std;

class DecodingUtils {
public:

    /**
     * It decodes a raw element transforming it into a decimal value.
     * @param hexValue Binary result of the parsing, input for the decoding function
     * @param elementLengthInBit Expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode Is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode Is the name of the class to be instantiated where the decoding result is going to be stored
     * @param validRange Is a pointer to the valid range allowed for the current Element
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager Is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *toDecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                   RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber, string *elementName,
                                   DecodedValueObjectPoolManager *decodedValueObjectPoolManager);

    /**
     * It decodes a raw element transforming it into a decimal value, to be printed in Hexadecimal representation.
     * @param hexValue Binary result of the parsing, input for the decoding function
     * @param elementLengthInBit Expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode Is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode Is the name of the class to be instantiated where the decoding result is going to be stored
     * @param validRange Is a pointer to the valid range allowed for the current Element
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager Is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *toHexadecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                       RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber,
                                       string *elementName,
                                       DecodedValueObjectPoolManager *decodedValueObjectPoolManager);

    /**
     * It decodes a raw element transforming it into an ASCII string.
     * @param hexValue Binary result of the parsing, input for the decoding function
     * @param elementLengthInBit Expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode Is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode Is the name of the class to be instantiated where the decoding result is going to be stored
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager Is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *toAscii(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                 RepresentationModeEnum *representationMode, unsigned int categoryNumber, string *elementName,
                                 DecodedValueObjectPoolManager *decodedValueObjectPoolManager);


    /**
     * It decodes a raw element expressed in two's complement transforming it into a decimal value.
     * @param hexValue Binary result of the parsing, input for the decoding function
     * @param elementLengthInBit Expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode Is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode Is the name of the class to be instantiated where the decoding result is going to be stored
     * @param validRange Is a pointer to the valid range allowed for the current Element
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager Is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *CA2toDecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                      RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber, string *elementName,
                                      DecodedValueObjectPoolManager *decodedValueObjectPoolManager);

    /**
     * It decodes a raw element expressed in two's complement transforming it into a decimal value, to be printed in Hexadecimal representation.
     * @param hexValue is the binary result of the parsing, input for the decoding function
     * @param elementLengthInBit expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode is the name of the class to be instantiated where the decoding result is going to be stored
     * @param validRange Is a pointer to the valid range allowed for the current Element
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *CA2toHexadecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                          RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber,
                                          string *elementName,
                                          DecodedValueObjectPoolManager *decodedValueObjectPoolManager);

    /**
     * It decodes a raw element transforming it into a decimal value, to be printed in Octal representation.
     * @param hexValue is the binary result of the parsing, input for the decoding function
     * @param elementLengthInBit expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode is the name of the class to be instantiated where the decoding result is going to be stored
     * @param validRange Is a pointer to the valid range allowed for the current Element
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *toOctal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                 RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber, string *elementName,
                                 DecodedValueObjectPoolManager *decodedValueObjectPoolManager);

    /**
     * It decodes the raw element Target Id transforming it into a string based on ICAO alphabet.
     * @param hexValue Binary result of the parsing, input for the decoding function
     * @param elementLengthInBit Expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode Is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode Is the name of the class to be instantiated where the decoding result is going to be stored
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager Is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *toTargetId(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                    RepresentationModeEnum *representationMode, unsigned int categoryNumber, string *elementName,
                                    DecodedValueObjectPoolManager *decodedValueObjectPoolManager);

    /**
     * It decodes the raw element MB Data transforming it into a string.
     * @param hexValue is the binary result of the parsing, input for the decoding function
     * @param elementLengthInBit expresses the parsed Element length in bit
     * @param multiplicationFactorToDecode is the factor by which the decoded value shall be multiplied, in order to represent the right data
     * @param representationMode is the name of the class to be instantiated where the decoding result is going to be stored
     * @param categoryNumber Category number that the current Element belong to
     * @param elementName Pointer to the name of the currentElement
     * @param decodedValueObjectPoolManager is a pointer to the OP where the decoded values are going to be stored
     * @return Pointer to an ElementValue instance, got from the 'decodedValueObjectPool', filled with decoded data
     */
    static ElementValue *toMBData(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                  RepresentationModeEnum *representationMode, unsigned int categoryNumber, string *elementName,
                                  DecodedValueObjectPoolManager *decodedValueObjectPoolManager);


private:

    /**
     * Constructor.<br>
     * This class has only static fields and methods therefore this method is private.
     */
    DecodingUtils();

    /**
     * Destructor.<br>
     * This class has only static fields and methods therefore this method is private.
     */
    ~DecodingUtils();

    /**
     * It checks if the current Element value is included in the validity range coming from the Item description.
     * @param currentElementRange Reference range with lower bound and upper bound of the Element.
     * @param currentElementValue Value to be checked.
     * @return True if the value is valid. Otherwise false.
     */
    static bool checkValidityRange(Range *currentElementRange, double currentElementValue);

    static CommonVariables *commonVariables;

    static DecodedValueObjectPoolManager *decodedValueObjectPoolManager;

    static FailureReport *failureReport;
};

#endif /* UTILS_DECODINGUTILS_H_ */
