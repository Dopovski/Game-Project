#define _CRT_SECURE_NO_WARNINGS
#include "InputManager.h"
#include "GameStateManager.h"
#include "CurrencyManager.h"
#include "UIManager.h"
#include "SystemsManager.h"
#include "NPCManager.h"
#include "MissionManager.h"
#include "CharacterManager.h"

// System components
#include "menu.h"
#include "Shop.h"
#include "ArenaScreen.h"
#include "BrewerSystem.h"
#include "QuestDialog.h"
#include "CharacterSheet.h"
#include "Rider.h"
#include "NameInput.h"
#include "Faction.h"

#include <iostream>

using namespace sf;
using namespace std;

// ============================================================================
// STEP 3 REFACTOR: Input handling (partial - non-mouse-click events)
// ============================================================================

void InputManager::handleWindowClose(
    const RenderWindow& window,
    CharacterManager* characterManager,
    bool hasExistingCharacter)
{
    // Save game state for all active gameplay states
    if (hasExistingCharacter &&
        (stateManager->getCurrentState() == GameState::MainGame ||
            stateManager->getCurrentState() == GameState::OnMission ||
            stateManager->getCurrentState() == GameState::DoorChoice ||
            stateManager->getCurrentState() == GameState::Combat)) {

        characterManager->saveCurrentCharacterFromSystems(stateManager, currency, systems);
    }
}

void InputManager::handleTextInput(
    const Event::TextEntered& textEvent,
    NameInputScreen* nameInputScreen)
{
    if (stateManager->getCurrentState() == GameState::NameInput && nameInputScreen) {
        nameInputScreen->handleTextInput(textEvent);
    }
}

void InputManager::handleMouseMoved(RenderWindow& window, FactionSelection* factionScreen)
{
    if (stateManager->getCurrentState() == GameState::FactionSelection && factionScreen) {
        factionScreen->checkHover(window);
    }
    else if (stateManager->getCurrentState() == GameState::MainGame ||
        stateManager->getCurrentState() == GameState::OnMission ||
        stateManager->getCurrentState() == GameState::DoorChoice ||
        stateManager->getCurrentState() == GameState::Combat) {

        if (systems->getFatigueSystem()) {
            systems->getFatigueSystem()->checkHover(window);
        }
        if (systems->getShop() && systems->getShop()->isVisible()) {
            systems->getShop()->checkHover(window);
        }
        if (systems->getCharacterSheet() && systems->getCharacterSheet()->isVisible()) {
            systems->getCharacterSheet()->checkEquipmentHover(window);
        }
        if (systems->getBrewerSystem() && systems->getBrewerSystem()->isVisible()) {
            systems->getBrewerSystem()->checkMousePosition(window);
        }
        if (systems->getMenu()) {
            systems->getMenu()->checkMousePosition(window);
        }
    }
}

void InputManager::handleKeyboardInput(const Event::KeyPressed& keyEvent)
{
    // Keyboard handling can be expanded here in future
    // Currently most keyboard input is handled by specific screens
}

// Original menu button click handler
// FIXED: Changed parameter from const RenderWindow& to RenderWindow& to match Menu::getClickedItemIndex
void InputManager::handleMouseClick(RenderWindow& window) {
    Menu* menu = systems->getMenu();
    if (!menu) return;

    // Get which menu button was clicked
    int clickedIndex = menu->getClickedItemIndex(window);

    if (clickedIndex == -1) {
        return;
    }

    // Handle menu button clicks
    switch (clickedIndex) {
    case 0: // Dungeons
    {
        std::cout << "Dungeons clicked - Opening quest dialog" << std::endl;
        QuestDialog* questDialog = systems->getQuestDialog();
        if (questDialog) {
            questDialog->show("Quest Master", "Choose your adventure!");
        }
        break;
    }

    case 1: // Arena
    {
        std::cout << "Arena clicked" << std::endl;
        ArenaScreen* arena = systems->getArenaScreen();
        if (arena) {
            arena->show();
        }
        break;
    }

    case 2: // Shop
    {
        std::cout << "Shop clicked" << std::endl;
        Shop* shop = systems->getShop();
        if (shop) {
            shop->show();
        }
        break;
    }

    case 3: // Brewer
    {
        std::cout << "Brewer clicked" << std::endl;
        BrewerSystem* brewer = systems->getBrewerSystem();
        if (brewer) {
            brewer->show();
        }
        break;
    }

    case 4: // Rider
    {
        std::cout << "Rider clicked" << std::endl;
        RiderScreen* rider = systems->getRiderScreen();
        if (rider) {
            rider->show();
        }
        break;
    }

    case 5: // Fortress
    {
        std::cout << "Fortress clicked - Not yet implemented" << std::endl;
        break;
    }

    case 6: // Curse
    {
        std::cout << "Curse clicked - Not yet implemented" << std::endl;
        break;
    }

    case 7: // Conquerors
    {
        std::cout << "Conquerors clicked - Not yet implemented" << std::endl;
        break;
    }

    case 8: // Guild
    {
        std::cout << "Guild clicked - Not yet implemented" << std::endl;
        break;
    }

    default:
        std::cout << "Unknown menu item clicked: " << clickedIndex << std::endl;
        break;
    }
}