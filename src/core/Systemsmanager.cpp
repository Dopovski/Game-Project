#include "SystemsManager.h"
#include "CurrencyManager.h"

SystemsManager::SystemsManager(float width, float height) {
    // Initialize all systems
    shop = new Shop(width, height);
    fatigueSystem = new FatigueSystem(width - 1620.f, height - 170.f, 1600.f, 150.f);
    questDialog = new QuestDialog(width, height);
    arenaScreen = new ArenaScreen(width, height);
    characterSheet = nullptr;  // Created later per character
    brewerSystem = new BrewerSystem(width, height);
    potionInventory = new PotionInventory();
    menu = new Menu(width, height);
    fatiguedDialog = new FatiguedDialog(width, height);

    basicInventory = new BasicInventory();
    inventoryUI = new InventoryUI(width, height);

    riderScreen = new RiderScreen(width, height);

    curseDialog = new CurseDialog(width, height);

    fortressScreen = new FortressScreen(width, height);
}

SystemsManager::~SystemsManager() {
    delete shop;
    delete fatigueSystem;
    delete questDialog;
    delete arenaScreen;
    delete characterSheet;
    delete brewerSystem;
    delete potionInventory;
    delete menu;
    delete fatiguedDialog;

    delete basicInventory;
    delete inventoryUI;

    delete riderScreen;

    delete curseDialog;

    delete fortressScreen;
}

void SystemsManager::initializeAllSystems(const std::string& raceName, CurrencyManager* currency) {
    // Empty - systems initialized in constructor
}

void SystemsManager::updateSystems() {
    if (fatigueSystem) {
        fatigueSystem->update();
    }

    if (shop) {
        shop->update();
    }

    if (potionInventory) {
        potionInventory->update();
    }
}

void SystemsManager::createCharacterSheet(float width, float height, int raceIndex, int classIndex, Faction faction) {
    // CRITICAL: Delete old character sheet before creating new one
    if (characterSheet) {
        delete characterSheet;
        characterSheet = nullptr;
    }
    characterSheet = new CharacterSheet(width, height, raceIndex, classIndex, faction);
}