#include "data_model/AsterixConfiguration.h"

const std::vector<CategoryConfiguration> &AsterixConfiguration::getCategoriesConfiguration() const {
    return categoriesConfiguration;
}

void AsterixConfiguration::setCategoriesConfiguration(const vector<CategoryConfiguration> &categoriesConfiguration) {
    AsterixConfiguration::categoriesConfiguration = categoriesConfiguration;
}

const string &AsterixConfiguration::getCategoriesFilterFile() const {
    return categoriesFilterFile;
}

void AsterixConfiguration::setCategoriesFilterFile(const string &categoriesFilterFile) {
    AsterixConfiguration::categoriesFilterFile = categoriesFilterFile;
}

// NLOHMANN-JSON
void to_json(json &j, const AsterixConfiguration &asterixConfig) {
    j = json{{"categories",           asterixConfig.getCategoriesConfiguration()},
             {"categoriesFilterFile", asterixConfig.getCategoriesFilterFile()}};
}

void from_json(const json &j, AsterixConfiguration &asterixConfig) {
    asterixConfig.setCategoriesConfiguration(j.at("categories").get<vector<CategoryConfiguration>>());
    asterixConfig.setCategoriesFilterFile(j.at("categoriesFilterFile").get<string>());
}