#pragma once
#include <SFML/Graphics.hpp>

// Forward declarations
class GameStateManager;
class CurrencyManager;
class UIManager;
class SystemsManager;
class NPCManager;
class MissionManager;
class CharacterManager;
class NameInputScreen;
class FactionSelection;

// Expanded InputManager - handles non-mouse-click input events
class InputManager {
private:
    GameStateManager* stateManager;
    CurrencyManager* currency;
    UIManager* uiManager;
    SystemsManager* systems;
    NPCManager* npcManager;
    MissionManager* missionManager;

    float windowWidth;
    float windowHeight;

public:
    InputManager(
        GameStateManager* stateMgr,
        CurrencyManager* currMgr,
        UIManager* uiMgr,
        SystemsManager* sysMgr,
        NPCManager* npcMgr,
        MissionManager* missionMgr,
        float winWidth,
        float winHeight)
        : stateManager(stateMgr), currency(currMgr), uiManager(uiMgr),
        systems(sysMgr), npcManager(npcMgr), missionManager(missionMgr),
        windowWidth(winWidth), windowHeight(winHeight) {
    }

    // ============================================================================
    // STEP 3 REFACTOR: Cleaner input handling
    // ============================================================================

    // Handle window close event (with save logic)
    void handleWindowClose(
        const sf::RenderWindow& window,
        CharacterManager* characterManager,
        bool hasExistingCharacter);

    // Handle text input events
    void handleTextInput(
        const sf::Event::TextEntered& textEvent,
        NameInputScreen* nameInputScreen);

    // Handle mouse moved events
    void handleMouseMoved(
        sf::RenderWindow& window,
        FactionSelection* factionScreen);

    // Handle keyboard events
    void handleKeyboardInput(
        const sf::Event::KeyPressed& keyEvent);

    // Handle menu button clicks (original functionality)
    // FIXED: Removed const qualifier to match what Menu::getClickedItemIndex expects
    void handleMouseClick(sf::RenderWindow& window);

    // Getters
    GameStateManager* getStateManager() { return stateManager; }
    CurrencyManager* getCurrency() { return currency; }
    UIManager* getUIManager() { return uiManager; }
    SystemsManager* getSystems() { return systems; }
    NPCManager* getNPCManager() { return npcManager; }
    MissionManager* getMissionManager() { return missionManager; }

    float getWindowWidth() const { return windowWidth; }
    float getWindowHeight() const { return windowHeight; }
};