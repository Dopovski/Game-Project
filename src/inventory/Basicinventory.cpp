#include "BasicInventory.h"

bool BasicInventory::addItem(const string& itemName) {
    itemCounts[itemName]++;
    return true;
}

bool BasicInventory::removeItem(const string& itemName) {
    auto it = itemCounts.find(itemName);
    if (it != itemCounts.end() && it->second > 0) {
        it->second--;
        if (it->second == 0) {
            itemCounts.erase(it);
        }
        return true;
    }
    return false;
}

int BasicInventory::getItemCount(const string& itemName) const {
    auto it = itemCounts.find(itemName);
    if (it != itemCounts.end()) {
        return it->second;
    }
    return 0;
}

vector<string> BasicInventory::getAllItems() const {
    vector<string> items;
    for (const auto& pair : itemCounts) {
        if (pair.second > 0) {
            items.push_back(pair.first);
        }
    }
    return items;
}

bool BasicInventory::hasItem(const string& itemName) const {
    auto it = itemCounts.find(itemName);
    return it != itemCounts.end() && it->second > 0;
}

int BasicInventory::getUniqueItemCount() const {
    return static_cast<int>(itemCounts.size());
}

void BasicInventory::clear() {
    itemCounts.clear();
}

std::map<std::string, int> BasicInventory::saveToMap() const {
    return itemCounts;
}

void BasicInventory::loadFromMap(const std::map<std::string, int>& items) {
    itemCounts = items;
}