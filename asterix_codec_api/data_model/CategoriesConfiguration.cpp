#include "CategoriesConfiguration.h"

CategoriesConfiguration::CategoriesConfiguration() {
	categoryNumber = 0;
}

CategoriesConfiguration::~CategoriesConfiguration() {
}

bool CategoriesConfiguration::isCategoryByDefault() const {
	return categoryByDefault;
}

int CategoriesConfiguration::getCategoryNumber() const {
	return categoryNumber;
}

const std::vector<string>& CategoriesConfiguration::getDataSourcesInDec() const {
	return dataSourcesInDec;
}

const std::string& CategoriesConfiguration::getFilterFile() const {
	return filterFile;
}

const std::string& CategoriesConfiguration::getItemsDescriptionFile() const {
	return itemsDescriptionFile;
}

const std::string& CategoriesConfiguration::getRefDescriptionFile() const {
	return refDescriptionFile;
}

const std::string& CategoriesConfiguration::getSpfDescriptionFile() const {
	return spfDescriptionFile;
}

const std::string& CategoriesConfiguration::getUapAndMandatoryItemsFile() const {
	return uapAndMandatoryItemsFile;
}

void CategoriesConfiguration::staticjson_init(staticjson::ObjectHandler *h) {
	h->add_property("categoryNumber", &this->categoryNumber);
	h->add_property("uapAndMandatoryItemsFile", &this->uapAndMandatoryItemsFile);
	h->add_property("itemsDescriptionFile", &this->itemsDescriptionFile);
	h->add_property("refDescriptionFile", &this->refDescriptionFile, staticjson::Flags::Optional);
	h->add_property("spfDescriptionFile", &this->spfDescriptionFile, staticjson::Flags::Optional);
	h->add_property("filterFile", &this->filterFile, staticjson::Flags::Optional);
	h->add_property("datasourcesInDec", &this->dataSourcesInDec, staticjson::Flags::Optional);
	h->add_property("default", &this->categoryByDefault, staticjson::Flags::Optional);
	h->set_flags(staticjson::Flags::Default | staticjson::Flags::AllowDuplicateKey);
}
