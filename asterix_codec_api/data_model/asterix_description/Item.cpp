#include <config/CommonVariables.h>   // It is a way to allow that two classes refer to each other
#include <config/FacadeCommonVariables.h>   // It is a way to allow that two classes refer to each other
#include "Item.h"

FacadeCommonVariables* facadeCommonVariablesForItemClass = FacadeCommonVariables::getInstance();

Item::Item() {
	lengthInByte = 0;
	toBeDecoded = true;
	toBeEncoded = true;
	mandatory = false;
	itemType = ItemTypeEnum::FixedLength;
}

Item::Item(string _name, int _lengthInByte, ItemTypeEnum _itemType) {
	name = _name;
	lengthInByte = _lengthInByte;
	toBeDecoded = false;
	toBeEncoded = false;
	mandatory = false;
	itemType = _itemType;
}

string Item::getName() {
	return name;
}

void Item::setName(string _name) {
	name = _name;
}

bool Item::isItemMandatory() {
	return mandatory;
}

void Item::setItemMandatory(bool _mandatory) {
	mandatory = _mandatory;
}

ItemTypeEnum Item::getItemType() {
	return itemType;
}

void Item::setType(ItemTypeEnum _type) {
	itemType = _type;
}

int Item::getLengthInByte() {
	return lengthInByte;
}

void Item::setLengthInByte(int _length) {
	lengthInByte = _length;
}

vector<string>* Item::getAllElementNames() {
	return &elementNames;
}

void Item::setAllElementNames(vector<string> _elementsName) {
	elementNames = _elementsName;
}

vector<int>* Item::getAllElementDescription() {
	return &elementDescriptions;
}

void Item::setAllElementDescription(vector<int> _elementsLength) {
	elementDescriptions = _elementsLength;
}

vector<string>* Item::getAllFunctionNamesForDecodingInString() {
	return &functionNameForDecodingInString;
}

void Item::setAllFunctionNamesForDecodingInString(vector<string> _functionNameForDecoding) {
	functionNameForDecodingInString = _functionNameForDecoding;
}

vector<string>* Item::getAllRepresentationModesInStringForDecoding() {
	return &representationModeForDecodingInString;
}

void Item::setAllRepresentationModesInStringForDecoding(vector<string> _representationModeForDecoding) {
	representationModeForDecodingInString = _representationModeForDecoding;
}

vector<RepresentationModeEnum>* Item::getAllRepresentationModesInEnumForDecoding() {
	return &representationModeForDecoding;
}

vector<FunctionNameEnum>* Item::getAllFunctionNamesInEnumForDecoding() {
	return &functionNameForDecoding;
}

void Item::setAllRepresentationModesInEnumForDecoding(vector<RepresentationModeEnum> _representationModeForDecoding) {
	representationModeForDecoding = _representationModeForDecoding;
}

void Item::setAllFunctionNamesInEnumForDecoding(vector<FunctionNameEnum> _functionNameForDecoding) {
	functionNameForDecoding = _functionNameForDecoding;
}

vector<vector<double>>* Item::getLsbValues() {
	return &lsbValues;
}

void Item::setAllLsbValues(vector<vector<double>> _lsbValues) {
	lsbValues = _lsbValues;
}

vector<Range>* Item::getAllValidRanges() {
	return &validRanges;
}

void Item::setAllValidRanges(vector<Range> ranges) {
	validRanges = ranges;
}

bool Item::isItemToBeDecoded() {
	// If filtering is NOT enabled -> item is to be decoded
	if (facadeCommonVariablesForItemClass->isFilterDecodingEnabled())
		return toBeDecoded;
	else
		return true;
}

void Item::setToBeDecoded(bool _boolean) {
	this->toBeDecoded = _boolean;
}

bool Item::isItemToBeEncoded() {
	// If filtering is NOT enabled -> item is to be encoded
	if (facadeCommonVariablesForItemClass->isFilterEncodingEnabled())
		return toBeEncoded;
	else
		return true;
}

void Item::setToBeEncoded(bool _boolean) {
	this->toBeEncoded = _boolean;
}

//OTHER FUNCTIONS
int Item::getIndexofElement(string elementName) {
	return (find(elementNames.begin(), elementNames.end(), elementName) - elementNames.begin());
}

string* Item::getElementNameAtIndex(int index) {
	return &elementNames[index];
}

int Item::getElementDescriptionAtIndex(int index) {
	return elementDescriptions[index];
}

string Item::getFunctionNameForDecodingInStringAtIndex(int index) {
	return functionNameForDecodingInString[index];
}

RepresentationModeEnum Item::getRepresentationModeInEnumForDecodingAtIndex(int index) {
	return representationModeForDecoding[index];
}

FunctionNameEnum Item::getFunctionNameInEnumForDecodingAtIndex(int index) {
	return functionNameForDecoding[index];
}

vector<double>* Item::getLsbValuesAtIndex(int index) {
	return &(lsbValues.at(index));
}

Range* Item::getValidRangeAtIndex(int index) {
	return &(validRanges.at(index));
}

int Item::getElementsTotalSize() {
	unsigned int totalSize = 0;
	for (unsigned int i = 0; i < elementDescriptions.size(); i++) {
		totalSize += elementDescriptions[i];
	}
	return totalSize;
}

unsigned short int Item::getNumberOfElementsInTheModule() {
	return numberOfElementsInTheModule;
}

void Item::convertRepresentationModeForDecodingInStringToEnum() {
	unsigned int size = representationModeForDecodingInString.size();
	if (size > 0) {
		for (unsigned int i = 0; i < size; i++) {
			representationModeForDecoding.push_back(ConversionFromStringToEnum::getRepresentationModeEnumFromString(representationModeForDecodingInString[i]));
			if (representationModeForDecoding[i] == RepresentationModeEnum::InvalidValue)
				cerr << "For '" << representationModeForDecodingInString[i] << "' missing the corresponding RepresentationModeEnum." << endl;
		}
	}
}

void Item::convertFunctionNameForDecodingInStringToEnum() {
	unsigned int size = functionNameForDecodingInString.size();
	if (size > 0) {
		for (unsigned int i = 0; i < size; i++) {
			functionNameForDecoding.push_back(ConversionFromStringToEnum::getFunctionNameEnumFromString(functionNameForDecodingInString[i]));
			if (functionNameForDecoding[i] == FunctionNameEnum::InvalidValue)
				cerr << "For '" << functionNameForDecodingInString[i] << "' missing the corresponding FunctionNameEnum." << endl;
		}
	}
}

void Item::createRepetitions() {
	CommonVariables* commonVariables = CommonVariables::getInstance();

	unsigned short int numberOfElementsRepetitionInItemDescription = commonVariables->getNumberOfElementsRepetitionInItemDescription();
	unsigned short int sizeOfFinalVector = 0;

	numberOfElementsInTheModule = elementDescriptions.size();

	sizeOfFinalVector = numberOfElementsInTheModule * numberOfElementsRepetitionInItemDescription;

	addRepetitionInVectors(numberOfElementsInTheModule, sizeOfFinalVector);
}

void Item::addRepetitionInVectors(unsigned short startDimensionOfVector, unsigned short sizeOfFinalVector) {

	string suffix;

	// Vectors resizing in order to accommodate new values
	elementNames.resize(sizeOfFinalVector);
	elementDescriptions.resize(sizeOfFinalVector);
	functionNameForDecoding.resize(sizeOfFinalVector);
	representationModeForDecoding.resize(sizeOfFinalVector);
	lsbValues.resize(sizeOfFinalVector);
	validRanges.resize(sizeOfFinalVector);

	// 'startDimensionOfVector' is the first cell to write the first Element of the first copy ["copy" as Element replica]
	// 'sizeOfFinalVector-1' is the last cell to write the last element of the last copy
	// j += numberOfElementsInTheModule -> is the number of new elements written, that is: numberOfCopyUntilNow * numberOfElementsInTheModule
	for (unsigned short int j = startDimensionOfVector; j < sizeOfFinalVector; j += numberOfElementsInTheModule) {

		suffix = string("_" + to_string(j / numberOfElementsInTheModule));    // j/numberOfElementsInTheModule -> number of repetition

		//For each base elements, I make a copy. The index to write the value is: j + i
		//- j: startDimensionOfVector + (numberOfCopyUntilNow * numberOfElementsInTheModule)
		//- i: number of base element
		for (unsigned short int i = 0; i < numberOfElementsInTheModule; i++) {

			elementDescriptions[j + i] = elementDescriptions[i];
			functionNameForDecoding[j + i] = functionNameForDecoding[i];
			representationModeForDecoding[j + i] = representationModeForDecoding[i];
			lsbValues[j + i] = lsbValues[i];
			validRanges[j + i] = validRanges[i];

			elementNames[j + i] = string(elementNames[i]);
			elementNames[j + i].append(suffix);
		}
	}
}
