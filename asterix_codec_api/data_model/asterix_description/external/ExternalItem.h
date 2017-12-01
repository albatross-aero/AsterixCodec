#ifndef DATAMODEL_EXTERNAL_EXTERNALITEM_H
#define DATAMODEL_EXTERNAL_EXTERNALITEM_H

#include <config/enum/FunctionNameEnum.hpp>
#include <config/enum/ItemTypeEnum.hpp>
#include <config/enum/RepresentationModeEnum.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "lib/autojsoncxx.hpp"
#include "lib/autoJSONcxx/staticjson/basic.hpp"
#include "lib/boost/serialization/base_object.hpp"
#include "lib/boost/serialization/vector.hpp"
#include "utils/ConversionFromStringToEnum.hpp"

using namespace std;

/**
 * ItemTypeEnum registration in autoJSONcxx to be automatically loaded from JSON file to object instance.
 */
STATICJSON_DECLARE_ENUM(ItemTypeEnum, {"fixed", ItemTypeEnum::FixedLength}, {"extended", ItemTypeEnum::ExtendedLength}, {"repetitive",
						ItemTypeEnum::RepetitiveLength}, {"compound", ItemTypeEnum::CompoundLength}, {"explicit", ItemTypeEnum::ExplicitLength})

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
		string getName();

		string* getPointerToName();

		/**
		 * Sets the name of an Item
		 * @param _name is the string representing the Item name
		 */
		void setName(string _name);

		/**
		 * Returns the length of the Item, expressed in Byte
		 * @return the number of Byte representing the Item length
		 */
		unsigned int getLengthInByte();

		/**
		 * Sets the length of an Item
		 * @param _lengthInByte is the length of the Item, expressed in Byte
		 */
		void setLengthInByte(unsigned int _lengthInByte);

		/**
		 * Returns the type of the Item
		 * @return a ItemTypeEnum value, to represent the type of the Item
		 */
		ItemTypeEnum getType();

		/**
		 * Sets the Element type of an Item
		 * @param _type is the ItemTypeEnum value, representing the type of the Item
		 */
		void setType(ItemTypeEnum _type);

		/**
		 * Returns the collection of Element descriptions of the Item
		 * @return a multidimensional vector of strings where each Element characteristic is represented by a string
		 */
		vector<vector<string> > getElementDescriptions();

		/**
		 * Returns a pointer to the existing collection od Element descriptions.
		 * @return A pointer to a multidimensional vector of strings.
		 */
		vector<vector<string>>* getPointerToElementDescriptions();

		/**
		 * Sets the Element descriptions collection of an Item
		 * @param elementDescriptions is a vector<vector<string>>, where the outer vector holds another vector of string, where the Element attributes are listed
		 */
		void setElementDescriptions(vector<vector<string> > elementDescriptions);

		// AUTOJSONCXX
		void staticjson_init(staticjson::ObjectHandler *h) {
			h->add_property("name", &this->name);
			h->add_property("type", &this->type);
			h->add_property("lengthInByte", &this->lengthInByte);
			h->add_property("elements", &this->elementDescriptions);

			h->set_flags(staticjson::Flags::Default | staticjson::Flags::AllowDuplicateKey);
		}

		// Serialization of the object using Boost
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & name;
			ar & type;
			ar & lengthInByte;
			ar & elementDescriptions;
		}

	private:
		string name;
		ItemTypeEnum type = ItemTypeEnum::InvalidValue;
		unsigned int lengthInByte;
		vector<vector<string>> elementDescriptions;
};

#endif //DATAMODEL_EXTERNAL_EXTERNALITEM_H
