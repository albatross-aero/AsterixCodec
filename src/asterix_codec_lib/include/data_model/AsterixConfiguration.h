#ifndef DATA_MODEL_ASTERIXCONFIGURATION_HPP_
#define DATA_MODEL_ASTERIXCONFIGURATION_HPP_

#include <string>
#include <vector>

#include "data_model/asterix_description/external/ExternalItem.h"
#include "data_model/CategoryConfiguration.h"
#include "nlohmann_json/json.hpp"

using json = nlohmann::json;

/**
 * \brief This class contains Category configurations with all file paths needed for each available Category. It is directly loaded from the JSON configuration file and
 * all information loaded are used later to create an ASTERIX categories description map.
 */
class AsterixConfiguration {

public:
    AsterixConfiguration() = default;

    ~AsterixConfiguration() = default;

    /**
     * Getter for the categoriesConfiguration object (it makes a copy of the private variable).
     */
    const vector<CategoryConfiguration>& getCategoriesConfiguration() const;

    /**
     * Setter for the categoriesConfiguration object.
     */
    void setCategoriesConfiguration(const vector<CategoryConfiguration>& categoriesConfiguration);

    /**
     * Getter for the categoriesFilterFile path.
     */
    const string &getCategoriesFilterFile() const;

    /**
     * Setter for the categoriesFilterFile path.
     */
    void setCategoriesFilterFile(const string &categoriesFilterFile);

private:
    /**
     * Vector of Category Description coming from ASTERIX Configuration File
     */
    vector<CategoryConfiguration> categoriesConfiguration;
    string categoriesFilterFile;
};

// NLOHMANN-JSON
void to_json(json &j, const AsterixConfiguration &asterixConfig);

void from_json(const json &j, AsterixConfiguration &asterixConfig);

#endif /* DATA_MODEL_ASTERIXCONFIGURATION_HPP_ */
