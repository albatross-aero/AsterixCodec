#include "CategoryDescription.h"

using namespace std;

FacadeCommonVariables* facadeCommonVariables = FacadeCommonVariables::getInstance();

CategoryDescription::CategoryDescription() {
	toBeDecoded = true;
	toBeEncoded = true;
}

CategoryDescription::CategoryDescription(vector<Item> _itemsDescription, vector<Item> _compoundSubItemsDescription, vector<Item> _refItemDescription,
		vector<Item> _spfItemDescription) {
	itemCollection = _itemsDescription;
	compoundSubItemsDescription = _compoundSubItemsDescription;
	refItemDescription = _refItemDescription;
	spfItemDescription = _spfItemDescription;
	toBeDecoded = true;
	toBeEncoded = true;
}

bool CategoryDescription::isToBeDecoded(){
	// If filtering is NOT enabled -> Category is to be decoded
	if (facadeCommonVariables->isFilterDecodingEnabled())
		return toBeDecoded;
	else
		return true;
}

void CategoryDescription::setToBeDecoded(bool _toBeDecoded){
	toBeDecoded = _toBeDecoded;
}

bool CategoryDescription::isToBeEncoded(){
	// If filtering is NOT enabled -> Category is to be decoded
	if (facadeCommonVariables->isFilterDecodingEnabled())
		return toBeEncoded;
	else
		return true;
}

void CategoryDescription::setToBeEncoded(bool _toBeEncoded){
	toBeEncoded = _toBeEncoded;
}

vector<Item>* CategoryDescription::getItemCollection() {
	return &itemCollection;
}

void CategoryDescription::setItemCollection(vector<Item> _itemsDescription) {
	itemCollection = _itemsDescription;
}

vector<Item>* CategoryDescription::getCompoundSubItemsDescription() {
	return &compoundSubItemsDescription;
}

void CategoryDescription::setCompoundSubItemsDescription(vector<Item> _compoundSubItemsDescription) {
	compoundSubItemsDescription = _compoundSubItemsDescription;
}

vector<Item>* CategoryDescription::getRefItemDescription() {
	return &refItemDescription;
}

void CategoryDescription::setRefItemDescription(vector<Item> _refItemDescription) {
	refItemDescription = _refItemDescription;
}

vector<Item>* CategoryDescription::getSpfItemDescription() {
	return &spfItemDescription;
}

void CategoryDescription::setSpfItemDescription(vector<Item> _spfItemDescription) {
	spfItemDescription = _spfItemDescription;
}

void CategoryDescription::convertInformationFromStringToEnum() {

	for (unsigned int i = 0; i < this->itemCollection.size(); i++) {
		if ((itemCollection.at(i).getItemType() != ItemTypeEnum::CompoundLength) && (itemCollection.at(i).getItemType() != ItemTypeEnum::ExplicitLength)) {

			itemCollection.at(i).convertRepresentationModeForDecodingInStringToEnum();
			itemCollection.at(i).convertFunctionNameForDecodingInStringToEnum();

			if (itemCollection.at(i).getItemType() == ItemTypeEnum::RepetitiveLength)
				itemCollection.at(i).createRepetitions();
		}
	}

	for (unsigned int i = 0; i < this->compoundSubItemsDescription.size(); i++) {
		if ((compoundSubItemsDescription.at(i).getItemType() != ItemTypeEnum::CompoundLength)
				&& (compoundSubItemsDescription.at(i).getItemType() != ItemTypeEnum::ExplicitLength)) {
			compoundSubItemsDescription.at(i).convertFunctionNameForDecodingInStringToEnum();
			compoundSubItemsDescription.at(i).convertRepresentationModeForDecodingInStringToEnum();

			if (compoundSubItemsDescription.at(i).getItemType() == ItemTypeEnum::RepetitiveLength)
				compoundSubItemsDescription.at(i).createRepetitions();
		}
	}

	for (unsigned int i = 0; i < this->refItemDescription.size(); i++) {

		if ((refItemDescription.at(i).getItemType() != ItemTypeEnum::CompoundLength)
				&& (refItemDescription.at(i).getItemType() != ItemTypeEnum::ExplicitLength)) {

			refItemDescription.at(i).convertFunctionNameForDecodingInStringToEnum();
			refItemDescription.at(i).convertRepresentationModeForDecodingInStringToEnum();
			if (refItemDescription.at(i).getItemType() == ItemTypeEnum::RepetitiveLength)

				refItemDescription.at(i).createRepetitions();
		}
	}

	for (unsigned int i = 0; i < this->spfItemDescription.size(); i++) {
		if ((spfItemDescription.at(i).getItemType() != ItemTypeEnum::CompoundLength)
				&& (spfItemDescription.at(i).getItemType() != ItemTypeEnum::ExplicitLength)) {
			spfItemDescription.at(i).convertFunctionNameForDecodingInStringToEnum();
			spfItemDescription.at(i).convertRepresentationModeForDecodingInStringToEnum();

			if (spfItemDescription.at(i).getItemType() == ItemTypeEnum::RepetitiveLength)
				spfItemDescription.at(i).createRepetitions();
		}
	}
}

void CategoryDescription::fillElementsMap() {
	Item* item;

	// For each Items, scan all Elements and save the Element name and the pointer to its Item in the 'elementsMap'

	for (unsigned int i = 0; i < this->itemCollection.size(); i++) {
		item = &(itemCollection.at(i));
		if (item->getItemType() != ItemTypeEnum::CompoundLength && item->getItemType() != ItemTypeEnum::ExplicitLength) {
			for (unsigned int j = 0; j < item->getAllElementNames()->size(); j++) {
				elementsMap.insert(pair<string, Item*>(item->getAllElementNames()->at(j), item));
			}
		}
	}

	for (unsigned int i = 0; i < this->compoundSubItemsDescription.size(); i++) {
		item = &(compoundSubItemsDescription.at(i));
		for (unsigned int j = 0; j < item->getAllElementNames()->size(); j++) {
			elementsMap.insert(pair<string, Item*>(item->getAllElementNames()->at(j), item));
		}
	}

	for (unsigned int i = 0; i < this->refItemDescription.size(); i++) {
		item = &(refItemDescription.at(i));

		if (item->getItemType() != ItemTypeEnum::CompoundLength && item->getItemType() != ItemTypeEnum::ExplicitLength) {
			for (unsigned int j = 0; j < item->getAllElementNames()->size(); j++) {
				elementsMap.insert(pair<string, Item*>(item->getAllElementNames()->at(j), item));
			}
		}
	}

	for (unsigned int i = 0; i < this->spfItemDescription.size(); i++) {
		item = &(spfItemDescription.at(i));

		if (item->getItemType() != ItemTypeEnum::CompoundLength && item->getItemType() != ItemTypeEnum::ExplicitLength) {
			for (unsigned int j = 0; j < item->getAllElementNames()->size(); j++) {
				elementsMap.insert(pair<string, Item*>(item->getAllElementNames()->at(j), item));
			}
		}
	}
}

map<string, Item*>* CategoryDescription::getElementsMap() {
	return &elementsMap;
}

vector<unsigned short>* CategoryDescription::getMandatoryItemFRNs() {
	return &mandatoryItemFRNs;
}

void CategoryDescription::setMandatoryItemFRNs(vector<unsigned short> _mandatoryItemFRNs) {
	this->mandatoryItemFRNs = _mandatoryItemFRNs;
}
