#include "config/CommonVariables.h"   // It is a way to allow that two classes refer to each other
#include "data_model/asterix_description/external/ExternalItem.h"

ExternalItem::ExternalItem() {
    lengthInByte = 0;
}

vector<vector<string> > ExternalItem::getElementDescriptions() const {
    return elementDescriptions;
}

vector<vector<string>> *ExternalItem::getPointerToElementDescriptions() {
    return &elementDescriptions;
}

void ExternalItem::setElementDescriptions(vector<vector<string> > elementDescriptions) {
    this->elementDescriptions = elementDescriptions;
}

unsigned short int ExternalItem::getLengthInByte() const {
    return lengthInByte;
}

void ExternalItem::setLengthInByte(unsigned short int lengthInByte) {
    this->lengthInByte = lengthInByte;
}

string ExternalItem::getName() const {
    return name;
}

string *ExternalItem::getPointerToName() {
    return &name;
}

void ExternalItem::setName(string name) {
    this->name = name;
}

ItemTypeEnum ExternalItem::getType() const {
    return type;
}

void ExternalItem::setType(ItemTypeEnum type) {
    this->type = type;
}

void to_json(json &j, const ExternalItem &item) {
    j = json{{"name",         item.getName()},
             {"type",         item.getType()},
             {"lengthInByte", item.getLengthInByte()},
             {"elements",     item.getElementDescriptions()}};
}

void from_json(const json &j, ExternalItem &item) {
    item.setName(j.at("name").get<string>());
    item.setType(j.at("type").get<ItemTypeEnum>());
    item.setLengthInByte(j.at("lengthInByte").get<unsigned short int>());
    item.setElementDescriptions(j.at("elements").get<vector<vector<string>>>());
}
