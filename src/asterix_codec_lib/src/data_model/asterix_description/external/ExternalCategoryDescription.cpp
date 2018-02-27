#include "data_model/asterix_description/external/ExternalCategoryDescription.h"

ExternalCategoryDescription::ExternalCategoryDescription() {
}

vector<ExternalItem> ExternalCategoryDescription::getItemCollection() const {
    return itemCollection;
}

vector<ExternalItem> *ExternalCategoryDescription::getPointerToItemCollection() {
    return &itemCollection;
}

void ExternalCategoryDescription::setItemCollection(vector<ExternalItem> itemCollection) {
    this->itemCollection = itemCollection;
}

vector<ExternalItem> ExternalCategoryDescription::getCompoundSubitemsCollection() const {
    return compoundSubitemCollection;
}

vector<ExternalItem> *ExternalCategoryDescription::getPointerToCompoundSubitemsCollection() {
    return &compoundSubitemCollection;
}

void ExternalCategoryDescription::setCompoundSubitemsCollection(vector<ExternalItem> compoundSubitemsCollection) {
    this->compoundSubitemCollection = compoundSubitemsCollection;
}

void to_json(json &j, const ExternalCategoryDescription &categoryDescription) {
    j = json{{"itemCollection",            categoryDescription.getItemCollection()},
             {"compoundSubitemCollection", categoryDescription.getCompoundSubitemsCollection()}};
}

void from_json(const json &j, ExternalCategoryDescription &categoryDescription) {
    categoryDescription.setItemCollection(j.at("itemCollection").get<vector<ExternalItem>>());
    categoryDescription.setCompoundSubitemsCollection(j.at("compoundSubitemCollection").get<vector<ExternalItem>>());
}