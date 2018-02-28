#include "data_model/CategoryFilter.h"

int CategoryFilter::getCategoryNumber() const {
    return categoryNumber;
}

vector<string> CategoryFilter::getListOfDatasourcesToFilter() const {
    return listOfDatasourcesToFilter;
}

void CategoryFilter::setCategoryNumber(int categoryNumber) {
    CategoryFilter::categoryNumber = categoryNumber;
}

void CategoryFilter::setListOfDatasourcesToFilter(const vector<string> &listOfDatasourcesToFilter) {
    CategoryFilter::listOfDatasourcesToFilter = listOfDatasourcesToFilter;
}

// NLOHMANN-JSON
void to_json(json &j, const CategoryFilter &categoryFilter) {
    j = json{{"categoryNumber",            categoryFilter.getCategoryNumber()},
             {"listOfDatasourcesToFilter", categoryFilter.getListOfDatasourcesToFilter()}};
}

void from_json(const json &j, CategoryFilter &categoryFilter) {
    categoryFilter.setCategoryNumber(j.at("categoryNumber").get<int>());
    categoryFilter.setListOfDatasourcesToFilter(j.at("listOfDatasourcesToFilter").get<vector<string>>());
}