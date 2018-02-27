#include "utils/ElementEncodingUtils.h"

CommonVariables *ElementEncodingUtils::commonVariables = CommonVariables::getInstance();
FailureReport *ElementEncodingUtils::failureReport = FailureReport::getInstance();

unsigned char *ElementEncodingUtils::getRawElement(long long integerToEncode, int lengthInBit) {
    int noBytes = lengthInBit / 8;
    if (lengthInBit % 8 != 0) {
        noBytes++;
    }

    /*
     * The 'encodedResult' size is intentionally one byte longer than needed (look at that "+1" in the array allocation).
     * This is needed while building an Item, by linking different Elements, not starting at the very beginning of a Byte.
     * With this spare byte, that function may shift an Element to the right, without loosing data.
     */

    unsigned char *encodedResult = new unsigned char[noBytes + 1];
    encodedResult[noBytes] = 0x0;
    unsigned char mask[noBytes];

    for (int i = noBytes - 1; i >= 0; i--) {
        mask[i] = 0xFF;    // Set the mask to do not filter anything

        encodedResult[i] = (unsigned char) (integerToEncode & mask[i]);    // Bitwise AND

        if (i != 0) {
            integerToEncode >>= 8;    // Bitwise right SHIFT
        }
    }

    // Align the resulting unsigned char* 'encodedResult' to the left of the most significant byte
    // Example: noBytes=2, lengthInBit=12, encodedResult=09 a3 -> encodedResult=9a 30
    if ((noBytes * 8) > lengthInBit) {
        int shiftInBit = (noBytes * 8) - lengthInBit;
        BitUtils::shiftLeft(encodedResult, noBytes, shiftInBit);
    }

    return encodedResult;
}

void ElementEncodingUtils::checkValidityRange(string *elementName, double elementValue, Range *elementRange, unsigned int categoryNumber) {
    /************************************************************************
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
     ************************************************************************/
    bool isInsideRange = true;
    // RANGE check
    if (!elementRange->isEmpty) {    // Check if a validation range is set for the current Element

        if (!elementRange->isMinIncluded) {
            if (!elementRange->isMaxIncluded) {
                // CASE A ||| LOWER and UPPER excluded
                if (!((elementValue > elementRange->minValue) && (elementValue < elementRange->maxValue))) {
                    // Element value out of range
                    isInsideRange = false;    // OK
                }
            } else {    // Lower bound NOT included
                // CASE B ||| LOWER escluso && UPPER incluso
                if (!((elementValue > elementRange->minValue) && (elementValue <= elementRange->maxValue))) {
                    // Element value out of range
                    isInsideRange = false;
                }
            }
        } else if (!elementRange->isMaxIncluded) {    // Upper bound NOT included
            // CASE C ||| LOWER incluso & UPPER escluso
            if (!((elementValue >= elementRange->minValue) && (elementValue < elementRange->maxValue))) {
                // Element value out of range
                isInsideRange = false;
            }
        } else {
            // CASE D ||| LOWER incluso & UPPER incluso
            if (!((elementValue >= elementRange->minValue) && (elementValue <= elementRange->maxValue))) {
                // Element value out of range
                isInsideRange = false;
            }
        }
    }

    if (isInsideRange == false) {
        // The library does not stop, decoding may proceed.
        // An error is is logged in the Report object.
        string message =
                "Out of range Element value '" + (*elementName) + "': it is " + to_string(elementValue) + ", but it was expected to be between "
                + to_string(elementRange->minValue) + " and " + to_string(elementRange->maxValue) + ".";
        failureReport->addFailure(categoryNumber, 0, (*elementName), nullptr, message, ReturnCodes::OUT_OF_RANGE_ENCODED_ELEMENT);
    }
}

unsigned char *
ElementEncodingUtils::encodeDecimal(ElementValue *element, Range *validityRange, int lengthInBit, double multFactor, string *elementName,
                                    unsigned int categoryNumber) {
    int decodedValue = 0;

    if (SignedInteger8bitValue *integerValue = dynamic_cast<SignedInteger8bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (SignedInteger16bitValue *integerValue = dynamic_cast<SignedInteger16bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (SignedInteger32bitValue *integerValue = dynamic_cast<SignedInteger32bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (SignedInteger64bitValue *integerValue = dynamic_cast<SignedInteger64bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (UnsignedInteger8bitValue *integerValue = dynamic_cast<UnsignedInteger8bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (UnsignedInteger16bitValue *integerValue = dynamic_cast<UnsignedInteger16bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (UnsignedInteger32bitValue *integerValue = dynamic_cast<UnsignedInteger32bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (UnsignedInteger64bitValue *integerValue = dynamic_cast<UnsignedInteger64bitValue *>(element)) {
        decodedValue = integerValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, decodedValue, validityRange, categoryNumber);

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }

    } else if (FloatValue *floatValue = dynamic_cast<FloatValue *>(element)) {
        float originalValue = floatValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, originalValue, validityRange, categoryNumber);

        // Multiplication factor applied if different from 0
        if (multFactor != 0) {
            double toBeRounded = originalValue / multFactor;
            decodedValue = round(toBeRounded);
        }
    } else if (DoubleValue *doubleValue = dynamic_cast<DoubleValue *>(element)) {
        double originalValue = doubleValue->getDecodedValue();

        // Check validity range
        checkValidityRange(elementName, originalValue, validityRange, categoryNumber);

        // Multiplication factor applied if different from 0
        if (multFactor != 0) {
            double toBeRounded = originalValue / multFactor;
            decodedValue = round(toBeRounded);
        }
    } else if (StringValue *stringValue = dynamic_cast<StringValue *>(element)) {
        string originalValue = stringValue->getDecodedValue();
        decodedValue = stoi(originalValue);

        // No validity range in case of StringValue

        // Multiplication factor applied, if different from 0
        if (multFactor != 0) {
            double toBeRounded = (double) decodedValue / multFactor;
            decodedValue = round(toBeRounded);
        }
    } else {    // Default
        return nullptr;
    }

    return getRawElement(decodedValue, lengthInBit);
}

unsigned char *
ElementEncodingUtils::encodeDecimalToCA2(ElementValue *element, Range *validityRange, int lengthInBit, double multFactor, string *elementName,
                                         unsigned int categoryNumber) {
    return encodeDecimal(element, validityRange, lengthInBit, multFactor, elementName, categoryNumber);
}

unsigned char *ElementEncodingUtils::encodeAscii(ElementValue *element, int lengthInBit) {
    if (StringValue *stringValue = dynamic_cast<StringValue *>(element)) {

        int noBytes = lengthInBit / 8;
        if (lengthInBit % 8 != 0) {
            noBytes++;
        }

        string originalValue = stringValue->getDecodedDataInString();
        unsigned char *cstr = new unsigned char[noBytes];
        memcpy(cstr, originalValue.c_str(), (noBytes));

        return cstr;

    } else {
        return nullptr;
    }
}

unsigned char *ElementEncodingUtils::encodeTargetId(ElementValue *element, int lengthInBit) {
    vector<char> *icaoAlphabet = commonVariables->getIcaoAlphabet();

    if (StringValue *stringValue = dynamic_cast<StringValue *>(element)) {
        string originalValue = stringValue->getDecodedValue();
        short noTargetIdChars = 8;

        int noBytes = lengthInBit / 8;

        unsigned char *rawTargetId = new unsigned char[noBytes + 1];
        rawTargetId[noBytes] = 0x0;
        memset(rawTargetId, 0, noBytes);

        for (int i = 0; i < noTargetIdChars; i++) {
            char charToSearch = originalValue.at(i);    // Here we extract the CHAR to be searched inside the ICAO Alphabet vector
            auto index = distance(icaoAlphabet->begin(), find(icaoAlphabet->begin(), icaoAlphabet->end(), charToSearch));
            BitUtils::shiftLeft(rawTargetId, noBytes, 6);
            rawTargetId[noBytes - 1] |= index;
        }

        return rawTargetId;

    } else {
        return nullptr;
    }
}

unsigned char *ElementEncodingUtils::encodeMBData(ElementValue *element, int lengthInBit) {
    if (StringValue *stringValue = dynamic_cast<StringValue *>(element)) {
        long originalValue = 0;
        stringstream ss;
        ss << std::hex << stringValue->getDecodedValue();    // Reinterpret the string as hexadecimal
        ss >> originalValue;    // Flush the hexadecimal value to a long int
        return getRawElement(originalValue, lengthInBit);
    } else {
        return nullptr;
    }
}
