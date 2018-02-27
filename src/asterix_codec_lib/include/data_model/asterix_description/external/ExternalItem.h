#ifndef DATAMODEL_EXTERNAL_EXTERNALITEM_H
#define DATAMODEL_EXTERNAL_EXTERNALITEM_H

#include <string>
#include <vector>
#include <iostream>

#include "config/enum/FunctionNameEnum.h"
#include "config/enum/ItemTypeEnum.h"
#include "config/enum/RepresentationModeEnum.h"

#include "boost/serialization/base_object.hpp"
#include "boost/serialization/vector.hpp"
#include "utils/ConversionFromStringToEnum.hpp"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

using namespace std;

//
// /**
// * ItemTypeEnum registration in autoJSONcxx to be automatically loaded from JSON file to object instance.
// */
//STATICJSON_DECLARE_ENUM(ItemTypeEnum, { "fixed", ItemTypeEnum::FixedLength }, { "extended", ItemTypeEnum::ExtendedLength }, {
//    "repetitive",
//    ItemTypeEnum::RepetitiveLength
//}, { "compound", ItemTypeEnum::CompoundLength }, { "explicit", ItemTypeEnum::ExplicitLength })

/**
 * \brief This class stores the description of an Item. There are some vectors that contains information for the parsing and the decoding of elements.
 */
class ExternalItem {

public:

    ExternalItem();

    //GETTERS AND SETTERS

    /**
     * Returns the name of the Item
     * @return a string with he name of the Item
     */
    string getName() const;

    string *getPointerToName();

    /**
     * Sets the name of an Item
     * @param _name is the string representing the Item name
     */
    void setName(string _name);

    /**
     * Returns the length of the Item, expressed in Byte
     * @return the number of Byte representing the Item length
     */
    unsigned short int getLengthInByte() const;

    /**
     * Sets the length of an Item
     * @param _lengthInByte is the length of the Item, expressed in Byte
     */
    void setLengthInByte(unsigned short int _lengthInByte);

    /**
     * Returns the type of the Item
     * @return a ItemTypeEnum value, to represent the type of the Item
     */
    ItemTypeEnum getType() const;

    /**
     * Sets the Element type of an Item
     * @param _type is the ItemTypeEnum value, representing the type of the Item
     */
    void setType(ItemTypeEnum _type);

    /**
     * Returns the collection of Element descriptions of the Item
     * @return a multidimensional vector of strings where each Element characteristic is represented by a string
     */
    vector<vector<string> > getElementDescriptions() const;

    /**
     * Returns a pointer to the existing collection od Element descriptions.
     * @return A pointer to a multidimensional vector of strings.
     */
    vector<vector<string>> *getPointerToElementDescriptions();

    /**
     * Sets the Element descriptions collection of an Item
     * @param elementDescriptions is a vector<vector<string>>, where the outer vector holds another vector of string, where the Element attributes are listed
     */
    void setElementDescriptions(vector<vector<string> > elementDescriptions);

    // Serialization of the object using Boost
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & name;
        ar & type;
        ar & lengthInByte;
        ar & elementDescriptions;
    }

private:
    string name;
    ItemTypeEnum type = ItemTypeEnum::InvalidValue;
    unsigned short int lengthInByte;
    vector<vector<string>> elementDescriptions;
};

// NLOHMANN-JSON
void to_json(json &j, const ExternalItem &item);

void from_json(const json &j, ExternalItem &item);

#endif //DATAMODEL_EXTERNAL_EXTERNALITEM_H
