#ifndef DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_UAPANDMANDATORYITEMS_H_
#define DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_UAPANDMANDATORYITEMS_H_

#include <string>
#include <vector>
#include <iostream>

#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

using namespace std;

/**
 * TODO:
*/
class UapAndMandatoryItems {

public:

    UapAndMandatoryItems() = default;

    ~UapAndMandatoryItems() = default;

    // GETTERS and SETTERS
    vector<string> getUapList() const {
        return uapList;
    }

    vector<string> *getPointerToUapList() {
        return &uapList;
    }

    vector<string> getMandatoryItems() const {
        return mandatoryItems;
    }

    vector<string> *getPointerToMandatoryItems() {
        return &mandatoryItems;
    }

    void setUapList(const vector<string> &uapList) {
        UapAndMandatoryItems::uapList = uapList;
    }

    void setMandatoryItems(const vector<string> &mandatoryItems) {
        UapAndMandatoryItems::mandatoryItems = mandatoryItems;
    }

private:
    vector<string> uapList;
    vector<string> mandatoryItems;
};

// NLOHMANN-JSON
void to_json(json &j, const UapAndMandatoryItems &items);

void from_json(const json &j, UapAndMandatoryItems &items);

#endif /* DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_UAPANDMANDATORYITEMS_H_ */
