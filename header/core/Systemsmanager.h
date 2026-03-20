#pragma once
#include "Shop.h"
#include "FatigueSystem.h"
#include "QuestDialog.h"
#include "ArenaScreen.h"
#include "CharacterSheet.h"
#include "BrewerSystem.h"
#include "PotionInventory.h"
#include "Menu.h"
#include "FatiguedDialog.h"
#include "BasicInventory.h"
#include "InventoryUI.h"
#include "Rider.h"
#include "CurseDialog.h"
#include "FortressScreen.h"

class CurrencyManager;

class SystemsManager {
private:
    Shop* shop;
    FatigueSystem* fatigueSystem;
    QuestDialog* questDialog;
    ArenaScreen* arenaScreen;
    CharacterSheet* characterSheet;
    BrewerSystem* brewerSystem;
    PotionInventory* potionInventory;
    Menu* menu;
    FatiguedDialog* fatiguedDialog;

    BasicInventory* basicInventory;
    InventoryUI* inventoryUI;

    RiderScreen* riderScreen;

    CurseDialog* curseDialog;

    FortressScreen* fortressScreen;

public:
    SystemsManager(float width, float height);
    ~SystemsManager();

    void initializeAllSystems(const std::string& raceName, CurrencyManager* currency);
    void updateSystems();
    void createCharacterSheet(float width, float height, int raceIndex, int classIndex, Faction faction);

    // Getters
    Shop* getShop() { return shop; }
    FatigueSystem* getFatigueSystem() { return fatigueSystem; }
    QuestDialog* getQuestDialog() { return questDialog; }
    ArenaScreen* getArenaScreen() { return arenaScreen; }
    CharacterSheet* getCharacterSheet() { return characterSheet; }
    BrewerSystem* getBrewerSystem() { return brewerSystem; }
    PotionInventory* getPotionInventory() { return potionInventory; }
    Menu* getMenu() { return menu; }
    FatiguedDialog* getFatiguedDialog() { return fatiguedDialog; }

    BasicInventory* getBasicInventory() { return basicInventory; }
    InventoryUI* getInventoryUI() { return inventoryUI; }

    RiderScreen* getRiderScreen() { return riderScreen; }

    CurseDialog* getCurseDialog() { return curseDialog; }

    FortressScreen* getFortressScreen() { return fortressScreen; }
};