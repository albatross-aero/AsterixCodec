#include "ParsedElement.h"

ParsedElement::ParsedElement() {
	parsedValue = nullptr;
	valueLengthInBit = 0;
	itemDescription = nullptr;
	indexInItemVectors = -1;
	parsedValueInString = "";
}

ParsedElement::ParsedElement(unsigned char* _value) {
	parsedValue = _value;
	valueLengthInBit = 0;
	itemDescription = nullptr;
	indexInItemVectors = -1;
}

ParsedElement::~ParsedElement() {
	if (parsedValue != nullptr) {
		delete[] parsedValue;
		parsedValue = nullptr;
	}
	itemDescription = nullptr;
}

std::string ParsedElement::getName() {
	if (itemDescription != nullptr && indexInItemVectors != -1)
		return itemDescription->getAllElementNames()->at(indexInItemVectors);
	else
		return "";
}

unsigned char* ParsedElement::getParsedValue() {
	return parsedValue;
}

void ParsedElement::setParsedValue(unsigned char* _value) {
	parsedValue = _value;
}

std::string ParsedElement::getStringValue() {
	if (parsedValueInString == "") {
		stringstream valueInString;

		if (valueLengthInBit == 0) {
			getValueLengthInBit();
		}

		int nBytes = valueLengthInBit / 8;
		if (valueLengthInBit % 8 != 0)
			nBytes++;

		for (int j = 0; j < nBytes; j++) {
			if (parsedValue) {
				if (((j == nBytes - 1) || (j == 0)) && (valueLengthInBit % 8 != 0))
					valueInString << std::hex << std::setw(1) << std::setfill('0') << static_cast<unsigned int>(parsedValue[j]) << "";
				else
					valueInString << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(parsedValue[j]) << "";
			}
		}
		parsedValueInString = valueInString.str();
	}
	return parsedValueInString;
}

int ParsedElement::getValueLengthInBit() {
	if (valueLengthInBit == 0 && itemDescription != nullptr && indexInItemVectors != -1) {
		valueLengthInBit = itemDescription->getAllElementDescription()->at(indexInItemVectors);
	}
	return valueLengthInBit;
}

void ParsedElement::setValueLenghtInBit(int valueLength) {
	valueLengthInBit = valueLength;
}

Item* ParsedElement::getItemDescription() {
	return itemDescription;
}

void ParsedElement::setItemDescription(Item* _itemInformation) {
	itemDescription = _itemInformation;
}

int ParsedElement::getIndexInItemVectors() {
	return indexInItemVectors;
}

void ParsedElement::setIndexInItemVectors(int _index) {
	indexInItemVectors = _index;
}

void ParsedElement::freeMemory() {
	if (parsedValue != nullptr) {
		delete[] parsedValue;
		parsedValue = nullptr;
	}

	parsedValueInString.clear();

	valueLengthInBit = 0;
	itemDescription = nullptr;
	indexInItemVectors = -1;
}
