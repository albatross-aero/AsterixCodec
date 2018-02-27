#include "data_model/CategoryConfiguration.h"

bool CategoryConfiguration::isCategoryByDefault() const {
    return categoryByDefault;
}

unsigned short int CategoryConfiguration::getCategoryNumber() const {
    return categoryNumber;
}

const vector<string>& CategoryConfiguration::getDataSourcesInDec() const {
    return dataSourcesInDec;
}

const string& CategoryConfiguration::getFilterFile() const {
    return filterFile;
}

const string& CategoryConfiguration::getItemsDescriptionFile() const {
    return itemsDescriptionFile;
}

const string& CategoryConfiguration::getRefDescriptionFile() const {
    return refDescriptionFile;
}

const string& CategoryConfiguration::getSpfDescriptionFile() const {
    return spfDescriptionFile;
}

const string& CategoryConfiguration::getUapAndMandatoryItemsFile() const {
    return uapAndMandatoryItemsFile;
}

void CategoryConfiguration::setCategoryByDefault(bool categoryByDefault) {
    CategoryConfiguration::categoryByDefault = categoryByDefault;
}

void CategoryConfiguration::setCategoryNumber(unsigned short int categoryNumber) {
    CategoryConfiguration::categoryNumber = categoryNumber;
}

void CategoryConfiguration::setUapAndMandatoryItemsFile(const string& uapAndMandatoryItemsFile) {
    CategoryConfiguration::uapAndMandatoryItemsFile = uapAndMandatoryItemsFile;
}

void CategoryConfiguration::setItemsDescriptionFile(const string& itemsDescriptionFile) {
    CategoryConfiguration::itemsDescriptionFile = itemsDescriptionFile;
}

void CategoryConfiguration::setRefDescriptionFile(const string& refDescriptionFile) {
    CategoryConfiguration::refDescriptionFile = refDescriptionFile;
}

void CategoryConfiguration::setSpfDescriptionFile(const string& spfDescriptionFile) {
    CategoryConfiguration::spfDescriptionFile = spfDescriptionFile;
}

void CategoryConfiguration::setFilterFile(const string& filterFile) {
    CategoryConfiguration::filterFile = filterFile;
}

void CategoryConfiguration::setDataSourcesInDec(const vector<string>& dataSourcesInDec) {
    CategoryConfiguration::dataSourcesInDec = dataSourcesInDec;
}

// NLOHMANN-JSON
void to_json(json& j, const CategoryConfiguration& config) {
    j = json{{"categoryNumber",           config.getCategoryNumber()},
             {"uapAndMandatoryItemsFile", config.getUapAndMandatoryItemsFile()},
             {"itemsDescriptionFile",     config.getItemsDescriptionFile()},
             {"refDescriptionFile",       config.getRefDescriptionFile()},
             {"spfDescriptionFile",       config.getSpfDescriptionFile()},
             {"filterFile",               config.getFilterFile()},
             {"dataSourcesInDec",         config.getDataSourcesInDec()},
             {"default",                  config.getCategoryNumber()}
    };
}

void from_json(const json& j, CategoryConfiguration& config) {
    config.setCategoryNumber(j.at("categoryNumber").get<unsigned short int>());
    config.setUapAndMandatoryItemsFile(j.at("uapAndMandatoryItemsFile").get<string>());
    config.setItemsDescriptionFile(j.at("itemsDescriptionFile").get<string>());
    config.setRefDescriptionFile(j.at("refDescriptionFile").get<string>());
    config.setSpfDescriptionFile(j.at("spfDescriptionFile").get<string>());
    config.setFilterFile(j.at("filterFile").get<string>());
    config.setDataSourcesInDec(j.at("dataSourcesInDec").get<vector<string>>());
    config.setCategoryByDefault(j.at("default").get<bool>());
}

//void CategoryConfiguration::staticjson_init(staticjson::ObjectHandler *h) {
//	h->add_property("categoryNumber", &this->categoryNumber);
//	h->add_property("uapAndMandatoryItemsFile", &this->uapAndMandatoryItemsFile);
//	h->add_property("itemsDescriptionFile", &this->itemsDescriptionFile);
//	h->add_property("refDescriptionFile", &this->refDescriptionFile, staticjson::Flags::Optional);
//	h->add_property("spfDescriptionFile", &this->spfDescriptionFile, staticjson::Flags::Optional);
//	h->add_property("filterFile", &this->filterFile, staticjson::Flags::Optional);
//	h->add_property("datasourcesInDec", &this->dataSourcesInDec, staticjson::Flags::Optional);
//	h->add_property("default", &this->categoryByDefault, staticjson::Flags::Optional);
//	h->set_flags(staticjson::Flags::Default | staticjson::Flags::AllowDuplicateKey);
//}
