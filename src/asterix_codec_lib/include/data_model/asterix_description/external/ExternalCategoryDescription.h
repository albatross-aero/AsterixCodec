#ifndef DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_
#define DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_

#include <vector>

#include "data_model/asterix_description/external/ExternalItem.h"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * The ExternalCategoryDescription class maps the ASTERIX Category JSON description to the internal Codec data model
 */
class ExternalCategoryDescription {
public:
    ExternalCategoryDescription();

    //GETTERS AND SETTERS

    /**
     * Returns a vector of ExternalItem instances, where the ASTERIX Category JSON description is mapped to.
     * @return a vector<ExternalItem> holding the Item collection of an ASTERIX Category
     */
    vector<ExternalItem> getItemCollection() const;

    vector<ExternalItem> *getPointerToItemCollection();

    /**
     * Sets a vector of ExternalItem instances, representing the Item collection of an ASTERIX Category JSON description.
     */
    void setItemCollection(vector<ExternalItem> itemCollection);

    /**
     * Returns a vector of ExternalItem instances, where the ASTERIX Category JSON description is mapped to.
     * @return a vector<ExternalItem> holding the collection of Compound Subitems of an ASTERIX Category
     */
    vector<ExternalItem> getCompoundSubitemsCollection() const;

    vector<ExternalItem> *getPointerToCompoundSubitemsCollection();

    /**
     * Sets a vector of ExternalItem instances, representing the Compound Subitem collection of an ASTERIX Category JSON description.
     */
    void setCompoundSubitemsCollection(vector<ExternalItem> compoundSubitemsCollection);

private:
    vector<ExternalItem> itemCollection;
    vector<ExternalItem> compoundSubitemCollection;
};

// NLOHMANN-JSON
void to_json(json &j, const ExternalCategoryDescription &categoryDescription);

void from_json(const json &j, ExternalCategoryDescription &categoryDescription);

#endif /* DATA_MODEL_ASTERIX_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_ */
