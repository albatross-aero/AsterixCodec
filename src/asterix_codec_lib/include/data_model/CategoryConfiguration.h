#ifndef DATA_MODEL_CATEGORIESCONFIGURATION_H_
#define DATA_MODEL_CATEGORIESCONFIGURATION_H_

#include <string>
#include <vector>

#include "nlohmann_json/json.hpp"

using namespace std;
using json = nlohmann::json;

/**
 * \brief This class collects all reference to the files a given Edition of an ASTERIX Category description is made of.
 *
 */
class CategoryConfiguration {

public:
    CategoryConfiguration() {
        categoryNumber = 0;
    }

    ~CategoryConfiguration() = default;

    // Getter and Setters
    bool isCategoryByDefault() const;

    void setCategoryByDefault(bool categoryByDefault);

    unsigned short int getCategoryNumber() const;

    const vector<string> &getDataSourcesInDec() const;

    const string &getFilterFile() const;

    const string &getItemsDescriptionFile() const;

    const string &getRefDescriptionFile() const;

    const string &getSpfDescriptionFile() const;

    const string &getUapAndMandatoryItemsFile() const;

    void setCategoryNumber(unsigned short int categoryNumber);

    void setUapAndMandatoryItemsFile(const string &uapAndMandatoryItemsFile);

    void setItemsDescriptionFile(const string &itemsDescriptionFile);

    void setRefDescriptionFile(const string &refDescriptionFile);

    void setSpfDescriptionFile(const string &spfDescriptionFile);

    void setFilterFile(const string &filterFile);

    void setDataSourcesInDec(const vector<string> &dataSourcesInDec);

private:
    /**
     * Value coming from the ASTERIX configuration file to generate the map ASTERIX Description.
     * Asterix Edition number a.k.a edition name
     */
    unsigned short int categoryNumber;

    /**
     * Path to JSON file with UAP and mandatory items
     */
    string uapAndMandatoryItemsFile;

    /**
     * Path to items description JSON file
     */
    string itemsDescriptionFile;

    /**
     * Path to Reserved Expansion Field JSON file
     */
    string refDescriptionFile;

    /**
     * Path to Special Purpose Field JSON file
     */
    string spfDescriptionFile;

    /**
     * Path to filter JSON file
     */
    string filterFile;

    /**
     * Array of Data Sources using this description
     */
    vector<string> dataSourcesInDec;

    /**
     * Boolean to identify if this description is to be used by default to support the category, or not
     */
    bool categoryByDefault = false;
};

// NLOHMANN-JSON
void to_json(json &j, const CategoryConfiguration &config);

void from_json(const json &j, CategoryConfiguration &config);

#endif /* DATA_MODEL_CATEGORIESCONFIGURATION_H_ */
