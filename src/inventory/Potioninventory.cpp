#include "PotionInventory.h"
#include <iostream>

PotionInventory::PotionInventory() {
    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for potion inventory!" << endl;
    }
}

PotionInventory::~PotionInventory() {
}

bool PotionInventory::addItem(const string& itemName) {
    potionCounts[itemName]++;
    return true;
}

bool PotionInventory::removeItem(const string& itemName) {
    auto it = potionCounts.find(itemName);
    if (it != potionCounts.end() && it->second > 0) {
        it->second--;
        if (it->second == 0) {
            potionCounts.erase(it);
        }
        return true;
    }
    return false;
}

int PotionInventory::getItemCount(const string& itemName) const {
    auto it = potionCounts.find(itemName);
    if (it != potionCounts.end()) {
        return it->second;
    }
    return 0;
}

vector<string> PotionInventory::getAllItems() const {
    vector<string> items;
    for (const auto& pair : potionCounts) {
        if (pair.second > 0) {
            items.push_back(pair.first);
        }
    }
    return items;
}

bool PotionInventory::hasItem(const string& itemName) const {
    auto it = potionCounts.find(itemName);
    return it != potionCounts.end() && it->second > 0;
}

bool PotionInventory::addPotion(const Potion& potion) {
    potionInventory.push_back(potion);
    addItem(potion.name);
    return true;
}

void PotionInventory::usePotion(int index) {
    if (index >= 0 && index < static_cast<int>(potionInventory.size())) {
        removeItem(potionInventory[index].name);
        potionInventory.erase(potionInventory.begin() + index);
    }
}

Potion PotionInventory::getPotionAtIndex(int index) const {
    if (index >= 0 && index < static_cast<int>(potionInventory.size())) {
        return potionInventory[index];
    }
    return Potion();
}

void PotionInventory::initUI(float x, float y) {
    // Initialize UI if needed
}

void PotionInventory::draw(RenderWindow& window) {
    // Draw potion inventory UI if needed
}

int PotionInventory::checkUseButtonClick(const RenderWindow& window) {
    // Check for button clicks and return index
    return -1;
}

double PotionInventory::getSpeedPotionFatigueReduction() const {
    return 0.0;
}
void PotionInventory::update() {
    // Update logic for potion inventory if needed
}