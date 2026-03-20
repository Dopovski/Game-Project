#pragma once
#include <string>
#include <vector>

using namespace std;

class Inventory {
public:
    Inventory() = default;
    virtual ~Inventory() = default;

    virtual bool addItem(const string& itemName) = 0;
    virtual bool removeItem(const string& itemName) = 0;
    virtual int getItemCount(const string& itemName) const = 0;
    virtual vector<string> getAllItems() const = 0;
    virtual bool hasItem(const string& itemName) const = 0;
};