#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include "CharacterData.h"
#include "GameStateManager.h"
#include "CurrencyManager.h"
#include "SystemsManager.h"
#include "CurseData.h"

class CharacterManager {
private:
    std::vector<CharacterSaveData> characters;
    int currentCharacterIndex;
    std::string saveFilePath;

    bool isValidName(const std::string& name) const {
        if (name.empty()) return false;

        for (size_t i = 0; i < name.length(); ++i) {
            char c = name[i];
            bool isLetter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
            bool isNumber = (c >= '0' && c <= '9');

            if (!isLetter && !isNumber) {
                return false;
            }
        }

        return true;
    }

    bool nameExists(const std::string& name) const {
        for (const auto& character : characters) {
            if (character.characterName == name) {
                return true;
            }
        }
        return false;
    }

public:
    CharacterManager() : currentCharacterIndex(-1), saveFilePath("characters.dat") {
    }

    bool loadCharacters() {
        characters.clear();

        std::ifstream file(saveFilePath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        try {
            int count;
            file.read(reinterpret_cast<char*>(&count), sizeof(int));

            if (count < 0 || count > 100) {
                file.close();
                return false;
            }

            for (int i = 0; i < count; ++i) {
                CharacterSaveData data;

                // Read name
                size_t nameLen;
                file.read(reinterpret_cast<char*>(&nameLen), sizeof(size_t));
                if (nameLen > 1000) {
                    file.close();
                    return false;
                }
                data.characterName.resize(nameLen);
                file.read(&data.characterName[0], nameLen);

                // Read race name
                size_t raceNameLen;
                file.read(reinterpret_cast<char*>(&raceNameLen), sizeof(size_t));
                if (raceNameLen > 1000) {
                    file.close();
                    return false;
                }
                data.raceName.resize(raceNameLen);
                file.read(&data.raceName[0], raceNameLen);

                // Read basic data
                file.read(reinterpret_cast<char*>(&data.raceIndex), sizeof(int));
                file.read(reinterpret_cast<char*>(&data.classIndex), sizeof(int));

                int factionInt;
                file.read(reinterpret_cast<char*>(&factionInt), sizeof(int));
                data.faction = static_cast<Faction>(factionInt);

                file.read(reinterpret_cast<char*>(&data.level), sizeof(int));
                file.read(reinterpret_cast<char*>(&data.currentXP), sizeof(double));
                file.read(reinterpret_cast<char*>(&data.silver), sizeof(int));
                file.read(reinterpret_cast<char*>(&data.gold), sizeof(int));
                file.read(reinterpret_cast<char*>(&data.torches), sizeof(int));
                file.read(reinterpret_cast<char*>(&data.mountType), sizeof(int));
                file.read(reinterpret_cast<char*>(&data.mountRemainingDays), sizeof(int));

                // Read fatigue
                if (!file.eof()) {
                    file.read(reinterpret_cast<char*>(&data.fatigue), sizeof(float));
                    if (!file.eof()) {
                        size_t fatigueResetLen;
                        file.read(reinterpret_cast<char*>(&fatigueResetLen), sizeof(size_t));
                        if (fatigueResetLen > 0 && fatigueResetLen < 100) {
                            data.lastFatigueResetDate.resize(fatigueResetLen);
                            file.read(&data.lastFatigueResetDate[0], fatigueResetLen);
                        }
                    }
                }

                // Read inventory
                int inventoryCount = 0;
                if (!file.eof()) {
                    file.read(reinterpret_cast<char*>(&inventoryCount), sizeof(int));
                    for (int j = 0; j < inventoryCount && !file.eof(); ++j) {
                        size_t itemNameLen;
                        file.read(reinterpret_cast<char*>(&itemNameLen), sizeof(size_t));
                        if (itemNameLen > 1000) break;

                        std::string itemName;
                        itemName.resize(itemNameLen);
                        file.read(&itemName[0], itemNameLen);

                        int quantity;
                        file.read(reinterpret_cast<char*>(&quantity), sizeof(int));

                        data.inventoryItems[itemName] = quantity;
                    }
                }

                // Read equipment
                int equipmentCount = 0;
                if (!file.eof()) {
                    file.read(reinterpret_cast<char*>(&equipmentCount), sizeof(int));
                    for (int j = 0; j < equipmentCount && !file.eof(); ++j) {
                        int slotIndex;
                        file.read(reinterpret_cast<char*>(&slotIndex), sizeof(int));

                        size_t itemNameLen;
                        file.read(reinterpret_cast<char*>(&itemNameLen), sizeof(size_t));
                        if (itemNameLen > 1000) break;

                        std::string itemName;
                        itemName.resize(itemNameLen);
                        file.read(&itemName[0], itemNameLen);

                        data.equipment[slotIndex] = itemName;
                    }
                }

                // Read brewer state
                if (!file.eof()) {
                    file.read(reinterpret_cast<char*>(&data.brewerState.isLocked), sizeof(bool));
                    file.read(reinterpret_cast<char*>(&data.brewerState.lastPurchaseTime), sizeof(long long));
                    file.read(reinterpret_cast<char*>(&data.brewerState.lockResetTime), sizeof(long long));
                }

                // Read potion inventory
                int potionCount = 0;
                if (!file.eof()) {
                    file.read(reinterpret_cast<char*>(&potionCount), sizeof(int));
                    for (int j = 0; j < potionCount && !file.eof(); ++j) {
                        SerializedPotion potion;
                        file.read(reinterpret_cast<char*>(&potion.type), sizeof(int));
                        file.read(reinterpret_cast<char*>(&potion.rarity), sizeof(int));
                        file.read(reinterpret_cast<char*>(&potion.cost), sizeof(int));
                        file.read(reinterpret_cast<char*>(&potion.statBonus), sizeof(double));
                        file.read(reinterpret_cast<char*>(&potion.durationDays), sizeof(int));
                        file.read(reinterpret_cast<char*>(&potion.purchaseTime), sizeof(long long));
                        file.read(reinterpret_cast<char*>(&potion.isActive), sizeof(bool));

                        data.potionInventory.push_back(potion);
                    }
                }

                // Read curse data
                if (!file.eof()) {
                    file.read(reinterpret_cast<char*>(&data.curseId), sizeof(int));
                    file.read(reinterpret_cast<char*>(&data.curseHealthMod), sizeof(double));
                    file.read(reinterpret_cast<char*>(&data.curseAttackMod), sizeof(double));
                    file.read(reinterpret_cast<char*>(&data.curseIntelligenceMod), sizeof(double));
                    file.read(reinterpret_cast<char*>(&data.curseDexterityMod), sizeof(double));
                    file.read(reinterpret_cast<char*>(&data.curseArmorMod), sizeof(double));
                    file.read(reinterpret_cast<char*>(&data.curseLuckMod), sizeof(double));
                    file.read(reinterpret_cast<char*>(&data.hasFirstCurse), sizeof(bool));
                    file.read(reinterpret_cast<char*>(&data.unlockedClass), sizeof(bool));
                }

                // Read shop state
                int shopItemCount = 0;
                if (!file.eof()) {
                    file.read(reinterpret_cast<char*>(&shopItemCount), sizeof(int));

                    if (shopItemCount > 0 && shopItemCount <= 10) {  // Sanity check
                        data.shopItemNames.clear();
                        data.shopItemRarities.clear();
                        data.shopItemPrices.clear();
                        data.shopItemPurchased.clear();
                        data.shopItemClassIndices.clear();

                        for (int j = 0; j < shopItemCount && !file.eof(); ++j) {
                            // Read item name
                            size_t itemNameLen;
                            file.read(reinterpret_cast<char*>(&itemNameLen), sizeof(size_t));
                            if (itemNameLen > 1000) break;

                            std::string itemName;
                            itemName.resize(itemNameLen);
                            file.read(&itemName[0], itemNameLen);
                            data.shopItemNames.push_back(itemName);

                            // Read item data into temporary variables
                            int rarity, price, classIdx;
                            bool purchased;
                            file.read(reinterpret_cast<char*>(&rarity), sizeof(int));
                            file.read(reinterpret_cast<char*>(&price), sizeof(int));
                            file.read(reinterpret_cast<char*>(&purchased), sizeof(bool));
                            file.read(reinterpret_cast<char*>(&classIdx), sizeof(int));

                            data.shopItemRarities.push_back(rarity);
                            data.shopItemPrices.push_back(price);
                            data.shopItemPurchased.push_back(purchased);
                            data.shopItemClassIndices.push_back(classIdx);
                        }

                        // Read last reset date
                        if (!file.eof()) {
                            size_t resetDateLen;
                            file.read(reinterpret_cast<char*>(&resetDateLen), sizeof(size_t));
                            if (resetDateLen > 0 && resetDateLen < 100) {
                                data.shopLastResetDate.resize(resetDateLen);
                                file.read(&data.shopLastResetDate[0], resetDateLen);
                            }
                        }
                    }
                }

                characters.push_back(data);
            }

            file.read(reinterpret_cast<char*>(&currentCharacterIndex), sizeof(int));
            file.close();

            std::cout << "[LOAD] Successfully loaded " << characters.size() << " character(s)" << std::endl;
            return true;

        }
        catch (const std::exception& e) {
            std::cerr << "[LOAD] Error loading characters: " << e.what() << std::endl;
            file.close();
            return false;
        }
    }

    void saveCharacters() {
        std::ofstream file(saveFilePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[SAVE] Failed to create save file!" << std::endl;
            return;
        }

        int count = static_cast<int>(characters.size());
        file.write(reinterpret_cast<const char*>(&count), sizeof(int));

        for (const auto& data : characters) {
            // Write name
            size_t nameLen = data.characterName.size();
            file.write(reinterpret_cast<const char*>(&nameLen), sizeof(size_t));
            file.write(data.characterName.c_str(), nameLen);

            // Write race name
            size_t raceNameLen = data.raceName.size();
            file.write(reinterpret_cast<const char*>(&raceNameLen), sizeof(size_t));
            file.write(data.raceName.c_str(), raceNameLen);

            // Write basic data
            file.write(reinterpret_cast<const char*>(&data.raceIndex), sizeof(int));
            file.write(reinterpret_cast<const char*>(&data.classIndex), sizeof(int));

            int factionInt = static_cast<int>(data.faction);
            file.write(reinterpret_cast<const char*>(&factionInt), sizeof(int));

            file.write(reinterpret_cast<const char*>(&data.level), sizeof(int));
            file.write(reinterpret_cast<const char*>(&data.currentXP), sizeof(double));
            file.write(reinterpret_cast<const char*>(&data.silver), sizeof(int));
            file.write(reinterpret_cast<const char*>(&data.gold), sizeof(int));
            file.write(reinterpret_cast<const char*>(&data.torches), sizeof(int));
            file.write(reinterpret_cast<const char*>(&data.mountType), sizeof(int));
            file.write(reinterpret_cast<const char*>(&data.mountRemainingDays), sizeof(int));

            // Write fatigue
            file.write(reinterpret_cast<const char*>(&data.fatigue), sizeof(float));
            size_t fatigueResetLen = data.lastFatigueResetDate.size();
            file.write(reinterpret_cast<const char*>(&fatigueResetLen), sizeof(size_t));
            file.write(data.lastFatigueResetDate.c_str(), fatigueResetLen);

            // Write inventory
            int inventoryCount = static_cast<int>(data.inventoryItems.size());
            file.write(reinterpret_cast<const char*>(&inventoryCount), sizeof(int));
            for (const auto& item : data.inventoryItems) {
                size_t itemNameLen = item.first.size();
                file.write(reinterpret_cast<const char*>(&itemNameLen), sizeof(size_t));
                file.write(item.first.c_str(), itemNameLen);
                file.write(reinterpret_cast<const char*>(&item.second), sizeof(int));
            }

            // Write equipment
            int equipmentCount = static_cast<int>(data.equipment.size());
            file.write(reinterpret_cast<const char*>(&equipmentCount), sizeof(int));
            for (const auto& equip : data.equipment) {
                file.write(reinterpret_cast<const char*>(&equip.first), sizeof(int));

                size_t itemNameLen = equip.second.size();
                file.write(reinterpret_cast<const char*>(&itemNameLen), sizeof(size_t));
                file.write(equip.second.c_str(), itemNameLen);
            }

            // Write brewer state
            file.write(reinterpret_cast<const char*>(&data.brewerState.isLocked), sizeof(bool));
            file.write(reinterpret_cast<const char*>(&data.brewerState.lastPurchaseTime), sizeof(long long));
            file.write(reinterpret_cast<const char*>(&data.brewerState.lockResetTime), sizeof(long long));

            // Write potion inventory
            int potionCount = static_cast<int>(data.potionInventory.size());
            file.write(reinterpret_cast<const char*>(&potionCount), sizeof(int));
            for (const auto& potion : data.potionInventory) {
                file.write(reinterpret_cast<const char*>(&potion.type), sizeof(int));
                file.write(reinterpret_cast<const char*>(&potion.rarity), sizeof(int));
                file.write(reinterpret_cast<const char*>(&potion.cost), sizeof(int));
                file.write(reinterpret_cast<const char*>(&potion.statBonus), sizeof(double));
                file.write(reinterpret_cast<const char*>(&potion.durationDays), sizeof(int));
                file.write(reinterpret_cast<const char*>(&potion.purchaseTime), sizeof(long long));
                file.write(reinterpret_cast<const char*>(&potion.isActive), sizeof(bool));
            }

            // Write curse data
            file.write(reinterpret_cast<const char*>(&data.curseId), sizeof(int));
            file.write(reinterpret_cast<const char*>(&data.curseHealthMod), sizeof(double));
            file.write(reinterpret_cast<const char*>(&data.curseAttackMod), sizeof(double));
            file.write(reinterpret_cast<const char*>(&data.curseIntelligenceMod), sizeof(double));
            file.write(reinterpret_cast<const char*>(&data.curseDexterityMod), sizeof(double));
            file.write(reinterpret_cast<const char*>(&data.curseArmorMod), sizeof(double));
            file.write(reinterpret_cast<const char*>(&data.curseLuckMod), sizeof(double));
            file.write(reinterpret_cast<const char*>(&data.hasFirstCurse), sizeof(bool));
            file.write(reinterpret_cast<const char*>(&data.unlockedClass), sizeof(bool));

            // Write shop state
            int shopItemCount = static_cast<int>(data.shopItemNames.size());
            file.write(reinterpret_cast<const char*>(&shopItemCount), sizeof(int));

            for (int j = 0; j < shopItemCount; ++j) {
                // Write item name
                size_t itemNameLen = data.shopItemNames[j].size();
                file.write(reinterpret_cast<const char*>(&itemNameLen), sizeof(size_t));
                file.write(data.shopItemNames[j].c_str(), itemNameLen);

                // Write item data
                int rarity = data.shopItemRarities[j];
                int price = data.shopItemPrices[j];
                bool purchased = data.shopItemPurchased[j];
                int classIdx = data.shopItemClassIndices[j];

                file.write(reinterpret_cast<const char*>(&rarity), sizeof(int));
                file.write(reinterpret_cast<const char*>(&price), sizeof(int));
                file.write(reinterpret_cast<const char*>(&purchased), sizeof(bool));
                file.write(reinterpret_cast<const char*>(&classIdx), sizeof(int));
            }

            // Write last reset date
            size_t resetDateLen = data.shopLastResetDate.size();
            file.write(reinterpret_cast<const char*>(&resetDateLen), sizeof(size_t));
            file.write(data.shopLastResetDate.c_str(), resetDateLen);
        }

        file.write(reinterpret_cast<const char*>(&currentCharacterIndex), sizeof(int));
        file.close();
    }

    // ============================================================================
    // NEW METHOD: Initialize loaded character into game systems
    // ============================================================================
    bool initializeLoadedCharacter(
        GameStateManager* stateManager,
        CurrencyManager* currency,
        SystemsManager* systems,
        float windowWidth,
        float windowHeight)
    {
        CharacterSaveData* currentChar = getCurrentCharacter();
        if (!currentChar) {
            std::cerr << "[LOAD] No current character to initialize!" << std::endl;
            return false;
        }

        std::cout << "\n[LOAD] Loading character: " << currentChar->characterName << std::endl;
        std::cout << "[LOAD DEBUG] Level: " << currentChar->level << " | XP: " << currentChar->currentXP << std::endl;
        std::cout << "[LOAD DEBUG] Silver: " << currentChar->silver << " | Gold: " << currentChar->gold << " | Torches: " << currentChar->torches << std::endl;
        std::cout << "[LOAD DEBUG] Inventory items: " << currentChar->inventoryItems.size() << std::endl;
        std::cout << "[LOAD DEBUG] Equipment items: " << currentChar->equipment.size() << std::endl;
        std::cout << "[LOAD DEBUG] Fatigue: " << currentChar->fatigue << std::endl;

        // Set state manager data
        stateManager->setPlayerFaction(currentChar->faction);
        stateManager->setPlayerRaceIndex(currentChar->raceIndex);
        stateManager->setPlayerRaceName(currentChar->raceName);
        stateManager->setPlayerClassIndex(currentChar->classIndex);

        // Set currency
        currency->setSilver(currentChar->silver);
        currency->setGold(currentChar->gold);
        currency->setTorches(currentChar->torches);

        // Set mount data
        MountData mount;
        mount.type = static_cast<MountType>(currentChar->mountType);
        mount.remainingDays = currentChar->mountRemainingDays;
        if (mount.type == MountType::Horse) {
            mount.speedBonus = 0.5f;
            mount.durationDays = 7;
        }
        else if (mount.type == MountType::Griffin) {
            mount.speedBonus = 0.75f;
            mount.durationDays = 14;
        }
        else {
            mount.speedBonus = 0.0f;
            mount.durationDays = 0;
            mount.cost = 0;
        }
        stateManager->setCurrentMount(mount);

        // Set game state to MainGame
        stateManager->setState(GameState::MainGame);

        // Create character sheet
        systems->createCharacterSheet(
            windowWidth,
            windowHeight,
            stateManager->getPlayerRaceIndex(),
            stateManager->getPlayerClassIndex(),
            stateManager->getPlayerFaction()
        );

        // Set shop to show items only for player's class
        if (systems->getShop()) {
            systems->getShop()->setPlayerClass(stateManager->getPlayerClassIndex());
        }

        // Initialize character sheet with saved data
        if (systems->getCharacterSheet()) {
            systems->getCharacterSheet()->setInventory(systems->getBasicInventory());

            // Get reference to mount data, then pass its address
            MountData& mountRef = stateManager->getCurrentMount();
            systems->getCharacterSheet()->setMountData(&mountRef);

            systems->getCharacterSheet()->addXP(currentChar->currentXP);

            // Load curse data
            if (currentChar->hasFirstCurse) {
                CurseData firstCurse(-1, "Blessing of Power", "Permanent +5% health bonus",
                    0.05, 0.0, 0.0, 0.0, 0.0, 0.0, true);
                systems->getCharacterSheet()->applyCurse(firstCurse);
                systems->getCharacterSheet()->setHasFirstCurse(true);
            }

            if (currentChar->curseId >= 0) {
                CurseData loadedCurse;
                loadedCurse.curseId = currentChar->curseId;
                loadedCurse.modifiers.healthModifier = currentChar->curseHealthMod;
                loadedCurse.modifiers.attackModifier = currentChar->curseAttackMod;
                loadedCurse.modifiers.intelligenceModifier = currentChar->curseIntelligenceMod;
                loadedCurse.modifiers.dexterityModifier = currentChar->curseDexterityMod;
                loadedCurse.modifiers.armorModifier = currentChar->curseArmorMod;
                loadedCurse.modifiers.luckModifier = currentChar->curseLuckMod;
                systems->getCharacterSheet()->applyCurse(loadedCurse);
            }

            if (currentChar->unlockedClass) {
                systems->getCharacterSheet()->setUnlockedClass(true);
            }
        }

        // Load inventory
        if (systems->getBasicInventory()) {
            systems->getBasicInventory()->clear();
            systems->getBasicInventory()->loadFromMap(currentChar->inventoryItems);
        }

        // Load equipment
        if (systems->getCharacterSheet()) {
            systems->getCharacterSheet()->loadEquipment(currentChar->equipment);
        }

        // Load brewer state
        if (systems->getBrewerSystem()) {
            systems->getBrewerSystem()->loadState(currentChar->brewerState);
        }

        // Load potion inventory
        if (systems->getPotionInventory()) {
            systems->getPotionInventory()->loadPotions(currentChar->potionInventory);
        }

        // Load shop state
        if (systems->getShop()) {
            ShopSaveData shopData;
            shopData.itemNames = currentChar->shopItemNames;
            shopData.itemRarities = currentChar->shopItemRarities;
            shopData.itemPrices = currentChar->shopItemPrices;
            shopData.itemPurchased = currentChar->shopItemPurchased;
            shopData.itemClassIndices = currentChar->shopItemClassIndices;
            shopData.lastResetDate = currentChar->shopLastResetDate;
            systems->getShop()->loadState(shopData);
        }

        // Set fatigue
        if (systems->getFatigueSystem()) {
            systems->getFatigueSystem()->setFatigueData(currentChar->fatigue, currentChar->lastFatigueResetDate);
        }

        std::cout << "[LOAD] Character initialization complete!" << std::endl;
        return true;
    }

    // ============================================================================
    // NEW METHOD: Save current character from game systems
    // ============================================================================
    bool saveCurrentCharacterFromSystems(
        GameStateManager* stateManager,
        CurrencyManager* currency,
        SystemsManager* systems)
    {
        CharacterSaveData* currentChar = getCurrentCharacter();
        if (!currentChar) {
            std::cerr << "[SAVE] No current character to save!" << std::endl;
            return false;
        }

        CharacterSaveData saveData;

        // Basic character info
        saveData.characterName = currentChar->characterName;
        saveData.raceIndex = stateManager->getPlayerRaceIndex();
        saveData.classIndex = stateManager->getPlayerClassIndex();
        saveData.faction = stateManager->getPlayerFaction();
        saveData.raceName = stateManager->getPlayerRaceName();

        // Level and XP
        if (systems->getCharacterSheet()) {
            saveData.level = systems->getCharacterSheet()->getLevel();
            saveData.currentXP = systems->getCharacterSheet()->getCurrentXP();
        }
        else {
            saveData.level = currentChar->level;
            saveData.currentXP = currentChar->currentXP;
        }

        // Currency
        saveData.silver = currency->getSilver();
        saveData.gold = currency->getGold();
        saveData.torches = currency->getTorches();

        // Mount data
        saveData.mountType = static_cast<int>(stateManager->getCurrentMount().type);
        saveData.mountRemainingDays = stateManager->getCurrentMount().remainingDays;

        // Fatigue
        if (systems->getFatigueSystem()) {
            saveData.fatigue = systems->getFatigueSystem()->getCurrentFatigue();
            saveData.lastFatigueResetDate = systems->getFatigueSystem()->getLastResetDate();
        }
        else {
            saveData.fatigue = currentChar->fatigue;
            saveData.lastFatigueResetDate = currentChar->lastFatigueResetDate;
        }

        // Inventory
        if (systems->getBasicInventory()) {
            saveData.inventoryItems = systems->getBasicInventory()->saveToMap();
        }

        // Equipment
        if (systems->getCharacterSheet()) {
            saveData.equipment = systems->getCharacterSheet()->saveEquipment();
        }

        // Brewer state
        if (systems->getBrewerSystem()) {
            systems->getBrewerSystem()->saveState(saveData.brewerState);
        }

        // Potion inventory
        if (systems->getPotionInventory()) {
            systems->getPotionInventory()->savePotions(saveData.potionInventory);
        }

        // Shop state
        if (systems->getShop()) {
            ShopSaveData shopData;
            systems->getShop()->saveState(shopData);
            saveData.shopItemNames = shopData.itemNames;
            saveData.shopItemRarities = shopData.itemRarities;
            saveData.shopItemPrices = shopData.itemPrices;
            saveData.shopItemPurchased = shopData.itemPurchased;
            saveData.shopItemClassIndices = shopData.itemClassIndices;
            saveData.shopLastResetDate = shopData.lastResetDate;
        }

        // Curse data
        if (systems->getCharacterSheet()) {
            CurseData currentCurse = systems->getCharacterSheet()->getCurrentCurse();
            saveData.curseId = currentCurse.curseId;
            saveData.curseHealthMod = currentCurse.modifiers.healthModifier;
            saveData.curseAttackMod = currentCurse.modifiers.attackModifier;
            saveData.curseIntelligenceMod = currentCurse.modifiers.intelligenceModifier;
            saveData.curseDexterityMod = currentCurse.modifiers.dexterityModifier;
            saveData.curseArmorMod = currentCurse.modifiers.armorModifier;
            saveData.curseLuckMod = currentCurse.modifiers.luckModifier;
            saveData.hasFirstCurse = systems->getCharacterSheet()->hasFirstCurseApplied();
            saveData.unlockedClass = systems->getCharacterSheet()->hasUnlockedClass();
        }

        // Update and save
        updateCurrentCharacter(saveData);

        std::cout << "[SAVE] Game progress saved successfully!" << std::endl;
        std::cout << "[SAVE DEBUG] Level: " << saveData.level << " | XP: " << saveData.currentXP << std::endl;
        std::cout << "[SAVE DEBUG] Silver: " << saveData.silver << " | Gold: " << saveData.gold << " | Torches: " << saveData.torches << std::endl;
        std::cout << "[SAVE DEBUG] Inventory items: " << saveData.inventoryItems.size() << std::endl;

        // Print each inventory item being saved
        std::cout << "[SAVE DEBUG] Inventory contents:" << std::endl;
        for (const auto& item : saveData.inventoryItems) {
            std::cout << "  - " << item.first << " x" << item.second << std::endl;
        }

        std::cout << "[SAVE DEBUG] Equipment items: " << saveData.equipment.size() << std::endl;
        std::cout << "[SAVE DEBUG] Fatigue: " << saveData.fatigue << std::endl;

        return true;
    }

    bool addCharacter(const CharacterSaveData& data) {
        if (!isValidName(data.characterName)) {
            std::cout << "VALIDATION FAILED: Name contains invalid characters (only letters and numbers allowed)" << std::endl;
            return false;
        }
        if (nameExists(data.characterName)) {
            std::cout << "VALIDATION FAILED: Character name already exists" << std::endl;
            return false;
        }
        characters.push_back(data);
        currentCharacterIndex = static_cast<int>(characters.size()) - 1;
        saveCharacters();
        return true;
    }

    void updateCurrentCharacter(const CharacterSaveData& data) {
        if (currentCharacterIndex >= 0 && currentCharacterIndex < characters.size()) {
            characters[currentCharacterIndex] = data;
            saveCharacters();
        }
    }

    CharacterSaveData* getCurrentCharacter() {
        if (currentCharacterIndex >= 0 && currentCharacterIndex < characters.size()) {
            return &characters[currentCharacterIndex];
        }
        return nullptr;
    }

    const std::vector<CharacterSaveData>& getAllCharacters() const {
        return characters;
    }

    void setCurrentCharacterIndex(int index) {
        if (index >= 0 && index < characters.size()) {
            currentCharacterIndex = index;
            saveCharacters();
        }
    }

    void deleteCharacter(int index) {
        if (index >= 0 && index < characters.size()) {
            characters.erase(characters.begin() + index);

            if (currentCharacterIndex >= characters.size()) {
                currentCharacterIndex = static_cast<int>(characters.size()) - 1;
            }
            if (currentCharacterIndex < 0 && !characters.empty()) {
                currentCharacterIndex = 0;
            }

            saveCharacters();
        }
    }

    int getCurrentCharacterIndex() const {
        return currentCharacterIndex;
    }

    bool hasCharacters() const {
        return !characters.empty();
    }
};