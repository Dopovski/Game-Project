#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "CharacterSheet.h"
#include "menu.h"
#include "EquipmentEffectsSystem.h"
#include "MageEquipmentEffectsSystem.h"
#include "BruiserEquipmentEffectsSystem.h"
#include "FloatingText.h"

using namespace std;
using namespace sf;

struct MonsterStats {
    double health;
    double maxHealth;
    double attack;
    double intelligence;
    double dexterity;
    double armor;
    int level;
};

class CombatScene {
public:
    CombatScene(float windowWidth, float windowHeight,
        CharacterSheet* playerSheet,
        const string& monsterTexture,
        const string& monsterName,
        int monsterLevel,
        Menu* menu,
        int silver, int gold, int torches,
        double damageModifier = 1.0,
        bool isArenaCombat = false,
        const string& playerRace = "",
        const string& playerName = "Player");
    ~CombatScene();

    void draw(RenderWindow& window);
    void update(float deltaTime);
    bool isCombatOver() const { return combatEnded; }
    bool didPlayerWin() const { return playerWon; }
    bool isContinueClicked(const RenderWindow& window) const;

    // Speed control
    int checkSpeedButtonClick(const RenderWindow& window) const;
    void setSpeed(int speed);

    Menu* getMenu() { return menu; }
    CharacterSheet* getCharacterSheet() { return playerSheet; }
    CircleShape& getCharacterButton() { return characterIconButton; }

    // Set player race for racial abilities
    void setPlayerRace(const string& race);

private:
    Font font;
    float windowWidth;
    float windowHeight;

    Menu* menu;

    CharacterSheet* playerSheet;
    Texture playerTexture;
    Sprite* playerSprite;
    double playerCurrentHealth;
    Vector2f playerBasePosition;

    Texture monsterTexture;
    Sprite* monsterSprite;
    MonsterStats monsterStats;
    string monsterName;
    string playerName;
    Vector2f monsterBasePosition;

    Text playerNameText{ font };
    Text playerHealthText{ font };
    Text playerStatsText{ font };

    Text monsterNameText{ font };
    Text monsterHealthText{ font };
    Text monsterStatsText{ font };

    RectangleShape playerHealthBar;
    RectangleShape playerHealthBarBg;
    RectangleShape playerShieldBar;  // Grey bar for shield
    RectangleShape monsterHealthBar;
    RectangleShape monsterHealthBarBg;

    CircleShape characterIconButton;
    Text characterIconText{ font };

    Text silverDisplayText{ font };
    Text goldDisplayText{ font };
    Text torchesDisplayText{ font };


    // Player stat box
    RectangleShape playerStatBox;
    Text playerStatBoxTitle{ font };
    Text playerAttackText{ font };
    Text playerDefenseText{ font };
    Text playerDexterityText{ font };
    Text playerIntelligenceText{ font };
    Text playerLuckText{ font };
    Text playerLevelText{ font };

    // Monster stat box
    RectangleShape monsterStatBox;
    Text monsterStatBoxTitle{ font };
    Text monsterAttackTextBox{ font };
    Text monsterDefenseTextBox{ font };
    Text monsterDexterityTextBox{ font };
    Text monsterIntelligenceTextBox{ font };
    Text monsterLevelTextBox{ font };
    bool combatEnded;
    bool playerWon;

    // Door path modifier
    double playerDamageModifier;

    // Speed control
    float combatSpeed;
    RectangleShape speed1xButton;
    RectangleShape speed2xButton;
    RectangleShape speed3xButton;
    Text speed1xText{ font };
    Text speed2xText{ font };
    Text speed3xText{ font };

    // Autobattle system
    enum class BattleState {
        Waiting,
        PlayerAttacking,
        PlayerReturning,
        MonsterAttacking,
        MonsterReturning
    };

    BattleState battleState;
    float attackTimer;
    float attackSpeed;
    float returnSpeed;
    bool playerTurn;

    // Class abilities
    bool bruiserLastStandUsed;
    bool bruiserLastStandActive;
    int bruiserLastStandAttackCount;
    bool mageFirstAttackUsed;

    // Last Stand (Bruiser-only ability - works in both arena AND missions)
    bool lastStandActive;
    int lastStandAttacksRemaining;
    bool lastStandTriggered; // Flag to trigger counter attacks

    // Track if this is arena combat (for other purposes)
    bool isArenaFight;

    // Race tracking for racial abilities
    string playerRace;
    bool isFirstCombatAttack; // For Dwarf Arcane Shield

    // Equipment effects system
    EquipmentEffectsSystem* equipmentEffects;
    MageEquipmentEffectsSystem* mageEquipmentEffects;
    BruiserEquipmentEffectsSystem* bruiserEquipmentEffects;
    vector<FloatingText> floatingTexts;

    // Victory/Defeat screen
    bool showEndScreen;
    RectangleShape endScreenBackground;
    Text endScreenTitle{ font };
    Text endScreenMessage{ font };
    RectangleShape continueButton;
    Text continueButtonText{ font };

    void generateMonsterStats(int level);
    void setupUI();
    void updateBattle(float deltaTime);
    void calculateDamage(bool isPlayerAttacking);
    void updateHealthBars();
    double calculateArmorReduction(double armor, int level);
    void setupEndScreen();
    bool isContinueButtonClicked(const RenderWindow& window) const;
};