#include "data_model/ItemsToFilter.h"

FilterTypeEnum ItemsToFilter::getTypeOfFiltering() const {
    return typeOfFiltering;
}

vector<string> ItemsToFilter::getListOfItemsToFilter() const {
    return listOfItemsToFilter;
}

void ItemsToFilter::setListOfItemsToFilter(vector<string> _listOfItemsToFilter) {
    listOfItemsToFilter = _listOfItemsToFilter;
}

void ItemsToFilter::setTypeOfFiltering(FilterTypeEnum typeOfFiltering) {
    ItemsToFilter::typeOfFiltering = typeOfFiltering;
}

void to_json(json &j, const ItemsToFilter &items) {
    j = json{{"typeOfFiltering", items.getTypeOfFiltering()},
             {"itemsToFilter",   items.getListOfItemsToFilter()}};
}

void from_json(const json &j, ItemsToFilter &items) {
    items.setTypeOfFiltering(j.at("typeOfFiltering").get<FilterTypeEnum>());
    items.setListOfItemsToFilter(j.at("itemsToFilter").get<vector<string>>());
}