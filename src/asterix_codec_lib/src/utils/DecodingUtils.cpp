#include "utils/DecodingUtils.h"

CommonVariables *DecodingUtils::commonVariables = CommonVariables::getInstance();
FailureReport *DecodingUtils::failureReport = FailureReport::getInstance();

bool DecodingUtils::checkValidityRange(Range *currentElementRange, double currentElementValue) {
    /**
     * **********************************************************************
     * In the graph below, VALID and INVALID values are plotted.            *
     * Note that 'O' means INCLUDED, while 'X' means EXCLUDED.				*
     *																		*
     * 						MIN						MAX						*
     * 						 |						 |						*
     * 			    INVALID  |		            	 |  INVALID				*
     * 	case A	  ===========O						 O===========			*
     * 						 |		   VALID		 |						*
     * 	case B	  ===========O						 X===========			*
     * 						 |		   				 |						*
     * 	case C	  ===========X						 O===========			*
     * 						 |		   VALID		 |						*
     * 	case D	  ===========X						 X===========			*
     * 						 |						 |						*
     ************************************************************************
     */
    bool isInsideRange = true;
    // RANGE check
    if (!currentElementRange->isEmpty) {    // Check if a validation range is set for the current Element

        if (!currentElementRange->isMinIncluded) {
            if (!currentElementRange->isMaxIncluded) {    // Lower and upper bound NOT included
                // CASE A ||| LOWER and UPPER excluded
                if (!((currentElementValue > currentElementRange->minValue) &&
                      (currentElementValue < currentElementRange->maxValue))) {
                    // Element value out of range
                    isInsideRange = false;
                }
            } else {    // Lower bound NOT included
                // CASE B ||| LOWER escluso && UPPER incluso
                if (!((currentElementValue > currentElementRange->minValue) &&
                      (currentElementValue <= currentElementRange->maxValue))) {
                    // Element value out of range
                    isInsideRange = false;
                }
            }
        } else if (!currentElementRange->isMaxIncluded) {    // Upper bound NOT included
            // CASE C ||| LOWER incluso & UPPER escluso
            if (!((currentElementValue >= currentElementRange->minValue) &&
                  (currentElementValue < currentElementRange->maxValue))) {
                // Element value out of range
                isInsideRange = false;
            }
        } else {
            // CASE D ||| LOWER incluso & UPPER incluso
            if (!((currentElementValue >= currentElementRange->minValue) &&
                  (currentElementValue <= currentElementRange->maxValue))) {
                // Element value out of range
                isInsideRange = false;
            }
        }
    }
    return isInsideRange;
}

ElementValue *
DecodingUtils::toDecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                         RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber,
                         string *elementName,
                         DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {

    ElementValue *elementValue = nullptr;
    int intResult = 0;
    double returnValue = 0;
    bool isInsideRange = true;

    int noBytes = (*elementLengthInBit) / 8;
    if ((*elementLengthInBit) % 8 != 0) {
        noBytes++;
    }

    for (int i = 0; i < noBytes; i++) {
        intResult |= (int) hexValue[i];        // Bitwise OR
        if (i != noBytes - 1) {
            intResult <<= 8;    // Bitwise left SHIFT
        }
    }

    returnValue = intResult * (*multiplicationFactorToDecode);

    // Range check
    isInsideRange = checkValidityRange(validRange, returnValue);

    if (!isInsideRange) {
        // The library does not stop, decoding can continue. Only an error is logged in the Report object.
        string message =
                "Element value '" + (*elementName) + "' out of range: it is " + to_string(returnValue) +
                ", but it was expected to be between "
                + to_string(validRange->minValue) + " and " + to_string(validRange->maxValue) + ".";
        failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                  ReturnCodes::OUT_OF_RANGE_DECODED_ELEMENT);
    }

    // Set the resulted decoded value
    switch (*representationMode) {
        case RepresentationModeEnum::Unsigned8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger8bitValueFromOP();
            ((UnsignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Unsigned16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger16bitValueFromOP();
            ((UnsignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Unsigned32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger32bitValueFromOP();
            ((UnsignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Unsigned64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger64bitValueFromOP();
            ((UnsignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger8bitValueFromOP();
            ((SignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger16bitValueFromOP();
            ((SignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger32bitValueFromOP();
            ((SignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger64bitValueFromOP();
            ((SignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::DoubleValue:
            elementValue = decodedValueObjectPoolManager->getDoubleValueFromOP();
            ((DoubleValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::FloatValue:
            elementValue = decodedValueObjectPoolManager->getFloatValueFromOP();
            ((FloatValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::StringValue:
            elementValue = decodedValueObjectPoolManager->getStringValueFromOP();
            ((StringValue *) elementValue)->setDecodedValue(to_string(returnValue));
            break;

        default:
            // Unknown function to decode the element
            string message =
                    "The RepresentationMode given for the current Element '" + (*elementName) + "' is not permitted!";
            failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                      ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
            break;
    }

    return elementValue;
}

ElementValue *
DecodingUtils::toHexadecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                             RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber,
                             string *elementName,
                             DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {

    ElementValue *elementValue = nullptr;
    int intResult = 0;
    double returnValue = 0;
    bool isInsideRange = true;

    int noBytes = (*elementLengthInBit) / 8;
    if ((*elementLengthInBit) % 8 != 0) {
        noBytes++;
    }

    for (int i = 0; i < noBytes; i++) {
        intResult |= (int) hexValue[i];        // Bitwise OR
        if (i != noBytes - 1) {
            intResult <<= 8;    // Bitwise left SHIFT
        }
    }

    returnValue = intResult * (*multiplicationFactorToDecode);

    // Range check
    isInsideRange = checkValidityRange(validRange, returnValue);

    if (!isInsideRange) {
        // The library does not stop, decoding can continue. Only an error is logged in the Report object.
        string message =
                "Element value '" + (*elementName) + "' out of range: it is " + to_string(returnValue) +
                ", but it was expected to be between "
                + to_string(validRange->minValue) + " and " + to_string(validRange->maxValue) + ".";
        failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                  ReturnCodes::OUT_OF_RANGE_DECODED_ELEMENT);
    }

    switch (*representationMode) {
        case RepresentationModeEnum::Unsigned8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger8bitValueFromOP();
            ((UnsignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                         RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Unsigned16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger16bitValueFromOP();
            ((UnsignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                          RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Unsigned32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger32bitValueFromOP();
            ((UnsignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                          RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Unsigned64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger64bitValueFromOP();
            ((UnsignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                          RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger8bitValueFromOP();
            ((SignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                       RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger16bitValueFromOP();
            ((SignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                        RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger32bitValueFromOP();
            ((SignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                        RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger64bitValueFromOP();
            ((SignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                        RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::DoubleValue:
            elementValue = decodedValueObjectPoolManager->getDoubleValueFromOP();
            ((DoubleValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::FloatValue:
            elementValue = decodedValueObjectPoolManager->getFloatValueFromOP();
            ((FloatValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::StringValue:
            elementValue = decodedValueObjectPoolManager->getStringValueFromOP();
            ((StringValue *) elementValue)->setDecodedValue(to_string(returnValue));
            break;

        default:
            // Unknown function to decode the element
            string message =
                    "The RepresentationMode given for the current Element '" + (*elementName) + "' is not permitted!";
            failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                      ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
            break;
    }
    return elementValue;
}

ElementValue *
DecodingUtils::CA2toDecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                            RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber,
                            string *elementName,
                            DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {
    ElementValue *elementValue = nullptr;
    int CA2version = 0, flippedVersion = 0;
    double returnValue = 0;
    bool isInsideRange = true;

    int noBytes = (*elementLengthInBit) / 8;
    if ((*elementLengthInBit) % 8 != 0)
        noBytes++;

    for (int i = 0; i < noBytes; i++) {
        CA2version |= (int) hexValue[i];    // Bitwise OR
        if (i != noBytes - 1) {
            CA2version <<= 8;    // Bitwise left SHIFT
        }
    }

    /*
     *  ++++++++++++++++++++++++++
     *  +       EXAMPLE #1       +
     *  ++++++++++++++++++++++++++
     * 13 bit Element -- expressed over 2 BYTES
     *        {  Element's bit   }
     * 		  xxx1 1100  |  1100 1001
     *  bit#     5 4321  |  ---- ----
     *    (what we keep) | (what we ignore)
     *
     * We are always working on the 8 most significant bits (the LEFT-most)!
     *
     * 13 (elementLenghInBit) -
     *  8 (what we ignore)    =
     *  5  <---- this is the index we are going to use to access the LEFT-MOST_SIGNIFICANT_BIT of the highest Element byte
     *
     *  ++++++++++++++++++++++++++
     *  +       EXAMPLE #2       +
     *  ++++++++++++++++++++++++++
     *  19 bit Element -- expressed over 3 BYTES
     *      {      Element's bit        }
     * 		  xxxx x110  |  1101 0010 | 1001 1111
     *  bit#        321  |  ---- ----   ---- ----
     *    (what we keep) | (   what we ignore    )
     *
     * We are always working on the 8 most significant bits (the LEFT-most)!
     *
     * 19 (elementLengthInBit) -
     * 16  (what we ignore)    =
     * 3  <---- this is the index we are going to use to access the LEFT-MOST_SIGNIFICANT_BIT of the highest Element byes
     */

    short int bitIndexToCheckWithMask = -1;
    // unsigned short int noBits = noBytes * 8;
    bitIndexToCheckWithMask = *elementLengthInBit - ((noBytes - 1) * 8);

    char *mask = new char[8];
    memset(mask, 0, 1);
    mask[0] = 1;    // MSB of the mask set to 1;
    while (bitIndexToCheckWithMask > 1) {
        *mask <<= 1;
        bitIndexToCheckWithMask--;
    }
    if (hexValue[0] & *mask) {
        // NEGATIVE value
        flippedVersion = -(~(CA2version) & ((1 << (*elementLengthInBit)) - 1)) - 1;
    } else {
        // POSITIVE value
        flippedVersion = CA2version;
    }

    returnValue = flippedVersion * (*multiplicationFactorToDecode);

    // Range check
    isInsideRange = checkValidityRange(validRange, returnValue);

    if (!isInsideRange) {
        // The library does not stop, decoding can continue. Only an error is logged in the Report object.
        string message =
                "Element value '" + (*elementName) + "' out of range: it is " + to_string(returnValue) +
                ", but it was expected to be between "
                + to_string(validRange->minValue) + " and " + to_string(validRange->maxValue) + ".";
        failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                  ReturnCodes::OUT_OF_RANGE_DECODED_ELEMENT);
    }

    switch (*representationMode) {
        case RepresentationModeEnum::Unsigned8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger8bitValueFromOP();
            ((UnsignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Unsigned16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger16bitValueFromOP();
            ((UnsignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Unsigned32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger32bitValueFromOP();
            ((UnsignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Unsigned64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger64bitValueFromOP();
            ((UnsignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger8bitValueFromOP();
            ((SignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger16bitValueFromOP();
            ((SignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger32bitValueFromOP();
            ((SignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::Signed64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger64bitValueFromOP();
            ((SignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::DoubleValue:
            elementValue = decodedValueObjectPoolManager->getDoubleValueFromOP();
            ((DoubleValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::FloatValue:
            elementValue = decodedValueObjectPoolManager->getFloatValueFromOP();
            ((FloatValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::StringValue:
            elementValue = decodedValueObjectPoolManager->getStringValueFromOP();
            ((StringValue *) elementValue)->setDecodedValue(to_string(returnValue));
            break;

        default:
            // Unknown function for decoding the element
            string message =
                    "The RepresentationMode given for the current Element '" + (*elementName) + "' is not permitted!";
            failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                      ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
            break;
    }

    return elementValue;
}

ElementValue *
DecodingUtils::CA2toHexadecimal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                                RepresentationModeEnum *representationMode, Range *validRange,
                                unsigned int categoryNumber,
                                string *elementName,
                                DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {
    ElementValue *elementValue = nullptr;
    int CA2version = 0, flippedVersion = 0;
    double returnValue = 0;
    bool isInsideRange = true;

    int noBytes = (*elementLengthInBit) / 8;
    if ((*elementLengthInBit) % 8 != 0)
        noBytes++;

    for (int i = 0; i < noBytes; i++) {
        CA2version |= (int) hexValue[i];    // Bitwise OR
        if (i != noBytes - 1) {
            CA2version <<= 8;    // Bitwise left SHIFT
        }
    }

    if (hexValue[0] & 0x80) {
        // NEGATIVE value
        flippedVersion = -(~(CA2version) & ((1 << (*elementLengthInBit)) - 1)) - 1;
    } else {
        // POSITIVE value
        flippedVersion = CA2version;
    }

    returnValue = flippedVersion * (*multiplicationFactorToDecode);

    // Range check
    isInsideRange = checkValidityRange(validRange, returnValue);
    if (!isInsideRange) {
        // The library does not stop, decoding can continue. Only an error is logged in the Report object.
        string message =
                "Element value '" + (*elementName) + "' out of range: it is " + to_string(returnValue) +
                ", but it was expected to be between "
                + to_string(validRange->minValue) + " and " + to_string(validRange->maxValue) + ".";
        failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                  ReturnCodes::OUT_OF_RANGE_DECODED_ELEMENT);
    }

    switch (*representationMode) {
        case RepresentationModeEnum::Unsigned8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger8bitValueFromOP();
            ((UnsignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                         RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Unsigned16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger16bitValueFromOP();
            ((UnsignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                          RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Unsigned32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getUnsignedInteger32bitValueFromOP();
            ((UnsignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                          RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Unsigned64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger64bitValueFromOP();
            ((UnsignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                          RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed8bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger8bitValueFromOP();
            ((SignedInteger8bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                       RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed16bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger16bitValueFromOP();
            ((SignedInteger16bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                        RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed32bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger32bitValueFromOP();
            ((SignedInteger32bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                        RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::Signed64bitIntegerValue:
            elementValue = decodedValueObjectPoolManager->getSignedInteger64bitValueFromOP();
            ((SignedInteger64bitValue *) elementValue)->setDecodedValue(returnValue,
                                                                        RepresentationModeEnum::HexadecimalValue);
            break;

        case RepresentationModeEnum::DoubleValue:
            elementValue = decodedValueObjectPoolManager->getDoubleValueFromOP();
            ((DoubleValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::FloatValue:
            elementValue = decodedValueObjectPoolManager->getFloatValueFromOP();
            ((FloatValue *) elementValue)->setDecodedValue(returnValue);
            break;

        case RepresentationModeEnum::StringValue:
            elementValue = decodedValueObjectPoolManager->getStringValueFromOP();
            ((StringValue *) elementValue)->setDecodedValue(to_string(returnValue));
            break;

        default:
            // Unknown function to decode the element
            string message =
                    "The RepresentationMode given for the current Element '" + (*elementName) + "' is not permitted!";
            failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                      ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
            break;
    }

    return elementValue;
}

ElementValue *
DecodingUtils::toOctal(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                       RepresentationModeEnum *representationMode, Range *validRange, unsigned int categoryNumber,
                       string *elementName,
                       DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {
    return toDecimal(hexValue, elementLengthInBit, multiplicationFactorToDecode, representationMode, validRange,
                     categoryNumber, elementName,
                     decodedValueObjectPoolManager);
}

ElementValue *
DecodingUtils::toAscii(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                       RepresentationModeEnum *representationMode, unsigned int categoryNumber, string *elementName,
                       DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {
    ElementValue *elementValue = nullptr;
    string ASCIIvalue;

    unsigned int noBytes = (*elementLengthInBit) / 8;

    if ((*elementLengthInBit) % 8 != 0) {
        noBytes++;
    }

    for (unsigned int i = 0; i < noBytes; i++, hexValue++) {
        ASCIIvalue.append((char *) hexValue);
    }

    switch (*representationMode) {
        case RepresentationModeEnum::StringValue:
            elementValue = decodedValueObjectPoolManager->getStringValueFromOP();
            ((StringValue *) elementValue)->setDecodedValue(ASCIIvalue);
            break;
        default:
            // Unknown function for decoding the element
            string message =
                    "The RepresentationMode given for the current Element '" + (*elementName) + "' is not permitted!";
            failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                      ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
            break;
    }

    return elementValue;
}

ElementValue *
DecodingUtils::toTargetId(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                          RepresentationModeEnum *representationMode, unsigned int categoryNumber, string *elementName,
                          DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {

    ElementValue *elementValue = nullptr;
    int shift = -1;
    char *target = new char[8 + 1];
    unsigned char *hexValueCopy = (unsigned char *) malloc(6 * sizeof(char));

    vector<char> *icaoAlphabet = commonVariables->getIcaoAlphabet();

    for (int i = 1; i <= 8; i++) {
        shift = 48 - (6 * i);    // 6*8=48 -> length in bit of the Target ID element
        memcpy(hexValueCopy, hexValue, 6);
        BitUtils::shiftRight(hexValueCopy, 6, shift);
        hexValueCopy[5] &= 0x3f;    // Mask to ignore the 2 most significant bits (ICAO alphabet has 6-bits characters)
        target[i - 1] = icaoAlphabet->at((int) hexValueCopy[5]);
    }
    target[8] = '\0';

    string targetId(target);

    switch (*representationMode) {
        case RepresentationModeEnum::StringValue:
            elementValue = decodedValueObjectPoolManager->getStringValueFromOP();
            ((StringValue *) elementValue)->setDecodedValue(targetId);
            break;

        default:
            // Unknown function for decoding the element
            string message =
                    "The RepresentationMode given for the current Element '" + (*elementName) + "' is not permitted!";
            failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                      ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
            break;
    }

    return elementValue;
}

ElementValue *
DecodingUtils::toMBData(unsigned char *hexValue, int *elementLengthInBit, double *multiplicationFactorToDecode,
                        RepresentationModeEnum *representationMode, unsigned int categoryNumber, string *elementName,
                        DecodedValueObjectPoolManager *decodedValueObjectPoolManager) {

    ElementValue *elementValue = nullptr;
    long long longResult = 0;
    stringstream ss;

    int noBytes = (*elementLengthInBit) / 8;
    if ((*elementLengthInBit) % 8 != 0) {
        noBytes++;
    }

    for (int i = 0; i < noBytes; i++) {
        longResult |= (int) hexValue[i];        // Bitwise OR
        if (i != noBytes - 1) {
            longResult <<= 8;    // Bitwise left SHIFT
        }
    }

    ss << "0x" << std::hex << longResult;

    switch (*representationMode) {
        case RepresentationModeEnum::StringValue:
            elementValue = decodedValueObjectPoolManager->getStringValueFromOP();
            ((StringValue *) elementValue)->setDecodedValue(ss.str());
            break;

        default:
            // Unknown function for decoding the element
            string message =
                    "The RepresentationMode given for the current Element '" + (*elementName) + "' is not permitted!";
            failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message,
                                      ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
            break;
    }
    return elementValue;
}
