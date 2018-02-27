#ifndef DATAMODEL_CATEGORY_DESCRIPTION_H
#define DATAMODEL_CATEGORY_DESCRIPTION_H

#include <vector>

#include "data_model/asterix_description/Item.h"
#include "config/FacadeCommonVariables.h"
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/map.hpp"

/**
 * \brief This class represents the ASTERIX Category JSON description in the internal Codec data model.
 * 'itemCollection' contains the description of all items of the Category stored in the same order of the UAP.
 */
class CategoryDescription {

public:

    CategoryDescription();

    CategoryDescription(vector<Item> _itemsDescription, vector<Item> _compoundSubItemsDescription, vector<Item> _refItemDescription,
                        vector<Item> _spfItemDescription);

    // GETTERS and SETTERS

    /**
     * TODO
     */
    bool isToBeDecoded();

    /**
     * TODO
     */
    void setToBeDecoded(bool _toBeDecoded);

    /**
     * TODO
     */
    bool isToBeEncoded();

    /**
     * TODO
     */
    void setToBeEncoded(bool _toBeEncoded);

    /**
     * Returns a pointer to the vector of Item instances, describing an ASTERIX Category.
     * @return a vector<Item>* holding the Item collection of an ASTERIX Category
     */
    vector<Item> *getItemCollection();

    /**
     * Sets a vector of Item instances, representing the Item collection of an ASTERIX Category JSON description.
     * @param _itemsDescription Vector of instances of Item descriptions
     */
    void setItemCollection(vector<Item> _itemsDescription);

    /**
     * Returns a pointer to the vector of Item instances, describing an ASTERIX Category.
     * @return a vector<Item>* holding the collection of Compound Subitems of an ASTERIX Category
     */
    vector<Item> *getCompoundSubItemsDescription();

    /**
     * Sets a vector of Item instances, representing the Compound Subitem collection of an ASTERIX Category JSON description.
     * @param _compoundSubItemsDescription Vector of instances of compound Item descriptions
     */
    void setCompoundSubItemsDescription(vector<Item> _compoundSubItemsDescription);

    /**
     * Returns a pointer to the vector of Item instances, describing an ASTERIX Category.
     * @return a vector<Item>* holding the Reserved Expansion Field (REF) Item collection of an ASTERIX Category
     */
    vector<Item> *getRefItemDescription();

    /**
     * Sets a vector of Item instances, representing the Reserved Expansion Field (REF) Item collection of an ASTERIX Category JSON description.
     * @param _reItemDescription Vector of instances of REF sub-Item descriptions
     */
    void setRefItemDescription(vector<Item> _reItemDescription);

    /**
     * Returns a pointer to the vector of Item instances, where the ASTERIX Category JSON description is mapped to.
     * @return a vector<Item>* holding the Item collection of the Special Purpose Field (SPF) of an ASTERIX Category
     */
    vector<Item> *getSpfItemDescription();

    /**
     * Sets a vector of Item instances, representing the Special Purpose Field (SPF) Item collection of an ASTERIX Category JSON description.
     * @param _spfItemDescription Vector of instances of SPF sub-Item descriptions
     */
    void setSpfItemDescription(vector<Item> _spfItemDescription);

    /**
     * In converts the string information related to each Item type (fixed, compound...) into the corresponding Enumeration version.
     */
    void convertInformationFromStringToEnum();

    /**
     *	Scans all Items description and fills a map, with as a key an Element names and as a value, the pointer to its Item description.
     */
    void fillElementsMap();

    /** Gets a map with all Element names of the Category and their pointer to the Item description.
     * @return Map with all Element names of the Category and their pointer to the Item description.
     */
    map<string, Item *> *getElementsMap();

    vector<unsigned short int> *getMandatoryItemFRNs();

    void setMandatoryItemFRNs(vector<unsigned short int> _mandatoryItemFRNs);

private:
    vector<Item> itemCollection;
    vector<Item> compoundSubItemsDescription;
    vector<Item> refItemDescription;
    vector<Item> spfItemDescription;

    map<string, Item *> elementsMap;
    vector<unsigned short> mandatoryItemFRNs;

    bool toBeDecoded;
    bool toBeEncoded;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & itemCollection;
        ar & compoundSubItemsDescription;
        ar & refItemDescription;
        ar & spfItemDescription;
        ar & elementsMap;
        ar & mandatoryItemFRNs;
        ar & toBeDecoded;
        ar & toBeEncoded;
    }
};

#endif  //DATAMODEL_CATEGORY_DESCRIPTION_H
