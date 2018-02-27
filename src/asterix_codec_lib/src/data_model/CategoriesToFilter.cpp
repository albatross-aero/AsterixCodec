#include "data_model/CategoriesToFilter.h"

FilterTypeEnum CategoriesToFilter::getTypeOfFiltering() const {
    return typeOfFiltering;
}

void CategoriesToFilter::setTypeOfFiltering(FilterTypeEnum _typeOfFiltering) {
    typeOfFiltering = _typeOfFiltering;
}

vector<CategoryFilter> CategoriesToFilter::getListOfCategoryFilter() const {
    return listOfCategoryFilter;
}

vector<int> CategoriesToFilter::getListOfCategoryInDecimalToFilter() const {
    return listOfCategoryInDecimalToFilter;
}

void CategoriesToFilter::setListOfCategoryInDecimalToFilter(vector<int> _listOfCategoryInDecimalToFilter) {
    listOfCategoryInDecimalToFilter = _listOfCategoryInDecimalToFilter;
}

vector<int> CategoriesToFilter::getListOfCategoryAndDatasourcesInDecimalToFilter() const {
    return listOfCategoryAndDatasourcesInDecimalToFilter;
}

void CategoriesToFilter::setListOfCategoryAndDatasourcesInDecimalToFilter(vector<int> _listOfCategoryAndDatasourcesInDecimalToFilter) {
    listOfCategoryAndDatasourcesInDecimalToFilter = _listOfCategoryAndDatasourcesInDecimalToFilter;
}

void CategoriesToFilter::setListOfCategoryFilter(const vector<CategoryFilter> &listOfCategoryFilter) {
    CategoriesToFilter::listOfCategoryFilter = listOfCategoryFilter;
}

// NLOHMANN-JSON
void to_json(json &j, const CategoriesToFilter &categories) {
    j = json{{"typeOfFiltering",                               categories.getTypeOfFiltering()},
             {"listOfCategoryFilter",                          categories.getListOfCategoryFilter()},
             {"listOfCategoryInDecimalToFilter",               categories.getListOfCategoryInDecimalToFilter()},
             {"listOfCategoryAndDatasourcesInDecimalToFilter", categories.getListOfCategoryAndDatasourcesInDecimalToFilter()}};
}

void from_json(const json &j, CategoriesToFilter &categories) {
    categories.setTypeOfFiltering(j.at("typeOfFiltering").get<FilterTypeEnum>());
    categories.setListOfCategoryFilter(j.at("listOfCategoryFilter").get<vector<CategoryFilter>>());
    categories.setListOfCategoryInDecimalToFilter(j.at("listOfCategoryInDecimalToFilter").get<vector<int>>());
    categories.setListOfCategoryAndDatasourcesInDecimalToFilter(j.at("listOfCategoryAndDatasourcesInDecimalToFilter").get<vector<int>>());
}