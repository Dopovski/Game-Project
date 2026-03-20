#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <ctime>

// Manager classes
#include "GameStateManager.h"
#include "CurrencyManager.h"
#include "UIManager.h"
#include "ResourceManager.h"
#include "SystemsManager.h"
#include "NPCManager.h"
#include "MissionManager.h"
#include "InputManager.h"

// Selection screens
#include "Faction.h"
#include "CharacterSelection.h"
#include "ClassSelection.h"
#include "NameInput.h"
#include "CharacterSwitchScreen.h"
#include "CurseSelection.h"

// Character persistence
#include "CharacterData.h"
#include "CharacterManager.h"
#include "SaveSystem.h"

// Other includes
#include "MissionRewardData.h"
#include "DoorSelection.h"
#include "ItemDatabase.h"
#include "Shop.h"  // For ShopSaveData

using namespace std;
using namespace sf;

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    RenderWindow window(VideoMode::getDesktopMode(), "Entente and Throng", State::Fullscreen);
    window.setVerticalSyncEnabled(true);

    Clock deltaClock;
    Vector2u win = window.getSize();

    CharacterManager characterManager;
    bool hasExistingCharacter = false;

    GameStateManager stateManager;
    CurrencyManager currency;
    UIManager uiManager(static_cast<float>(win.x), static_cast<float>(win.y));
    ResourceManager resources(static_cast<float>(win.x), static_cast<float>(win.y));
    SystemsManager systems(static_cast<float>(win.x), static_cast<float>(win.y));
    NPCManager npcManager;
    MissionManager missionManager;

    InputManager inputManager(
        &stateManager, &currency, &uiManager, &systems, &npcManager, &missionManager,
        static_cast<float>(win.x), static_cast<float>(win.y)
    );

    FactionSelection factionScreen(static_cast<float>(win.x), static_cast<float>(win.y));
    CharacterSelection* raceScreen = nullptr;
    ClassSelection* classScreen = nullptr;
    NameInputScreen* nameInputScreen = nullptr;
    CharacterSwitchScreen* switchScreen = nullptr;
    CurseSelection* curseScreen = nullptr;

    CharacterSheet* missionCharacterSheet = nullptr;
    Menu* missionMenu = nullptr;

    Font switchFont;

    // ============================================================================
    // STEP 1 REFACTOR: Character loading moved to CharacterManager
    // ============================================================================
    try {
        if (characterManager.loadCharacters() && characterManager.hasCharacters()) {
            hasExistingCharacter = true;

            // Initialize all game systems with loaded character data
            if (characterManager.initializeLoadedCharacter(
                &stateManager,
                &currency,
                &systems,
                static_cast<float>(win.x),
                static_cast<float>(win.y))) {

                // Load background and NPCs
                if (!resources.loadBackground("background/bar1.jpg")) {
                    cout << "Error loading background!" << endl;
                }
                npcManager.initializeNPCs(static_cast<float>(win.x), static_cast<float>(win.y));

                // Initialize UI buttons
                uiManager.initCharacterButton();
                uiManager.initInventoryButton();
                if (switchFont.openFromFile("arial.ttf")) {
                    uiManager.initCharacterSwitchButton(switchFont);
                }
                uiManager.registerButtons();
                currency.initUI();

                // Ensure clean dungeon/main game state - close all other screens
                if (systems.getShop()) systems.getShop()->hide();
                if (systems.getCharacterSheet()) systems.getCharacterSheet()->hide();
                if (systems.getArenaScreen()) systems.getArenaScreen()->hide();
                if (systems.getBrewerSystem()) systems.getBrewerSystem()->hide();
                if (systems.getRiderScreen()) systems.getRiderScreen()->hide();

                // Set mission pointers
                missionCharacterSheet = systems.getCharacterSheet();
                missionMenu = systems.getMenu();

                // Initialize potion inventory UI
                if (systems.getPotionInventory()) {
                    systems.getPotionInventory()->initUI(
                        static_cast<float>(win.x) - 400.f,
                        static_cast<float>(win.y) / 2.f + 100.f
                    );
                }
            }
        }
    }
    catch (const std::exception& e) {
        cout << "Error loading character: " << e.what() << endl;
        cout << "Starting new game..." << endl;
        hasExistingCharacter = false;
        stateManager.setState(GameState::FactionSelection);
    }
    // ============================================================================
    while (window.isOpen()) {
        float deltaTime = deltaClock.restart().asSeconds();

        while (const optional event = window.pollEvent()) {
            // ====================================================================
            // FIXED: Single Event::Closed handler with debug output
            // ====================================================================
            if (event->is<Event::Closed>()) {
                std::cout << "\n===== CLOSING GAME =====" << std::endl;
                std::cout << "hasExistingCharacter: " << (hasExistingCharacter ? "true" : "false") << std::endl;
                std::cout << "Current state: " << static_cast<int>(stateManager.getCurrentState()) << std::endl;

                // Save game state for all active gameplay states (not just MainGame)
                if (hasExistingCharacter &&
                    (stateManager.getCurrentState() == GameState::MainGame ||
                        stateManager.getCurrentState() == GameState::OnMission ||
                        stateManager.getCurrentState() == GameState::DoorChoice ||
                        stateManager.getCurrentState() == GameState::Combat)) {

                    std::cout << "ATTEMPTING SAVE..." << std::endl;
                    characterManager.saveCurrentCharacterFromSystems(&stateManager, &currency, &systems);
                    std::cout << "SAVE COMPLETE!" << std::endl;
                }
                else {
                    std::cout << "SAVE SKIPPED - ";
                    if (!hasExistingCharacter) {
                        std::cout << "No existing character flag set" << std::endl;
                    }
                    else {
                        std::cout << "Not in valid gameplay state" << std::endl;
                    }
                }

                window.close();
            }
            // ====================================================================

            if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                inputManager.handleTextInput(*textEvent, nameInputScreen);
            }

            if (event->is<Event::MouseMoved>()) {
                inputManager.handleMouseMoved(window, &factionScreen);
            }

            // Mouse button handling remains here due to complexity (~1400 lines)
            // TODO: Future refactoring - break into smaller handler methods
            if (const auto* mouseButton = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseButton->button == Mouse::Button::Left) {

                    // FIRST: Check for sell confirmation popup (highest priority - before anything else)
                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible() &&
                        systems.getCharacterSheet()->isShowingSellConfirmation()) {

                        CharacterSheet* charSheet = systems.getCharacterSheet();

                        // Check if clicked "Sure" button - sell the item
                        if (charSheet->checkConfirmSellClick(window)) {
                            string itemToSell = charSheet->getSelectedInventoryItem();
                            int sellPrice = charSheet->getSellPrice();

                            if (!itemToSell.empty() && systems.getBasicInventory()) {
                                cout << "[SELL DEBUG] Attempting to sell: " << itemToSell << endl;
                                cout << "[SELL DEBUG] Items before remove: " << systems.getBasicInventory()->getAllItems().size() << endl;

                                // Remove item from inventory
                                bool removed = systems.getBasicInventory()->removeItem(itemToSell);

                                cout << "[SELL DEBUG] removeItem returned: " << (removed ? "TRUE" : "FALSE") << endl;
                                cout << "[SELL DEBUG] Items after remove: " << systems.getBasicInventory()->getAllItems().size() << endl;

                                // Add silver to currency
                                currency.addSilver(sellPrice);

                                // Clear selection
                                charSheet->clearInventorySelection();

                                // Update displays
                                charSheet->updateInventoryDisplay();

                                cout << "Sold " << itemToSell << " for " << sellPrice << " silver!" << endl;
                            }
                            continue;
                        }

                        // Check if clicked "No" button - cancel sell
                        if (charSheet->checkCancelSellClick(window)) {
                            continue;
                        }

                        // Check if clicked outside popup - close it
                        if (charSheet->checkClickOutsidePopup(window)) {
                            continue;
                        }

                        // If popup is showing, don't process other clicks
                        continue;
                    }

                    // EXCLUSIVE BUTTON HANDLING - STATE INDEPENDENT
                    uiManager.handleButtonClick(window);

                    // Check for equipment slot clicks (unequip system)
                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                        systems.getCharacterSheet()->checkEquippedItemClick(window);
                    }

                    // Update character sheet visibility based on button state
                    if (systems.getCharacterSheet()) {
                        if (uiManager.isCharacterSheetActive()) {
                            if (!systems.getCharacterSheet()->isVisible()) {
                                systems.getCharacterSheet()->show();
                            }
                        }
                        else {
                            if (systems.getCharacterSheet()->isVisible()) {
                                systems.getCharacterSheet()->hide();
                            }
                        }
                    }

                    // Update switch screen based on button state
                    if (hasExistingCharacter && uiManager.isSwitchScreenActive()) {
                        if (!switchScreen) {
                            CharacterSaveData saveData;
                            CharacterSaveData* currentChar = characterManager.getCurrentCharacter();
                            if (currentChar) {
                                saveData.characterName = currentChar->characterName;
                                saveData.raceIndex = stateManager.getPlayerRaceIndex();
                                saveData.classIndex = stateManager.getPlayerClassIndex();
                                saveData.faction = stateManager.getPlayerFaction();
                                saveData.raceName = stateManager.getPlayerRaceName();

                                if (systems.getCharacterSheet()) {
                                    saveData.level = systems.getCharacterSheet()->getLevel();
                                    saveData.currentXP = systems.getCharacterSheet()->getCurrentXP();
                                }

                                saveData.silver = currency.getSilver();
                                saveData.gold = currency.getGold();
                                saveData.torches = currency.getTorches();
                                saveData.mountType = static_cast<int>(stateManager.getCurrentMount().type);
                                saveData.mountRemainingDays = stateManager.getCurrentMount().remainingDays;

                                if (systems.getFatigueSystem()) {
                                    saveData.fatigue = systems.getFatigueSystem()->getCurrentFatigue();
                                    saveData.lastFatigueResetDate = systems.getFatigueSystem()->getLastResetDate();
                                }

                                saveData.inventoryItems = systems.getBasicInventory()->saveToMap();

                                // Save equipment
                                if (systems.getCharacterSheet()) {
                                    saveData.equipment = systems.getCharacterSheet()->saveEquipment();
                                }

                                if (systems.getBrewerSystem()) {
                                    systems.getBrewerSystem()->saveState(saveData.brewerState);
                                }

                                if (systems.getPotionInventory()) {
                                    systems.getPotionInventory()->savePotions(saveData.potionInventory);
                                }

                                // Save shop state
                                if (systems.getShop()) {
                                    ShopSaveData shopData;
                                    systems.getShop()->saveState(shopData);
                                    saveData.shopItemNames = shopData.itemNames;
                                    saveData.shopItemRarities = shopData.itemRarities;
                                    saveData.shopItemPrices = shopData.itemPrices;
                                    saveData.shopItemPurchased = shopData.itemPurchased;
                                    saveData.shopItemClassIndices = shopData.itemClassIndices;
                                    saveData.shopLastResetDate = shopData.lastResetDate;
                                }

                                characterManager.updateCurrentCharacter(saveData);
                            }

                            switchScreen = new CharacterSwitchScreen(
                                static_cast<float>(win.x),
                                static_cast<float>(win.y),
                                characterManager.getAllCharacters()
                            );
                        }
                    }
                    else if (!uiManager.isSwitchScreenActive() && switchScreen) {
                        delete switchScreen;
                        switchScreen = nullptr;
                    }

                    if (switchScreen) {
                        if (switchScreen->isShowingDeleteConfirm()) {
                            if (switchScreen->isDeleteConfirmed(window)) {
                                int deleteIdx = switchScreen->getDeleteIndex();
                                if (deleteIdx != -1) {
                                    cout << "Deleting character at index " << deleteIdx << endl;

                                    bool deletingCurrent = (deleteIdx == characterManager.getCurrentCharacterIndex());

                                    characterManager.deleteCharacter(deleteIdx);

                                    delete switchScreen;
                                    switchScreen = nullptr;

                                    if (!characterManager.hasCharacters()) {
                                        hasExistingCharacter = false;
                                        stateManager.setState(GameState::FactionSelection);
                                        uiManager.forceDeactivateAll();
                                    }
                                    else if (deletingCurrent) {
                                        CharacterSaveData* newChar = characterManager.getCurrentCharacter();
                                        if (newChar) {
                                            stateManager.setPlayerFaction(newChar->faction);
                                            stateManager.setPlayerRaceIndex(newChar->raceIndex);
                                            stateManager.setPlayerRaceName(newChar->raceName);
                                            stateManager.setPlayerClassIndex(newChar->classIndex);

                                            currency.setSilver(newChar->silver);
                                            currency.setGold(newChar->gold);
                                            currency.setTorches(newChar->torches);

                                            MountData mount;
                                            mount.type = static_cast<MountType>(newChar->mountType);
                                            mount.remainingDays = newChar->mountRemainingDays;
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
                                            stateManager.setCurrentMount(mount);

                                            systems.createCharacterSheet(
                                                static_cast<float>(win.x),
                                                static_cast<float>(win.y),
                                                newChar->raceIndex,
                                                newChar->classIndex,
                                                newChar->faction
                                            );

                                            // Set shop to show items only for player's class
                                            systems.getShop()->setPlayerClass(newChar->classIndex);

                                            if (systems.getCharacterSheet()) {
                                                systems.getCharacterSheet()->setInventory(systems.getBasicInventory());
                                                systems.getCharacterSheet()->setMountData(&stateManager.getCurrentMount());
                                                systems.getCharacterSheet()->addXP(newChar->currentXP);
                                            }

                                            // Load inventory
                                            systems.getBasicInventory()->clear();
                                            systems.getBasicInventory()->loadFromMap(newChar->inventoryItems);

                                            // Load equipment
                                            if (systems.getCharacterSheet()) {
                                                systems.getCharacterSheet()->loadEquipment(newChar->equipment);
                                            }

                                            // Load brewer state
                                            if (systems.getBrewerSystem()) {
                                                systems.getBrewerSystem()->loadState(newChar->brewerState);
                                            }

                                            // Load potion inventory
                                            if (systems.getPotionInventory()) {
                                                systems.getPotionInventory()->loadPotions(newChar->potionInventory);
                                            }

                                            // Load shop state
                                            if (systems.getShop()) {
                                                ShopSaveData shopData;
                                                shopData.itemNames = newChar->shopItemNames;
                                                shopData.itemRarities = newChar->shopItemRarities;
                                                shopData.itemPrices = newChar->shopItemPrices;
                                                shopData.itemPurchased = newChar->shopItemPurchased;
                                                shopData.itemClassIndices = newChar->shopItemClassIndices;
                                                shopData.lastResetDate = newChar->shopLastResetDate;
                                                systems.getShop()->loadState(shopData);
                                            }

                                            if (systems.getFatigueSystem()) {
                                                systems.getFatigueSystem()->setFatigueData(newChar->fatigue, newChar->lastFatigueResetDate);
                                            }

                                            missionCharacterSheet = systems.getCharacterSheet();
                                            missionMenu = systems.getMenu();
                                        }
                                    }
                                    else {
                                        switchScreen = new CharacterSwitchScreen(
                                            static_cast<float>(win.x),
                                            static_cast<float>(win.y),
                                            characterManager.getAllCharacters()
                                        );
                                    }
                                }
                            }
                            continue;
                        }

                        if (switchScreen->checkDeleteClick(window) != -1) {
                            continue;
                        }

                        int clickedSlot = switchScreen->checkSlotClick(window);
                        if (clickedSlot != -1) {
                            characterManager.setCurrentCharacterIndex(clickedSlot);
                            CharacterSaveData* newChar = characterManager.getCurrentCharacter();

                            if (newChar) {
                                stateManager.setPlayerFaction(newChar->faction);
                                stateManager.setPlayerRaceIndex(newChar->raceIndex);
                                stateManager.setPlayerRaceName(newChar->raceName);
                                stateManager.setPlayerClassIndex(newChar->classIndex);

                                currency.setSilver(newChar->silver);
                                currency.setGold(newChar->gold);
                                currency.setTorches(newChar->torches);

                                MountData mount;
                                mount.type = static_cast<MountType>(newChar->mountType);
                                mount.remainingDays = newChar->mountRemainingDays;
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
                                stateManager.setCurrentMount(mount);

                                systems.createCharacterSheet(
                                    static_cast<float>(win.x),
                                    static_cast<float>(win.y),
                                    newChar->raceIndex,
                                    newChar->classIndex,
                                    newChar->faction
                                );

                                // Set shop to show items only for player's class
                                systems.getShop()->setPlayerClass(newChar->classIndex);

                                if (systems.getCharacterSheet()) {
                                    systems.getCharacterSheet()->setInventory(systems.getBasicInventory());
                                    systems.getCharacterSheet()->setMountData(&stateManager.getCurrentMount());
                                    systems.getCharacterSheet()->addXP(newChar->currentXP);
                                }

                                // Load inventory
                                systems.getBasicInventory()->clear();
                                systems.getBasicInventory()->loadFromMap(newChar->inventoryItems);

                                // Load equipment
                                if (systems.getCharacterSheet()) {
                                    systems.getCharacterSheet()->loadEquipment(newChar->equipment);
                                }

                                // Load brewer state
                                if (systems.getBrewerSystem()) {
                                    systems.getBrewerSystem()->loadState(newChar->brewerState);
                                }

                                // Load potion inventory
                                if (systems.getPotionInventory()) {
                                    systems.getPotionInventory()->loadPotions(newChar->potionInventory);
                                }

                                // Load shop state
                                if (systems.getShop()) {
                                    ShopSaveData shopData;
                                    shopData.itemNames = newChar->shopItemNames;
                                    shopData.itemRarities = newChar->shopItemRarities;
                                    shopData.itemPrices = newChar->shopItemPrices;
                                    shopData.itemPurchased = newChar->shopItemPurchased;
                                    shopData.itemClassIndices = newChar->shopItemClassIndices;
                                    shopData.lastResetDate = newChar->shopLastResetDate;
                                    systems.getShop()->loadState(shopData);
                                }

                                if (systems.getFatigueSystem()) {
                                    systems.getFatigueSystem()->setFatigueData(newChar->fatigue, newChar->lastFatigueResetDate);
                                }

                                // Load curse data
                                if (systems.getCharacterSheet()) {
                                    if (newChar->hasFirstCurse) {
                                        CurseData firstCurse(-1, "Blessing of Power", "Permanent +5% health bonus", 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, true);
                                        systems.getCharacterSheet()->applyCurse(firstCurse);
                                        systems.getCharacterSheet()->setHasFirstCurse(true);
                                    }

                                    if (newChar->curseId >= 0) {
                                        CurseData loadedCurse;
                                        loadedCurse.curseId = newChar->curseId;
                                        loadedCurse.modifiers.healthModifier = newChar->curseHealthMod;
                                        loadedCurse.modifiers.attackModifier = newChar->curseAttackMod;
                                        loadedCurse.modifiers.intelligenceModifier = newChar->curseIntelligenceMod;
                                        loadedCurse.modifiers.dexterityModifier = newChar->curseDexterityMod;
                                        loadedCurse.modifiers.armorModifier = newChar->curseArmorMod;
                                        loadedCurse.modifiers.luckModifier = newChar->curseLuckMod;
                                        systems.getCharacterSheet()->applyCurse(loadedCurse);
                                    }

                                    if (newChar->unlockedClass) {
                                        systems.getCharacterSheet()->setUnlockedClass(true);
                                    }
                                }

                                missionCharacterSheet = systems.getCharacterSheet();
                                missionMenu = systems.getMenu();

                                // Ensure clean dungeon/main game state after character switch - close all other screens
                                if (systems.getShop()) systems.getShop()->hide();
                                if (systems.getCharacterSheet()) systems.getCharacterSheet()->hide();
                                if (systems.getArenaScreen()) systems.getArenaScreen()->hide();
                                if (systems.getBrewerSystem()) systems.getBrewerSystem()->hide();
                                if (systems.getRiderScreen()) systems.getRiderScreen()->hide();
                                if (systems.getQuestDialog()) systems.getQuestDialog()->hide();

                                uiManager.forceDeactivateAll();
                                delete switchScreen;
                                switchScreen = nullptr;
                            }
                            continue;
                        }
                        else if (switchScreen->isCreateNewClicked(window)) {
                            delete switchScreen;
                            switchScreen = nullptr;
                            hasExistingCharacter = false;
                            stateManager.setState(GameState::FactionSelection);
                            uiManager.forceDeactivateAll();
                            continue;
                        }
                        continue;
                    }

                    if (stateManager.getCurrentState() == GameState::FactionSelection) {
                        Faction clicked = factionScreen.checkClick(window);
                        if (clicked != Faction::None) {
                            stateManager.setPlayerFaction(clicked);
                            stateManager.setState(GameState::RaceSelection);

                            cout << "Joined " << (clicked == Faction::Entente ? "ENTENTE" : "THRONG") << "!" << endl;

                            raceScreen = new CharacterSelection(
                                static_cast<float>(win.x),
                                static_cast<float>(win.y),
                                clicked
                            );
                        }
                    }

                    else if (stateManager.getCurrentState() == GameState::RaceSelection && raceScreen) {
                        if (raceScreen->isConfirmClicked(window)) {
                            stateManager.setPlayerRaceIndex(raceScreen->getSelectedCharacterIndex());
                            stateManager.setState(GameState::ClassSelection);

                            vector<string> races = (stateManager.getPlayerFaction() == Faction::Entente)
                                ? vector<string>{"Human", "Dwarf", "Golem"}
                            : vector<string>{ "Goblin", "Elf", "Orc" };
                            stateManager.setPlayerRaceName(races[stateManager.getPlayerRaceIndex()]);

                            cout << "Race: " << stateManager.getPlayerRaceName() << endl;

                            classScreen = new ClassSelection(
                                static_cast<float>(win.x),
                                static_cast<float>(win.y)
                            );
                        }
                        else {
                            int clickedIcon = raceScreen->checkIconClick(window);
                            if (clickedIcon != -1) {
                                raceScreen->selectCharacter(clickedIcon);
                            }
                        }
                    }

                    else if (stateManager.getCurrentState() == GameState::ClassSelection && classScreen) {
                        if (classScreen->isConfirmClicked(window)) {
                            stateManager.setPlayerClassIndex(classScreen->getSelectedClassIndex());
                            stateManager.setState(GameState::NameInput);

                            cout << "Class: " << stateManager.getPlayerClassIndex() << endl;

                            nameInputScreen = new NameInputScreen(
                                static_cast<float>(win.x),
                                static_cast<float>(win.y)
                            );
                        }
                        else {
                            int clickedIcon = classScreen->checkIconClick(window);
                            if (clickedIcon != -1) {
                                classScreen->selectClass(clickedIcon);
                            }
                        }
                    }

                    else if (stateManager.getCurrentState() == GameState::NameInput && nameInputScreen) {
                        if (nameInputScreen->isConfirmClicked(window)) {
                            string characterName = nameInputScreen->getInputName();

                            CharacterSaveData newCharacter;
                            newCharacter.characterName = characterName;
                            newCharacter.raceIndex = stateManager.getPlayerRaceIndex();
                            newCharacter.classIndex = stateManager.getPlayerClassIndex();
                            newCharacter.faction = stateManager.getPlayerFaction();
                            newCharacter.raceName = stateManager.getPlayerRaceName();
                            newCharacter.level = 1;
                            newCharacter.currentXP = 0.0;
                            newCharacter.silver = 0;
                            newCharacter.gold = 25;
                            newCharacter.torches = 0;
                            newCharacter.mountType = 0;
                            newCharacter.mountRemainingDays = 0;

                            newCharacter.fatigue = 0.0f;

                            time_t now = time(0);
                            tm* ltm = localtime(&now);
                            char buffer[11];
                            sprintf(buffer, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
                            newCharacter.lastFatigueResetDate = std::string(buffer);

                            // Empty inventory for new character
                            newCharacter.inventoryItems.clear();

                            // Empty brewer state for new character
                            newCharacter.brewerState = BrewerStateData();

                            // Empty potion inventory for new character
                            newCharacter.potionInventory.clear();

                            if (!characterManager.addCharacter(newCharacter)) {
                                cout << "Character creation failed: Name already exists or invalid (letters/numbers only)" << endl;
                                continue;
                            }

                            cout << "Character Name: " << characterName << endl;

                            hasExistingCharacter = true;

                            currency.setSilver(0);
                            currency.setGold(25);
                            currency.setTorches(0);

                            // Reset mount data for new character
                            MountData emptyMount;
                            emptyMount.type = static_cast<MountType>(0);
                            emptyMount.cost = 0;
                            emptyMount.speedBonus = 0.0f;
                            emptyMount.durationDays = 0;
                            emptyMount.remainingDays = 0;
                            stateManager.setCurrentMount(emptyMount);

                            stateManager.setState(GameState::MainGame);

                            if (!resources.loadBackground("background/bar1.jpg")) {
                                cout << "Error loading background!" << endl;
                                return 1;
                            }

                            npcManager.initializeNPCs(
                                static_cast<float>(win.x),
                                static_cast<float>(win.y)
                            );

                            vector<string> races = (stateManager.getPlayerFaction() == Faction::Entente)
                                ? vector<string>{"Human", "Dwarf", "Golem"}
                            : vector<string>{ "Goblin", "Elf", "Orc" };

                            systems.createCharacterSheet(
                                static_cast<float>(win.x),
                                static_cast<float>(win.y),
                                stateManager.getPlayerRaceIndex(),
                                stateManager.getPlayerClassIndex(),
                                stateManager.getPlayerFaction()
                            );

                            // Set shop to show items only for player's class
                            systems.getShop()->setPlayerClass(stateManager.getPlayerClassIndex());
                            // Force generate new shop for new character
                            systems.getShop()->forceRefresh();

                            systems.getCharacterSheet()->setInventory(systems.getBasicInventory());
                            systems.getCharacterSheet()->setMountData(&stateManager.getCurrentMount());

                            // Apply first curse automatically
                            if (systems.getCharacterSheet() && !systems.getCharacterSheet()->hasFirstCurseApplied()) {
                                CurseData firstCurse(-1, "Blessing of Power", "Permanent +5% health bonus", 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, true);
                                systems.getCharacterSheet()->applyCurse(firstCurse);
                                systems.getCharacterSheet()->setHasFirstCurse(true);
                                systems.getCharacterSheet()->setNeedsCurseSelection(false);
                            }

                            // Clear all systems for new character
                            systems.getBasicInventory()->clear();

                            if (systems.getPotionInventory()) {
                                systems.getPotionInventory()->clearAll();
                            }

                            if (systems.getFatigueSystem()) {
                                CharacterSaveData* currentChar = characterManager.getCurrentCharacter();
                                if (currentChar) {
                                    systems.getFatigueSystem()->setFatigueData(currentChar->fatigue, currentChar->lastFatigueResetDate);
                                }
                            }

                            uiManager.initCharacterButton();
                            uiManager.initInventoryButton();
                            if (switchFont.openFromFile("arial.ttf")) {
                                uiManager.initCharacterSwitchButton(switchFont);
                            }
                            uiManager.registerButtons();
                            currency.initUI();

                            if (systems.getPotionInventory()) {
                                systems.getPotionInventory()->initUI(
                                    static_cast<float>(win.x) - 400.f,
                                    static_cast<float>(win.y) / 2.f + 100.f
                                );
                            }

                            missionCharacterSheet = systems.getCharacterSheet();
                            missionMenu = systems.getMenu();

                            // Ensure clean dungeon/main game state - close all other screens
                            if (systems.getShop()) systems.getShop()->hide();
                            if (systems.getCharacterSheet()) systems.getCharacterSheet()->hide();
                            if (systems.getArenaScreen()) systems.getArenaScreen()->hide();
                            if (systems.getBrewerSystem()) systems.getBrewerSystem()->hide();
                            if (systems.getRiderScreen()) systems.getRiderScreen()->hide();

                            delete nameInputScreen;
                            nameInputScreen = nullptr;
                        }
                    }


                    else if (stateManager.getCurrentState() == GameState::CurseSelection && curseScreen) {
                        curseScreen->handleClick(window);

                        if (curseScreen->isSelectionMade()) {
                            if (curseScreen->isAccepted()) {
                                if (systems.getCharacterSheet()) {
                                    CurseSelection tempCurseGen(static_cast<float>(win.x), static_cast<float>(win.y));
                                    CurseData newCurse = tempCurseGen.generateRandomCurse(
                                        systems.getCharacterSheet()->getLevel(),
                                        systems.getCharacterSheet()->getFaction()
                                    );
                                    systems.getCharacterSheet()->applyCurse(newCurse);
                                }
                            }

                            if (systems.getCharacterSheet()) {
                                systems.getCharacterSheet()->setNeedsCurseSelection(false);
                            }

                            delete curseScreen;
                            curseScreen = nullptr;
                            stateManager.setState(stateManager.getPreviousState());
                        }
                    }
                    else if (stateManager.getCurrentState() == GameState::MainGame) {

                        if (systems.getFatiguedDialog() && systems.getFatiguedDialog()->isVisible()) {
                            if (systems.getFatiguedDialog()->isCloseButtonClicked(window)) {
                                systems.getFatiguedDialog()->hide();
                                continue;
                            }
                            continue;
                        }

                        if (systems.getCurseDialog() && systems.getCurseDialog()->isVisible()) {
                            if (systems.getCurseDialog()->isCloseButtonClicked(window)) {
                                systems.getCurseDialog()->hide();
                                continue;
                            }
                            continue;
                        }

                        if (systems.getFatigueSystem() && systems.getFatigueSystem()->isRefillButtonClicked(window)) {
                            if (currency.getGold() >= 1) {
                                currency.removeGold(1);
                                systems.getFatigueSystem()->refillFatigue(20.0f);
                                cout << "Refilled 20% fatigue for 1 gold!" << endl;
                            }
                            else {
                                cout << "Not enough gold to refill fatigue!" << endl;
                            }
                            continue;
                        }

                        if (systems.getMenu()) {
                            int clickedMenuButton = systems.getMenu()->getClickedItemIndex(window);
                            if (clickedMenuButton != -1) {
                                cout << "Menu button " << clickedMenuButton << " clicked!" << endl;

                                if (clickedMenuButton == 0) {
                                    // Dungeons button - close all screens
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                    }
                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }
                                    if (systems.getFortressScreen() && systems.getFortressScreen()->isVisible()) {
                                        systems.getFortressScreen()->hide();
                                    }
                                }
                                else if (clickedMenuButton == 2) {
                                    // Shop button
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }
                                    if (systems.getFortressScreen() && systems.getFortressScreen()->isVisible()) {
                                        systems.getFortressScreen()->hide();
                                    }

                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                        if (systems.getShop()) systems.getShop()->show();
                                    }
                                    else if (systems.getShop() && systems.getShop()->isVisible()) {
                                        systems.getShop()->hide();
                                    }
                                    else {
                                        if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                            systems.getArenaScreen()->hide();
                                        }
                                        if (systems.getShop()) systems.getShop()->show();
                                    }
                                }
                                else if (clickedMenuButton == 1) {
                                    // Arena button
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }
                                    if (systems.getFortressScreen() && systems.getFortressScreen()->isVisible()) {
                                        systems.getFortressScreen()->hide();
                                    }

                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    else {
                                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                            systems.getCharacterSheet()->hide();
                                            uiManager.forceDeactivateAll();
                                        }
                                        if (systems.getArenaScreen()) systems.getArenaScreen()->show();
                                    }
                                }
                                else if (clickedMenuButton == 3) {
                                    // Brewer button
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }
                                    if (systems.getFortressScreen() && systems.getFortressScreen()->isVisible()) {
                                        systems.getFortressScreen()->hide();
                                    }

                                    if (systems.getBrewerSystem()) {
                                        if (systems.getBrewerSystem()->isVisible()) {
                                            systems.getBrewerSystem()->hide();
                                        }
                                        else {
                                            systems.getBrewerSystem()->show();
                                        }
                                    }
                                }
                                else if (clickedMenuButton == 4) {
                                    // Rider button
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                    }
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getFortressScreen() && systems.getFortressScreen()->isVisible()) {
                                        systems.getFortressScreen()->hide();
                                    }

                                    if (systems.getRiderScreen()) {
                                        if (systems.getRiderScreen()->isVisible()) {
                                            systems.getRiderScreen()->hide();
                                        }
                                        else {
                                            systems.getRiderScreen()->show();
                                        }
                                    }
                                }
                                else if (clickedMenuButton == 5) {
                                    // Fortress button
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                    }
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }

                                    if (systems.getFortressScreen()) {
                                        if (systems.getFortressScreen()->isVisible()) {
                                            systems.getFortressScreen()->hide();
                                        }
                                        else {
                                            systems.getFortressScreen()->show();
                                        }
                                    }
                                }
                                else if (clickedMenuButton == 6) {
                                    // Curse button
                                    if (systems.getCharacterSheet()) {
                                        if (systems.getCharacterSheet()->getLevel() < 25) {
                                            systems.getCurseDialog()->show("You are not level 25.\\nCome back later!");
                                        }
                                        else {
                                            systems.getCurseDialog()->show("Curse system available!");
                                        }
                                    }
                                }
                                // Buttons 5,6,7,8 (Fortress, Curse, Conquerors, Guild) not yet implemented
                                continue;
                            }
                        }


                        // Handle character sheet inventory item click for equipping
                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible() &&
                            systems.getBasicInventory()) {
                            string clickedItem = systems.getCharacterSheet()->checkInventoryItemClick(window);
                            if (!clickedItem.empty()) {
                                cout << "DEBUG: Clicked item in character sheet: " << clickedItem << endl;

                                // Get item slot from database
                                EquipmentSlot slot = ItemDatabase::getInstance().getItemSlot(clickedItem);

                                // Equip the item (passing inventory for swapping)
                                systems.getCharacterSheet()->equipItem(slot, clickedItem, systems.getBasicInventory());

                                // Remove from inventory
                                systems.getBasicInventory()->removeItem(clickedItem);

                                // Update character sheet inventory display
                                systems.getCharacterSheet()->updateInventoryDisplay();

                                cout << "Equipped: " << clickedItem << " to slot " << (int)slot << endl;
                                continue;
                            }
                        }
                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                            if (systems.getPotionInventory()) {
                                int potionIndex = systems.getPotionInventory()->checkUseButtonClick(window);
                                if (potionIndex != -1) {
                                    systems.getPotionInventory()->usePotion(potionIndex);
                                    continue;
                                }
                            }
                            continue;
                        }

                        if (systems.getShop() && systems.getShop()->isVisible()) {
                            if (systems.getShop()->checkCheatClick(window, currency)) {
                                continue;
                            }
                            if (systems.getShop()->checkRefreshClick(window, currency)) {
                                continue;
                            }
                            if (systems.getShop()->checkItemClick(window, currency, systems.getBasicInventory())) {
                                continue;
                            }
                        }


                        // Handle inventory item click for equipping (sell handled globally above)
                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible() &&
                            systems.getBasicInventory() && !systems.getCharacterSheet()->isShowingSellConfirmation()) {

                            CharacterSheet* charSheet = systems.getCharacterSheet();

                            // Normal item click handling (equip via double-click or button)
                            string clickedItem = charSheet->checkInventoryItemClick(window);
                            if (!clickedItem.empty()) {
                                // Get item slot from database
                                EquipmentSlot slot = ItemDatabase::getInstance().getItemSlot(clickedItem);

                                // Equip the item (passing inventory for swapping)
                                charSheet->equipItem(slot, clickedItem, systems.getBasicInventory());

                                // Remove from inventory
                                systems.getBasicInventory()->removeItem(clickedItem);

                                // Update inventory display
                                charSheet->updateInventoryDisplay();

                                cout << "Equipped: " << clickedItem << endl;
                                continue;
                            }
                        }

                        if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                            if (systems.getBrewerSystem()->checkRefreshButtonClick(window, *currency.getGoldPtr())) {
                                continue;
                            }
                            if (systems.getBrewerSystem()->checkSkipButtonClick(window, *currency.getGoldPtr())) {
                                continue;
                            }
                            int potionIndex = systems.getBrewerSystem()->checkBuyButtonClick(window, *currency.getGoldPtr());
                            if (potionIndex != -1) {
                                Potion boughtPotion = systems.getBrewerSystem()->getPotionAtIndex(potionIndex);
                                if (systems.getPotionInventory()->addPotion(boughtPotion)) {
                                    systems.getBasicInventory()->addItem(boughtPotion.name);
                                }
                                else {
                                    currency.addGold(boughtPotion.cost);
                                }
                                continue;
                            }
                            // If Brewer is visible, consume the click and don't process anything else
                            continue;
                        }

                        if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                            int mountClicked = systems.getRiderScreen()->getMountButtonClicked(window);
                            if (mountClicked == 0) {
                                if (currency.getGold() >= 10) {
                                    currency.removeGold(10);

                                    MountData currentMount = stateManager.getCurrentMount();

                                    // Check if already has a Horse mount
                                    if (currentMount.type == MountType::Horse) {
                                        // Stack duration - add 7 days
                                        currentMount.remainingDays += 7;
                                        stateManager.setCurrentMount(currentMount);
                                        systems.getRiderScreen()->hide();
                                        cout << "Extended Horse mount duration! +7 days (Total: " << currentMount.remainingDays << " days remaining)" << endl;
                                    }
                                    else {
                                        // Different mount or no mount - replace completely
                                        MountData newMount;
                                        newMount.type = MountType::Horse;
                                        newMount.cost = 10;
                                        newMount.speedBonus = 0.5f;
                                        newMount.durationDays = 7;
                                        newMount.remainingDays = 7;
                                        stateManager.setCurrentMount(newMount);
                                        systems.getRiderScreen()->hide();
                                        cout << "Purchased Horse! -10 Gold" << endl;
                                    }
                                }
                                else {
                                    cout << "Not enough gold for Horse!" << endl;
                                }
                                continue;
                            }
                            else if (mountClicked == 1) {
                                if (currency.getGold() >= 20) {
                                    currency.removeGold(20);

                                    MountData currentMount = stateManager.getCurrentMount();

                                    // Check if already has a Griffin mount
                                    if (currentMount.type == MountType::Griffin) {
                                        // Stack duration - add 14 days
                                        currentMount.remainingDays += 14;
                                        stateManager.setCurrentMount(currentMount);
                                        systems.getRiderScreen()->hide();
                                        cout << "Extended Griffin mount duration! +14 days (Total: " << currentMount.remainingDays << " days remaining)" << endl;
                                    }
                                    else {
                                        // Different mount or no mount - replace completely
                                        MountData newMount;
                                        newMount.type = MountType::Griffin;
                                        newMount.cost = 20;
                                        newMount.speedBonus = 0.75f;
                                        newMount.durationDays = 14;
                                        newMount.remainingDays = 14;
                                        stateManager.setCurrentMount(newMount);
                                        systems.getRiderScreen()->hide();
                                        cout << "Purchased Griffin! -20 Gold" << endl;
                                    }
                                }
                                else {
                                    cout << "Not enough gold for Griffin!" << endl;
                                }
                                continue;
                            }
                            // If Rider is visible, consume the click and don't process anything else
                            continue;
                        }

                        if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                            int fightIndex = systems.getArenaScreen()->getFightButtonClicked(window);
                            if (fightIndex != -1) {
                                const ArenaPlayer& opponent = systems.getArenaScreen()->getSelectedPlayer(fightIndex);
                                systems.getArenaScreen()->hide();

                                if (systems.getCharacterSheet() && systems.getMenu()) {
                                    missionManager.createCombat(
                                        static_cast<float>(win.x), static_cast<float>(win.y),
                                        systems.getCharacterSheet(),
                                        opponent.imagePath,
                                        opponent.name,
                                        opponent.level,
                                        systems.getMenu(),
                                        currency.getSilver(),
                                        currency.getGold(),
                                        currency.getTorches(),
                                        1.0,
                                        true,
                                        stateManager.getPlayerRaceName()
                                    );

                                    missionManager.setInArenaCombat(true);
                                    stateManager.setState(GameState::Combat);
                                }
                                continue;
                            }
                        }
                        // Block clicks from passing through to NPCs/background when menu screens are visible
                        if ((systems.getShop() && systems.getShop()->isVisible()) ||
                            (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) ||
                            (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) ||
                            (systems.getRiderScreen() && systems.getRiderScreen()->isVisible())) {
                            // Menu screen is open, don't process anything below (NPCs, quest dialog)
                            continue;
                        }

                        if (systems.getQuestDialog()->isAcceptButtonClicked(window)) {
                            if (!systems.getFatigueSystem()->canStartMission()) {
                                systems.getFatiguedDialog()->show(systems.getFatigueSystem()->getTimeUntilReset());
                                systems.getQuestDialog()->hide();
                                continue;
                            }

                            systems.getQuestDialog()->hide();

                            if (!systems.getCharacterSheet()) {
                                cout << "CharacterSheet NULL - forcing creation..." << endl;

                                systems.createCharacterSheet(
                                    static_cast<float>(win.x),
                                    static_cast<float>(win.y),
                                    stateManager.getPlayerRaceIndex(),
                                    stateManager.getPlayerClassIndex(),
                                    stateManager.getPlayerFaction()
                                );
                                systems.getCharacterSheet()->setInventory(systems.getBasicInventory());

                                uiManager.initCharacterButton();
                                uiManager.initInventoryButton();
                                if (switchFont.openFromFile("arial.ttf")) {
                                    uiManager.initCharacterSwitchButton(switchFont);
                                }
                                uiManager.registerButtons();
                                currency.initUI();

                                if (systems.getPotionInventory()) {
                                    systems.getPotionInventory()->initUI(
                                        static_cast<float>(win.x) - 400.f,
                                        static_cast<float>(win.y) / 2.f + 100.f
                                    );
                                }
                            }

                            missionCharacterSheet = systems.getCharacterSheet();
                            missionMenu = systems.getMenu();

                            cout << "FINAL CHECK - POINTERS:" << endl;
                            cout << "  CharacterSheet: " << (missionCharacterSheet ? "EXISTS" : "NULL") << endl;
                            cout << "  Menu: " << (missionMenu ? "EXISTS" : "NULL") << endl;

                            if (!missionCharacterSheet) {
                                cout << "CRITICAL ERROR: Cannot start mission without CharacterSheet!" << endl;
                                systems.getQuestDialog()->show("Error", "Character not initialized. Please restart.");
                                continue;
                            }

                            int playerLevel = missionCharacterSheet->getLevel();
                            int monsterLevel = std::max(1, std::min(100, playerLevel));

                            const NPCMissionData& npc = npcManager.getSelectedNPC();
                            MonsterReward reward = MissionRewardDatabase::getReward(monsterLevel, npc.missionType);

                            missionManager.startMission(
                                npc,
                                reward.monsterName,
                                npc.monsterTexture,
                                monsterLevel,
                                stateManager.getPlayerRaceName(),
                                static_cast<float>(win.x), static_cast<float>(win.y),
                                missionMenu,
                                missionCharacterSheet,
                                currency.getSilverPtr(),
                                currency.getGoldPtr(),
                                currency.getTorchesPtr(),
                                &uiManager.getCharacterButton(),
                                uiManager.getCharacterText()
                            );

                            if (missionManager.getCurrentMission()) {
                                missionManager.getCurrentMission()->setMountSpeedBonus(stateManager.getMountSpeedBonus());
                            }

                            stateManager.setState(GameState::OnMission);

                            npcManager.generateNewQuestForNPC(npcManager.getSelectedNPCIndex(), playerLevel);
                            npcManager.clearSelection();

                            continue;
                        }
                        else {
                            int npcIndex = npcManager.checkNPCClick(window);
                            if (npcIndex != -1) {
                                npcManager.selectNPC(npcIndex);

                                int playerLevel = systems.getCharacterSheet() ? systems.getCharacterSheet()->getLevel() : 1;
                                string questWithRewards = npcManager.getQuestWithRewards(npcIndex, playerLevel);

                                systems.getQuestDialog()->show(
                                    npcManager.getSelectedNPC().npcName,
                                    questWithRewards
                                );
                                continue;
                            }
                        }
                    }

                    else if (stateManager.getCurrentState() == GameState::OnMission && missionManager.getMissionScreen()) {

                        if (systems.getMenu()) {
                            int clickedMenuButton = systems.getMenu()->getClickedItemIndex(window);
                            if (clickedMenuButton != -1) {
                                if (clickedMenuButton == 0) {
                                    // Dungeons button - close all screens
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                    }
                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }
                                }
                                else if (clickedMenuButton == 2) {
                                    // Shop button
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }

                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                        if (systems.getShop()) systems.getShop()->show();
                                    }
                                    else if (systems.getShop() && systems.getShop()->isVisible()) {
                                        systems.getShop()->hide();
                                    }
                                    else {
                                        if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                            systems.getArenaScreen()->hide();
                                        }
                                        if (systems.getShop()) systems.getShop()->show();
                                    }
                                }
                                else if (clickedMenuButton == 1) {
                                    // Arena button
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }

                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    else {
                                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                            systems.getCharacterSheet()->hide();
                                            uiManager.forceDeactivateAll();
                                        }
                                        if (systems.getArenaScreen()) systems.getArenaScreen()->show();
                                    }
                                }
                                else if (clickedMenuButton == 3) {
                                    // Brewer button
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                    }
                                    if (systems.getRiderScreen() && systems.getRiderScreen()->isVisible()) {
                                        systems.getRiderScreen()->hide();
                                    }

                                    if (systems.getBrewerSystem()) {
                                        if (systems.getBrewerSystem()->isVisible()) {
                                            systems.getBrewerSystem()->hide();
                                        }
                                        else {
                                            systems.getBrewerSystem()->show();
                                        }
                                    }
                                }
                                else if (clickedMenuButton == 4) {
                                    // Rider button
                                    if (systems.getShop() && systems.getShop()->isVisible()) systems.getShop()->hide();
                                    if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                                        systems.getArenaScreen()->hide();
                                    }
                                    if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                                        systems.getCharacterSheet()->hide();
                                        uiManager.forceDeactivateAll();
                                    }
                                    if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                                        systems.getBrewerSystem()->hide();
                                    }

                                    if (systems.getRiderScreen()) {
                                        if (systems.getRiderScreen()->isVisible()) {
                                            systems.getRiderScreen()->hide();
                                        }
                                        else {
                                            systems.getRiderScreen()->show();
                                        }
                                    }
                                }
                                // Buttons 5,6,7,8 (Fortress, Curse, Conquerors, Guild) not yet implemented
                                continue;
                            }
                        }

                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                            if (systems.getPotionInventory()) {
                                int potionIndex = systems.getPotionInventory()->checkUseButtonClick(window);
                                if (potionIndex != -1) {
                                    systems.getPotionInventory()->usePotion(potionIndex);
                                    continue;
                                }
                            }
                            continue;
                        }

                        if (systems.getShop() && systems.getShop()->isVisible()) {
                            if (systems.getShop()->checkRefreshClick(window, currency)) {
                                continue;
                            }
                            if (systems.getShop()->checkItemClick(window, currency, systems.getBasicInventory())) {
                                continue;
                            }
                        }

                        if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                            if (systems.getBrewerSystem()->checkRefreshButtonClick(window, *currency.getGoldPtr())) {
                                continue;
                            }
                            if (systems.getBrewerSystem()->checkSkipButtonClick(window, *currency.getGoldPtr())) {
                                continue;
                            }
                            int potionIndex = systems.getBrewerSystem()->checkBuyButtonClick(window, *currency.getGoldPtr());
                            if (potionIndex != -1) {
                                Potion boughtPotion = systems.getBrewerSystem()->getPotionAtIndex(potionIndex);
                                if (systems.getPotionInventory()->addPotion(boughtPotion)) {
                                    systems.getBasicInventory()->addItem(boughtPotion.name);
                                }
                                else {
                                    currency.addGold(boughtPotion.cost);
                                }
                                continue;
                            }
                        }

                        if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                            int fightIndex = systems.getArenaScreen()->getFightButtonClicked(window);
                            if (fightIndex != -1) {
                                const ArenaPlayer& opponent = systems.getArenaScreen()->getSelectedPlayer(fightIndex);
                                systems.getArenaScreen()->hide();

                                if (systems.getCharacterSheet() && systems.getMenu()) {
                                    missionManager.createCombat(
                                        static_cast<float>(win.x), static_cast<float>(win.y),
                                        systems.getCharacterSheet(),
                                        opponent.imagePath,
                                        opponent.name,
                                        opponent.level,
                                        systems.getMenu(),
                                        currency.getSilver(),
                                        currency.getGold(),
                                        currency.getTorches(),
                                        1.0,
                                        true,
                                        stateManager.getPlayerRaceName()
                                    );

                                    missionManager.setInArenaCombat(true);
                                    stateManager.setState(GameState::Combat);
                                }
                                continue;
                            }
                        }

                        if (missionManager.getMissionScreen()->isSkipButtonClicked(window)) {
                            missionManager.getCurrentMission()->skip();
                            continue;
                        }
                    }

                    else if (stateManager.getCurrentState() == GameState::DoorChoice && missionManager.getDoorSelection()) {
                        auto doorSelection = missionManager.getDoorSelection();

                        if (doorSelection->isConfirmationVisible()) {
                            continue;
                        }

                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                            continue;
                        }

                        int torchClicked = doorSelection->checkTorchButtonClick(window);
                        if (torchClicked != -1) {
                            if (doorSelection->hasEnoughTorches() && !doorSelection->isDoorRevealed(torchClicked)) {
                                doorSelection->useTorch(torchClicked);
                                currency.setTorches(doorSelection->getCurrentTorches());
                            }
                            continue;
                        }

                        int doorClicked = doorSelection->checkDoorClick(window);
                        if (doorClicked != -1) {
                            doorSelection->showConfirmation(doorClicked);
                            continue;
                        }
                    }

                    else if (stateManager.getCurrentState() == GameState::Combat && missionManager.getCombatScene()) {
                        auto combatScene = missionManager.getCombatScene();

                        if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                            if (systems.getPotionInventory()) {
                                int potionIndex = systems.getPotionInventory()->checkUseButtonClick(window);
                                if (potionIndex != -1) {
                                    systems.getPotionInventory()->usePotion(potionIndex);
                                    continue;
                                }
                            }
                            continue;
                        }

                        if (systems.getMenu()) {
                            int menuClick = systems.getMenu()->getClickedItemIndex(window);
                            if (menuClick != -1) {
                                continue;
                            }
                        }

                        if (!combatScene->isCombatOver()) {
                            int speedClicked = combatScene->checkSpeedButtonClick(window);
                            if (speedClicked > 0) {
                                combatScene->setSpeed(speedClicked);
                                cout << "Combat speed set to " << speedClicked << "x" << endl;
                                continue;
                            }
                        }

                        if (combatScene->isCombatOver()) {
                            if (combatScene->didPlayerWin()) {
                                if (!missionManager.isInArenaCombat() && missionManager.getCurrentMission()) {
                                    missionManager.getCurrentMission()->applyRacialBonuses();

                                    MissionRewards rewards = missionManager.getCurrentMission()->getRewards();
                                    currency.addSilver(rewards.silver);
                                    currency.addGold(rewards.gold);
                                    currency.addTorches(rewards.torches);

                                    if (systems.getCharacterSheet()) {
                                        systems.getCharacterSheet()->addXP(rewards.xp);
                                    }

                                    if (systems.getFatigueSystem()) {
                                        double speedReduction = 0.0;
                                        if (systems.getPotionInventory()) {
                                            speedReduction = systems.getPotionInventory()->getSpeedPotionFatigueReduction();
                                        }

                                        float mountBonus = stateManager.getMountSpeedBonus();
                                        if (mountBonus > 0.0f) {
                                            speedReduction += mountBonus;
                                        }

                                        systems.getFatigueSystem()->addMissionFatigue(speedReduction);
                                    }

                                    cout << "Mission complete! Silver +" << rewards.silver
                                        << ", Gold +" << rewards.gold << ", XP +" << rewards.xp << endl;

                                    missionManager.cleanupMission();
                                }

                                if (missionManager.isInArenaCombat()) {
                                    if (missionManager.getMissionScreen()) {
                                        stateManager.setState(GameState::OnMission);
                                    }
                                    else {
                                        stateManager.setState(GameState::MainGame);
                                    }
                                }
                                else {
                                    stateManager.setState(GameState::MainGame);
                                }
                            }
                            else {
                                stateManager.setState(GameState::MainGame);
                            }


                            // Check if curse selection is needed after combat
                            if (systems.getCharacterSheet() && systems.getCharacterSheet()->shouldTriggerCurseSelection()) {
                                stateManager.setState(GameState::CurseSelection);

                                if (curseScreen) {
                                    delete curseScreen;
                                }
                                curseScreen = new CurseSelection(static_cast<float>(win.x), static_cast<float>(win.y));

                                CurseSelection tempCurseGen(static_cast<float>(win.x), static_cast<float>(win.y));
                                CurseData newCurse = tempCurseGen.generateRandomCurse(
                                    systems.getCharacterSheet()->getLevel(),
                                    systems.getCharacterSheet()->getFaction()
                                );

                                CurseData currentCurse = systems.getCharacterSheet()->getCurrentCurse();
                                bool hasExisting = systems.getCharacterSheet()->hasActiveCurse();
                                bool isFirst = (systems.getCharacterSheet()->getLevel() == 1 && !systems.getCharacterSheet()->hasFirstCurseApplied());

                                curseScreen->setupCurseChoice(newCurse, currentCurse, hasExisting,
                                    systems.getCharacterSheet()->getLevel(), isFirst);
                            }
                            missionManager.cleanupCombat();
                            continue;
                        }
                    }
                }
            }
        }

        if (stateManager.getCurrentState() == GameState::FactionSelection) {
            factionScreen.update(deltaTime);
        }

        if (stateManager.getCurrentState() == GameState::MainGame) {
            systems.updateSystems();
        }

        if (stateManager.getCurrentState() == GameState::OnMission) {
            if (systems.getShop()) systems.getShop()->update();

            missionManager.updateMissionScreen();

            if (missionManager.isMissionComplete()) {
                cout << "CREATING DOOR SELECTION WITH:" << endl;
                cout << "  missionMenu: " << (missionMenu ? "EXISTS" : "NULL") << endl;
                cout << "  missionCharacterSheet: " << (missionCharacterSheet ? "EXISTS" : "NULL") << endl;

                missionManager.createDoorSelection(
                    static_cast<float>(win.x),
                    static_cast<float>(win.y),
                    missionMenu,
                    missionCharacterSheet,
                    currency.getSilver(),
                    currency.getGold(),
                    currency.getTorches()
                );
                stateManager.setState(GameState::DoorChoice);
            }
        }

        if (stateManager.getCurrentState() == GameState::DoorChoice) {
            missionManager.updateDoorSelection(deltaTime);

            if (missionManager.isDoorConfirmationFinished()) {
                auto doorSelection = missionManager.getDoorSelection();
                int doorIndex = doorSelection->getSelectedDoor();
                DoorPath path = doorSelection->getDoorPath(doorIndex);
                double damageModifier = 1.0;

                if (path == DoorPath::Ambushed) {
                    int silverLoss = static_cast<int>(currency.getSilver() * 0.05);
                    currency.removeSilver(silverLoss);
                    cout << "Ambushed! Lost " << silverLoss << " silver!" << endl;
                }
                else if (path == DoorPath::Ambush) {
                    damageModifier = 1.05;
                    cout << "Ambushed the monster! +5% damage!" << endl;
                }

                auto mission = missionManager.getCurrentMission();

                if (mission && doorSelection) {
                    CharacterSheet* charSheet = doorSelection->getCharacterSheet();
                    Menu* menuPtr = doorSelection->getMenu();

                    cout << "Mission: " << (mission ? "EXISTS" : "NULL") << endl;
                    cout << "DoorSelection: " << (doorSelection ? "EXISTS" : "NULL") << endl;
                    cout << "CharSheet: " << (charSheet ? "EXISTS" : "NULL") << endl;
                    cout << "Menu: " << (menuPtr ? "EXISTS" : "NULL") << endl;

                    if (charSheet && menuPtr) {
                        cout << "Creating combat scene..." << endl;
                        missionManager.createCombat(
                            static_cast<float>(win.x), static_cast<float>(win.y),
                            charSheet,
                            mission->getMonsterTexture(),
                            mission->getMonsterName(),
                            mission->getMonsterLevel(),
                            menuPtr,
                            currency.getSilver(),
                            currency.getGold(),
                            currency.getTorches(),
                            damageModifier,
                            false,
                            stateManager.getPlayerRaceName()
                        );

                        cout << "Combat scene created, switching to Combat state" << endl;
                        stateManager.setState(GameState::Combat);
                    }
                    else {
                        cout << "ERROR: CharSheet or Menu is NULL - returning to MainGame" << endl;
                        stateManager.setState(GameState::MainGame);
                    }
                }
                else {
                    cout << "ERROR: Mission or DoorSelection is NULL - returning to MainGame" << endl;
                    stateManager.setState(GameState::MainGame);
                }
                missionManager.cleanupDoorSelection();
            }
        }

        if (stateManager.getCurrentState() == GameState::Combat) {
            missionManager.updateCombat(deltaTime);
        }

        window.clear(Color::Black);

        if (switchScreen) {
            switchScreen->draw(window);
        }
        else if (stateManager.getCurrentState() == GameState::FactionSelection) {
            factionScreen.draw(window);
        }
        else if (stateManager.getCurrentState() == GameState::RaceSelection && raceScreen) {
            raceScreen->draw(window);
        }
        else if (stateManager.getCurrentState() == GameState::ClassSelection && classScreen) {
            classScreen->draw(window);
        }
        else if (stateManager.getCurrentState() == GameState::NameInput && nameInputScreen) {
            nameInputScreen->draw(window);
        }
        else if (stateManager.getCurrentState() == GameState::CurseSelection && curseScreen) {
            curseScreen->draw(window);
        }
        else if (stateManager.getCurrentState() == GameState::MainGame) {
            resources.drawBackground(window);
            npcManager.drawNPCs(window);
            if (systems.getMenu()) systems.getMenu()->draw(window);
            currency.draw(window);
            if (systems.getFatigueSystem()) systems.getFatigueSystem()->draw(window);

            // UI BUTTONS - ALWAYS DRAWN
            uiManager.drawCharacterButton(window);

            if (hasExistingCharacter) {
                uiManager.drawCharacterSwitchButton(window);
            }

            if (systems.getQuestDialog()) systems.getQuestDialog()->draw(window);
            if (systems.getArenaScreen()) systems.getArenaScreen()->draw(window);
            if (systems.getShop()) systems.getShop()->draw(window);
            if (systems.getBrewerSystem()) systems.getBrewerSystem()->draw(window);
            if (systems.getRiderScreen()) systems.getRiderScreen()->draw(window);
            if (systems.getFortressScreen()) systems.getFortressScreen()->draw(window);
            if (systems.getCharacterSheet()) systems.getCharacterSheet()->draw(window);
            if (systems.getPotionInventory() && systems.getCharacterSheet() &&
                systems.getCharacterSheet()->isVisible()) {
                systems.getPotionInventory()->draw(window);
            }
            if (systems.getFatiguedDialog()) systems.getFatiguedDialog()->draw(window);
            if (systems.getCurseDialog()) systems.getCurseDialog()->draw(window);
        }
        else if (stateManager.getCurrentState() == GameState::OnMission && missionManager.getMissionScreen()) {
            missionManager.getMissionScreen()->draw(window);

            // UI BUTTONS - ALWAYS DRAWN
            uiManager.drawCharacterButton(window);

            if (hasExistingCharacter) {
                uiManager.drawCharacterSwitchButton(window);
            }

            if (systems.getArenaScreen() && systems.getArenaScreen()->isVisible()) {
                systems.getArenaScreen()->draw(window);
            }
            if (systems.getShop() && systems.getShop()->isVisible()) {
                systems.getShop()->draw(window);
            }
            if (systems.getBrewerSystem() && systems.getBrewerSystem()->isVisible()) {
                systems.getBrewerSystem()->draw(window);
            }
            if (systems.getRiderScreen()) {
                systems.getRiderScreen()->draw(window);
            }
            if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                systems.getCharacterSheet()->draw(window);
                if (systems.getPotionInventory()) {
                    systems.getPotionInventory()->draw(window);
                }
            }
        }
        else if (stateManager.getCurrentState() == GameState::DoorChoice && missionManager.getDoorSelection()) {
            missionManager.getDoorSelection()->draw(window);

            // UI BUTTONS - ALWAYS DRAWN
            uiManager.drawCharacterButton(window);

            if (hasExistingCharacter) {
                uiManager.drawCharacterSwitchButton(window);
            }

            if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                systems.getCharacterSheet()->draw(window);
                if (systems.getPotionInventory()) {
                    systems.getPotionInventory()->draw(window);
                }
            }
        }
        else if (stateManager.getCurrentState() == GameState::Combat && missionManager.getCombatScene()) {
            missionManager.getCombatScene()->draw(window);

            // UI BUTTONS - ALWAYS DRAWN
            uiManager.drawCharacterButton(window);

            if (hasExistingCharacter) {
                uiManager.drawCharacterSwitchButton(window);
            }

            if (systems.getCharacterSheet() && systems.getCharacterSheet()->isVisible()) {
                systems.getCharacterSheet()->draw(window);
                if (systems.getPotionInventory()) {
                    systems.getPotionInventory()->draw(window);
                }
            }
        }

        window.display();
    }

    delete raceScreen;
    delete classScreen;
    delete nameInputScreen;
    delete switchScreen;
    delete curseScreen;

    return 0;
}