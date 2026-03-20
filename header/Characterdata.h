#pragma once
#include <string>
#include <map>
#include <vector>
#include "Faction.h"

struct BrewerStateData {
    bool isLocked;
    long long lastPurchaseTime;
    long long lockResetTime;

    BrewerStateData() : isLocked(false), lastPurchaseTime(0), lockResetTime(0) {}
};

struct SerializedPotion {
    int type;
    int rarity;
    int cost;
    double statBonus;
    int durationDays;
    long long purchaseTime;
    bool isActive;

    SerializedPotion() : type(0), rarity(0), cost(0), statBonus(0.0), durationDays(0), purchaseTime(0), isActive(false) {}
};

struct CharacterSaveData {
    std::string characterName;
    int raceIndex;
    int classIndex;
    Faction faction;
    std::string raceName;
    int level;
    double currentXP;

    // Currency
    int silver;
    int gold;
    int torches;

    // Mount data
    int mountType;
    int mountRemainingDays;

    // Fatigue
    float fatigue;
    std::string lastFatigueResetDate;

    // Inventory per character
    std::map<std::string, int> inventoryItems;

    // Equipment per character (FIXED - THIS WAS MISSING)
    std::map<int, std::string> equipment;

    // Brewer state per character
    BrewerStateData brewerState;

    // Potion inventory per character
    std::vector<SerializedPotion> potionInventory;

    // Curse data per character
    int curseId;
    double curseHealthMod;
    double curseAttackMod;
    double curseIntelligenceMod;
    double curseDexterityMod;
    double curseArmorMod;
    double curseLuckMod;
    bool hasFirstCurse;
    bool unlockedClass;

    // ============================================
    // SHOP STATE PER CHARACTER (NEW)
    // ============================================
    std::vector<std::string> shopItemNames;
    std::vector<int> shopItemRarities;
    std::vector<int> shopItemPrices;
    std::vector<bool> shopItemPurchased;
    std::vector<int> shopItemClassIndices;
    std::string shopLastResetDate;

    CharacterSaveData()
        : characterName(""),
        raceIndex(-1),
        classIndex(-1),
        faction(Faction::None),
        raceName(""),
        level(1),
        currentXP(0.0),
        silver(0),
        gold(0),
        torches(0),
        mountType(0),
        mountRemainingDays(0),
        fatigue(0.0f),
        lastFatigueResetDate(""),
        curseId(-1),
        curseHealthMod(0.0),
        curseAttackMod(0.0),
        curseIntelligenceMod(0.0),
        curseDexterityMod(0.0),
        curseArmorMod(0.0),
        curseLuckMod(0.0),
        hasFirstCurse(false),
        unlockedClass(false),
        shopLastResetDate("") {
    }
};