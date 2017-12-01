#ifndef DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_UAPANDMANDATORYITEMS_H_
#define DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_UAPANDMANDATORYITEMS_H_

#include <string>
#include <vector>
#include <iostream>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"

using namespace std;

/**
 * TODO:
*/
class UapAndMandatoryItems {

public:

	UapAndMandatoryItems() {
	}

	~UapAndMandatoryItems() {
	}

	// GETTERS and SETTERS
	std::vector<std::string> getUapList() {
		return uapList;
	}

	std::vector<std::string>* getPointerToUapList() {
		return &uapList;
	}

	std::vector<std::string> getMandatoryItems() {
		return mandatoryItems;
	}

	std::vector<std::string>* getPointerToMandatoryItems() {
		return &mandatoryItems;
	}

	//OTHERS
	/**
	 * AutoJSON loading from JSON to Object
	 */
	void staticjson_init(staticjson::ObjectHandler *h) {
		h->add_property("uap", &this->uapList);
		h->add_property("mandatory", &this->mandatoryItems, staticjson::Flags::Optional);
		h->set_flags(staticjson::Flags::Default | staticjson::Flags::DisallowUnknownKey);
	}

private:
	std::vector<std::string> uapList;
	std::vector<std::string> mandatoryItems;
};

#endif /* DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_UAPANDMANDATORYITEMS_H_ */
