#pragma once
#include "Inventory.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

class BasicInventory : public Inventory {
private:
    std::map<std::string, int> itemCounts;

public:
    BasicInventory() = default;
    ~BasicInventory() = default;

    bool addItem(const string& itemName) override;
    bool removeItem(const string& itemName) override;
    int getItemCount(const string& itemName) const override;
    vector<string> getAllItems() const override;
    bool hasItem(const string& itemName) const override;

    int getUniqueItemCount() const;
    void clear();
    std::map<std::string, int> saveToMap() const;
    void loadFromMap(const std::map<std::string, int>& items);
};