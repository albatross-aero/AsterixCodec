#include "data_model/asterix_description/external/UapAndMandatoryItems.h"

void to_json(json &j, const UapAndMandatoryItems &items) {
    j = json{{"uap",       items.getUapList()},
             {"mandatory", items.getMandatoryItems()}};
}

void from_json(const json &j, UapAndMandatoryItems &items) {
    items.setUapList(j.at("uap").get<vector<string>>());
    items.setMandatoryItems(j.at("mandatory").get<vector<string>>());
}