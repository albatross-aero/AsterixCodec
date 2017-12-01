#include <data_model/asterix_description/external/ExternalCategoryDescription.h>

ExternalCategoryDescription::ExternalCategoryDescription() {
}

vector<ExternalItem> ExternalCategoryDescription::getItemCollection() {
	return itemCollection;
}

vector<ExternalItem>* ExternalCategoryDescription::getPointerToItemCollection() {
	return &itemCollection;
}

void ExternalCategoryDescription::setItemCollection(vector<ExternalItem> itemCollection) {
	this->itemCollection = itemCollection;
}

vector<ExternalItem> ExternalCategoryDescription::getCompoundSubitemsCollection() {
	return compoundSubitemCollection;
}

vector<ExternalItem>* ExternalCategoryDescription::getPointerToCompoundSubitemsCollection() {
	return &compoundSubitemCollection;
}

void ExternalCategoryDescription::setCompoundSubitemsCollection(vector<ExternalItem> compoundSubitemsCollection) {
	this->compoundSubitemCollection = compoundSubitemsCollection;
}
