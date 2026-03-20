#pragma once
#include "Inventory.h"
#include "PotionData.h"
#include "CharacterData.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

using namespace std;
using namespace sf;

class PotionInventory : public Inventory {
public:
    PotionInventory();
    ~PotionInventory();

    // Inventory interface implementation
    bool addItem(const string& itemName) override;
    bool removeItem(const string& itemName) override;
    int getItemCount(const string& itemName) const override;
    vector<string> getAllItems() const override;
    bool hasItem(const string& itemName) const override;

    // Potion-specific methods
    bool addPotion(const Potion& potion);
    void usePotion(int index);
    Potion getPotionAtIndex(int index) const;
    int getPotionCount() const { return static_cast<int>(potionInventory.size()); }
    void initUI(float x, float y);
    void draw(RenderWindow& window);
    void update();
    int checkUseButtonClick(const RenderWindow& window);
    double getSpeedPotionFatigueReduction() const;

    // Per-character state save/load
    void savePotions(vector<SerializedPotion>& outPotions) const {
        outPotions.clear();
        for (const auto& potion : potionInventory) {
            SerializedPotion sp;
            sp.type = static_cast<int>(potion.type);
            sp.rarity = static_cast<int>(potion.rarity);
            sp.cost = potion.cost;
            sp.statBonus = potion.statBonus;
            sp.durationDays = potion.durationDays;
            sp.purchaseTime = static_cast<long long>(potion.purchaseTime);
            sp.isActive = potion.isActive;
            outPotions.push_back(sp);
        }
    }

    void loadPotions(const vector<SerializedPotion>& inPotions) {
        potionInventory.clear();
        potionCounts.clear();

        for (const auto& sp : inPotions) {
            Potion potion;
            potion.type = static_cast<PotionType>(sp.type);
            potion.rarity = static_cast<PotionRarity>(sp.rarity);
            potion.cost = sp.cost;
            potion.statBonus = sp.statBonus;
            potion.durationDays = sp.durationDays;
            potion.purchaseTime = static_cast<time_t>(sp.purchaseTime);
            potion.isActive = sp.isActive;
            potion.updateNameAndDescription();

            potionInventory.push_back(potion);
            addItem(potion.name);
        }
    }

    void clearAll() {
        potionInventory.clear();
        potionCounts.clear();
    }

private:
    map<string, int> potionCounts;
    vector<Potion> potionInventory;
    Font font;
    vector<RectangleShape> useButtons;
    vector<Text> potionLabels;
};