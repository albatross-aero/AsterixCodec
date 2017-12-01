#include <config/CommonVariables.h>   // It is a way to allow that two classes refer to each other
#include "ExternalItem.h"

ExternalItem::ExternalItem() {
	lengthInByte = 0;
}

vector<vector<string> > ExternalItem::getElementDescriptions() {
	return elementDescriptions;
}

vector<vector<string>>* ExternalItem::getPointerToElementDescriptions() {
	return &elementDescriptions;
}

void ExternalItem::setElementDescriptions(vector<vector<string> > elementDescriptions) {
	this->elementDescriptions = elementDescriptions;
}

unsigned int ExternalItem::getLengthInByte() {
	return lengthInByte;
}

void ExternalItem::setLengthInByte(unsigned int lengthInByte) {
	this->lengthInByte = lengthInByte;
}

string ExternalItem::getName() {
	return name;
}

string* ExternalItem::getPointerToName() {
	return &name;
}

void ExternalItem::setName(string name) {
	this->name = name;
}

ItemTypeEnum ExternalItem::getType() {
	return type;
}

void ExternalItem::setType(ItemTypeEnum type) {
	this->type = type;
}
