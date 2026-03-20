#include "CombatScene.h"
#include <iostream>
#include <cmath>

CombatScene::CombatScene(float winWidth, float winHeight,
    CharacterSheet* playerSheet,
    const string& monsterTexturePath,
    const string& monsterName,
    int monsterLevel,
    Menu* menu,
    int silver, int gold, int torches,
    double damageModifier,
    bool isArenaCombat,
    const string& playerRace,
    const string& playerName)
    : windowWidth(winWidth), windowHeight(winHeight),
    playerName(playerName),
    playerSheet(playerSheet), monsterName(monsterName),
    combatEnded(false), playerWon(false),
    playerSprite(nullptr), monsterSprite(nullptr),
    menu(menu),
    playerDamageModifier(damageModifier),
    combatSpeed(1.0f),
    isArenaFight(isArenaCombat),
    lastStandActive(false),
    lastStandAttacksRemaining(0),
    lastStandTriggered(false),
    battleState(BattleState::Waiting),
    attackTimer(0.2f),        // 1.0f -> 0.2f (5x faster)
    attackSpeed(2500.0f),     // 500.0f -> 2500.0f (5x faster)
    returnSpeed(2000.0f),     // 400.0f -> 2000.0f (5x faster)
    playerTurn(true),
    bruiserLastStandUsed(false),
    bruiserLastStandActive(false),
    bruiserLastStandAttackCount(0),
    mageFirstAttackUsed(false),
    showEndScreen(false),
    isFirstCombatAttack(true),
    playerRace(playerRace),
    equipmentEffects(nullptr),
    mageEquipmentEffects(nullptr),
    bruiserEquipmentEffects(nullptr) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading combat font!" << endl;
    }

    // Load player sprite
    if (!playerTexture.loadFromFile("Assets/NPC_Veron1.png")) {
        cerr << "Error loading player combat texture!" << endl;
    }
    playerTexture.setSmooth(true);
    playerSprite = new Sprite(playerTexture);

    float playerScale = 400.f / playerTexture.getSize().y;
    playerSprite->setScale({ playerScale, playerScale });
    playerBasePosition = { 450.f, windowHeight / 2.f - 200.f };
    playerSprite->setPosition(playerBasePosition);

    // Load monster sprite
    if (!this->monsterTexture.loadFromFile(monsterTexturePath)) {
        cerr << "Error loading monster combat texture!" << endl;
    }
    this->monsterTexture.setSmooth(true);
    monsterSprite = new Sprite(this->monsterTexture);

    float monsterScale = 400.f / this->monsterTexture.getSize().y;
    monsterSprite->setScale({ monsterScale, monsterScale });
    monsterBasePosition = { windowWidth - 850.f, windowHeight / 2.f - 200.f };
    monsterSprite->setPosition(monsterBasePosition);

    playerCurrentHealth = playerSheet->getStat("Health");
    generateMonsterStats(monsterLevel);

    // Announce racial passive at combat start
    if (!playerRace.empty()) {
        cout << "\n===== COMBAT START =====" << endl;
        cout << "Player: " << playerRace << " (Level " << playerSheet->getLevel() << ")" << endl;

        if (playerRace == "Human") {
            cout << "Racial Passive: Silver Tongue (+5% silver gain)" << endl;
        }
        else if (playerRace == "Dwarf") {
            cout << "Racial Passive: Arcane Shield (50% chance to reduce first attack damage by 50%)" << endl;
        }
        else if (playerRace == "Golem") {
            int armorBonus = (playerSheet->getClassIndex() == 1) ? 10 : 5;
            cout << "Racial Passive: Stone Skin (+" << armorBonus << "% armor - permanent bonus)" << endl;
        }
        else if (playerRace == "Goblin") {
            cout << "Racial Passive: Slippery (dodge cap increased to 55% - permanent bonus)" << endl;
        }
        else if (playerRace == "Elf") {
            cout << "Racial Passive: Arcane Affinity (+10% intelligence - permanent bonus)" << endl;
        }
        else if (playerRace == "Orc") {
            cout << "Racial Passive: Blood Fury (+1% damage per 10% missing health, max +10%)" << endl;
        }
        cout << "========================\n" << endl;
    }

    // Setup character icon button
    characterIconButton.setRadius(40.f);
    characterIconButton.setFillColor(Color(100, 100, 200));
    characterIconButton.setOutlineThickness(3.f);
    characterIconButton.setOutlineColor(Color::White);
    characterIconButton.setPosition({ 120.f, windowHeight - 100.f });

    characterIconText.setString("C");
    characterIconText.setCharacterSize(35);
    characterIconText.setFillColor(Color::White);
    FloatRect iconTextBounds = characterIconText.getLocalBounds();
    characterIconText.setOrigin({ iconTextBounds.size.x / 2.f, iconTextBounds.size.y / 2.f });
    characterIconText.setPosition({ 160.f, windowHeight - 65.f });

    // Setup currency display
    silverDisplayText.setString("Silver: " + to_string(silver));
    silverDisplayText.setCharacterSize(24);
    silverDisplayText.setFillColor(Color(192, 192, 192));
    silverDisplayText.setPosition({ 20.f, 20.f });

    goldDisplayText.setString("Gold: " + to_string(gold));
    goldDisplayText.setCharacterSize(24);
    goldDisplayText.setFillColor(Color(255, 215, 0));
    goldDisplayText.setPosition({ 180.f, 20.f });

    torchesDisplayText.setString("Torches: " + to_string(torches));
    torchesDisplayText.setCharacterSize(24);
    torchesDisplayText.setFillColor(Color(255, 140, 0));
    torchesDisplayText.setPosition({ 20.f, 50.f });

    // Setup speed buttons (top right corner)
    float buttonWidth = 80.f;
    float buttonHeight = 40.f;
    float buttonY = 100.f;
    float buttonSpacing = 10.f;
    float startX = windowWidth - (buttonWidth * 3 + buttonSpacing * 2 + 20.f);

    // 1x button
    speed1xButton.setSize({ buttonWidth, buttonHeight });
    speed1xButton.setFillColor(Color(100, 200, 100)); // Green - active by default
    speed1xButton.setOutlineThickness(3.f);
    speed1xButton.setOutlineColor(Color::White);
    speed1xButton.setPosition({ startX, buttonY });

    speed1xText.setString("1x");
    speed1xText.setCharacterSize(28);
    speed1xText.setFillColor(Color::White);
    FloatRect text1xBounds = speed1xText.getLocalBounds();
    speed1xText.setOrigin({ text1xBounds.size.x / 2.f, text1xBounds.size.y / 2.f });
    speed1xText.setPosition({ startX + buttonWidth / 2.f, buttonY + buttonHeight / 2.f });

    // 2x button
    speed2xButton.setSize({ buttonWidth, buttonHeight });
    speed2xButton.setFillColor(Color(100, 100, 100)); // Gray - inactive
    speed2xButton.setOutlineThickness(3.f);
    speed2xButton.setOutlineColor(Color::White);
    speed2xButton.setPosition({ startX + buttonWidth + buttonSpacing, buttonY });

    speed2xText.setString("2x");
    speed2xText.setCharacterSize(28);
    speed2xText.setFillColor(Color::White);
    FloatRect text2xBounds = speed2xText.getLocalBounds();
    speed2xText.setOrigin({ text2xBounds.size.x / 2.f, text2xBounds.size.y / 2.f });
    speed2xText.setPosition({ startX + buttonWidth + buttonSpacing + buttonWidth / 2.f, buttonY + buttonHeight / 2.f });

    // 3x button
    speed3xButton.setSize({ buttonWidth, buttonHeight });
    speed3xButton.setFillColor(Color(100, 100, 100)); // Gray - inactive
    speed3xButton.setOutlineThickness(3.f);
    speed3xButton.setOutlineColor(Color::White);
    speed3xButton.setPosition({ startX + (buttonWidth + buttonSpacing) * 2, buttonY });

    speed3xText.setString("3x");
    speed3xText.setCharacterSize(28);
    speed3xText.setFillColor(Color::White);
    FloatRect text3xBounds = speed3xText.getLocalBounds();
    speed3xText.setOrigin({ text3xBounds.size.x / 2.f, text3xBounds.size.y / 2.f });
    speed3xText.setPosition({ startX + (buttonWidth + buttonSpacing) * 2 + buttonWidth / 2.f, buttonY + buttonHeight / 2.f });

    // Initialize equipment effects system based on class
    int playerClassIndex = playerSheet->getClassIndex();

    if (playerClassIndex == 2) {
        // Mage - use MageEquipmentEffectsSystem
        mageEquipmentEffects = new MageEquipmentEffectsSystem();
        mageEquipmentEffects->initialize(playerSheet);

        // Setup floating text callback for Mage
        mageEquipmentEffects->setFloatingTextCallback(
            [this](const string& text, Vector2f pos, Color color, bool aroundPlayer) {
                floatingTexts.push_back(FloatingText(text, pos, color, aroundPlayer));
            }
        );

        // Set character positions for floating text
        mageEquipmentEffects->setCharacterPositions(playerBasePosition, monsterBasePosition);

        // Trigger combat start effects for Mage
        double playerShield = 0.0;
        mageEquipmentEffects->onCombatStart(playerCurrentHealth, playerShield, monsterStats.health, isArenaFight);
    }
    else if (playerClassIndex == 1) {
        // Bruiser - use BruiserEquipmentEffectsSystem
        bruiserEquipmentEffects = new BruiserEquipmentEffectsSystem();
        bruiserEquipmentEffects->initialize(playerSheet);

        // Setup floating text callback for Bruiser
        bruiserEquipmentEffects->setFloatingTextCallback(
            [this](const string& text, Vector2f pos, Color color, bool aroundPlayer) {
                floatingTexts.push_back(FloatingText(text, pos, color, aroundPlayer));
            }
        );

        // Set character positions for floating text
        bruiserEquipmentEffects->setCharacterPositions(playerBasePosition, monsterBasePosition);

        // Trigger combat start effects for Bruiser
        double playerShield = 0.0;
        bruiserEquipmentEffects->onCombatStart(playerCurrentHealth, playerShield, monsterStats.attack, isArenaFight);
    }
    else {
        // Assassin - use EquipmentEffectsSystem
        equipmentEffects = new EquipmentEffectsSystem();
        equipmentEffects->initialize(playerSheet);

        // Setup floating text callback
        equipmentEffects->setFloatingTextCallback(
            [this](const string& text, Vector2f pos, Color color, bool aroundPlayer) {
                floatingTexts.push_back(FloatingText(text, pos, color, aroundPlayer));
            }
        );

        // Set character positions for floating text
        equipmentEffects->setCharacterPositions(playerBasePosition, monsterBasePosition);

        // Trigger combat start effects
        double playerShield = 0.0;
        equipmentEffects->onCombatStart(playerCurrentHealth, playerShield);
    }

    setupUI();
}

CombatScene::~CombatScene() {
    delete playerSprite;
    delete monsterSprite;
    delete equipmentEffects;
    delete mageEquipmentEffects;
    delete bruiserEquipmentEffects;
}

void CombatScene::generateMonsterStats(int level) {
    monsterStats.level = level;

    // Base stats scale with level
    double baseHealth = 50.0 + (level * 15.0);
    double baseAttack = 8.0 + (level * 2.5);
    double baseArmor = 5.0 + (level * 1.5);

    // Add some randomness (±10%)
    double healthVariance = (rand() % 21 - 10) / 100.0; // -10% to +10%
    double attackVariance = (rand() % 21 - 10) / 100.0;
    double armorVariance = (rand() % 21 - 10) / 100.0;

    monsterStats.maxHealth = baseHealth * (1.0 + healthVariance);
    monsterStats.health = monsterStats.maxHealth;
    monsterStats.attack = baseAttack * (1.0 + attackVariance);
    monsterStats.armor = baseArmor * (1.0 + armorVariance);
    monsterStats.intelligence = 10.0;
    monsterStats.dexterity = 10.0;

    cout << "Monster Level " << level << " generated:" << endl;
    cout << "  Health: " << monsterStats.health << endl;
    cout << "  Attack: " << monsterStats.attack << endl;
    cout << "  Armor: " << monsterStats.armor << endl;
}

void CombatScene::setupUI() {
    // Player name and stats (above player)
    playerNameText.setString(playerName);
    playerNameText.setCharacterSize(28);
    playerNameText.setFillColor(Color::White);

    // Position above player sprite
    FloatRect playerBounds = playerSprite->getGlobalBounds();
    FloatRect playerNameBounds = playerNameText.getLocalBounds();
    playerNameText.setOrigin({ playerNameBounds.size.x / 2.f, playerNameBounds.size.y / 2.f });
    playerNameText.setPosition({
        playerBasePosition.x + playerBounds.size.x / 2.f,
        playerBasePosition.y - 120.f
        });

    playerHealthText.setString("HP: " + to_string(static_cast<int>(playerCurrentHealth)) +
        "/" + to_string(static_cast<int>(playerSheet->getStat("Health"))));
    playerHealthText.setCharacterSize(20);
    playerHealthText.setFillColor(Color::White);
    FloatRect playerHealthBounds = playerHealthText.getLocalBounds();
    playerHealthText.setOrigin({ playerHealthBounds.size.x / 2.f, playerHealthBounds.size.y / 2.f });
    playerHealthText.setPosition({
        playerBasePosition.x + playerBounds.size.x / 2.f,
        playerBasePosition.y - 90.f
        });

    playerStatsText.setString("ATK: " + to_string(static_cast<int>(playerSheet->getStat("Attack"))) +
        " | DEF: " + to_string(static_cast<int>(playerSheet->getStat("Armor"))));
    playerStatsText.setCharacterSize(18);
    playerStatsText.setFillColor(Color(200, 200, 200));
    FloatRect playerStatsBounds = playerStatsText.getLocalBounds();
    playerStatsText.setOrigin({ playerStatsBounds.size.x / 2.f, playerStatsBounds.size.y / 2.f });
    playerStatsText.setPosition({
        playerBasePosition.x + playerBounds.size.x / 2.f,
        playerBasePosition.y - 65.f
        });

    // Monster name and stats (above monster)
    monsterNameText.setString(monsterName);
    monsterNameText.setCharacterSize(28);
    monsterNameText.setFillColor(Color::White);

    FloatRect monsterBounds = monsterSprite->getGlobalBounds();
    FloatRect monsterNameBounds = monsterNameText.getLocalBounds();
    monsterNameText.setOrigin({ monsterNameBounds.size.x / 2.f, monsterNameBounds.size.y / 2.f });
    monsterNameText.setPosition({
        monsterBasePosition.x + monsterBounds.size.x / 2.f,
        monsterBasePosition.y - 120.f
        });

    monsterHealthText.setString("HP: " + to_string(static_cast<int>(monsterStats.health)) +
        "/" + to_string(static_cast<int>(monsterStats.maxHealth)));
    monsterHealthText.setCharacterSize(20);
    monsterHealthText.setFillColor(Color::White);
    FloatRect monsterHealthBounds = monsterHealthText.getLocalBounds();
    monsterHealthText.setOrigin({ monsterHealthBounds.size.x / 2.f, monsterHealthBounds.size.y / 2.f });
    monsterHealthText.setPosition({
        monsterBasePosition.x + monsterBounds.size.x / 2.f,
        monsterBasePosition.y - 90.f
        });

    monsterStatsText.setString("ATK: " + to_string(static_cast<int>(monsterStats.attack)) +
        " | DEF: " + to_string(static_cast<int>(monsterStats.armor)));
    monsterStatsText.setCharacterSize(18);
    monsterStatsText.setFillColor(Color(200, 200, 200));
    FloatRect monsterStatsBounds = monsterStatsText.getLocalBounds();
    monsterStatsText.setOrigin({ monsterStatsBounds.size.x / 2.f, monsterStatsBounds.size.y / 2.f });
    monsterStatsText.setPosition({
        monsterBasePosition.x + monsterBounds.size.x / 2.f,
        monsterBasePosition.y - 65.f
        });

    // Health bars above characters
    // Player health bar
    playerHealthBarBg.setSize({ 200.f, 20.f });
    playerHealthBarBg.setFillColor(Color(50, 50, 50));
    playerHealthBarBg.setOutlineThickness(2.f);
    playerHealthBarBg.setOutlineColor(Color::White);
    playerHealthBarBg.setPosition({
        playerBasePosition.x + playerBounds.size.x / 2.f - 100.f,
        playerBasePosition.y - 40.f
        });

    playerHealthBar.setSize({ 200.f, 20.f });
    playerHealthBar.setFillColor(Color::Green);
    playerHealthBar.setPosition({
        playerBasePosition.x + playerBounds.size.x / 2.f - 100.f,
        playerBasePosition.y - 40.f
        });

    // Player shield bar (grey, drawn on top of health)
    playerShieldBar.setSize({ 0.f, 20.f });  // Start with 0 width
    playerShieldBar.setFillColor(Color(150, 150, 150, 200));  // Grey with transparency
    playerShieldBar.setPosition({
        playerBasePosition.x + playerBounds.size.x / 2.f - 100.f,
        playerBasePosition.y - 40.f
        });

    // Monster health bar
    monsterHealthBarBg.setSize({ 200.f, 20.f });
    monsterHealthBarBg.setFillColor(Color(50, 50, 50));
    monsterHealthBarBg.setOutlineThickness(2.f);
    monsterHealthBarBg.setOutlineColor(Color::White);
    monsterHealthBarBg.setPosition({
        monsterBasePosition.x + monsterBounds.size.x / 2.f - 100.f,
        monsterBasePosition.y - 40.f
        });

    monsterHealthBar.setSize({ 200.f, 20.f });
    monsterHealthBar.setFillColor(Color::Red);
    monsterHealthBar.setPosition({
        monsterBasePosition.x + monsterBounds.size.x / 2.f - 100.f,
        monsterBasePosition.y - 40.f
        });

    updateHealthBars();

    // ===== PLAYER STAT BOX =====
    float playerStatBoxX = playerBasePosition.x + playerBounds.size.x / 2.f - 100.f;
    float playerStatBoxY = playerBasePosition.y + playerBounds.size.y + 20.f;

    playerStatBox.setSize({ 200.f, 160.f });
    playerStatBox.setFillColor(Color(30, 30, 40, 220));
    playerStatBox.setOutlineThickness(2.f);
    playerStatBox.setOutlineColor(Color(100, 150, 255));
    playerStatBox.setPosition({ playerStatBoxX, playerStatBoxY });

    playerStatBoxTitle.setString("PLAYER STATS");
    playerStatBoxTitle.setCharacterSize(16);
    playerStatBoxTitle.setFillColor(Color(100, 150, 255));
    playerStatBoxTitle.setStyle(Text::Bold);
    FloatRect titleBounds = playerStatBoxTitle.getLocalBounds();
    playerStatBoxTitle.setOrigin({ titleBounds.size.x / 2.f, 0.f });
    playerStatBoxTitle.setPosition({ playerStatBoxX + 100.f, playerStatBoxY + 5.f });

    playerLevelText.setString("Level: " + to_string(playerSheet->getLevel()));
    playerLevelText.setCharacterSize(14);
    playerLevelText.setFillColor(Color::White);
    playerLevelText.setPosition({ playerStatBoxX + 10.f, playerStatBoxY + 25.f });

    playerAttackText.setString("Attack: " + to_string(static_cast<int>(playerSheet->getStat("Attack"))));
    playerAttackText.setCharacterSize(14);
    playerAttackText.setFillColor(Color(255, 100, 100));
    playerAttackText.setPosition({ playerStatBoxX + 10.f, playerStatBoxY + 45.f });

    playerDefenseText.setString("Armor: " + to_string(static_cast<int>(playerSheet->getStat("Armor"))));
    playerDefenseText.setCharacterSize(14);
    playerDefenseText.setFillColor(Color(150, 150, 200));
    playerDefenseText.setPosition({ playerStatBoxX + 10.f, playerStatBoxY + 65.f });

    playerDexterityText.setString("Dexterity: " + to_string(static_cast<int>(playerSheet->getStat("Dexterity"))));
    playerDexterityText.setCharacterSize(14);
    playerDexterityText.setFillColor(Color(200, 100, 255));
    playerDexterityText.setPosition({ playerStatBoxX + 10.f, playerStatBoxY + 85.f });

    playerIntelligenceText.setString("Intelligence: " + to_string(static_cast<int>(playerSheet->getStat("Intelligence"))));
    playerIntelligenceText.setCharacterSize(14);
    playerIntelligenceText.setFillColor(Color(100, 200, 255));
    playerIntelligenceText.setPosition({ playerStatBoxX + 10.f, playerStatBoxY + 105.f });

    playerLuckText.setString("Luck: " + to_string(static_cast<int>(playerSheet->getStat("Luck"))));
    playerLuckText.setCharacterSize(14);
    playerLuckText.setFillColor(Color(255, 215, 0));
    playerLuckText.setPosition({ playerStatBoxX + 10.f, playerStatBoxY + 125.f });

    // ===== MONSTER STAT BOX =====
    float monsterStatBoxX = monsterBasePosition.x + monsterBounds.size.x / 2.f - 100.f;
    float monsterStatBoxY = monsterBasePosition.y + monsterBounds.size.y + 20.f;

    monsterStatBox.setSize({ 200.f, 140.f });
    monsterStatBox.setFillColor(Color(40, 30, 30, 220));
    monsterStatBox.setOutlineThickness(2.f);
    monsterStatBox.setOutlineColor(Color(255, 100, 100));
    monsterStatBox.setPosition({ monsterStatBoxX, monsterStatBoxY });

    monsterStatBoxTitle.setString("ENEMY STATS");
    monsterStatBoxTitle.setCharacterSize(16);
    monsterStatBoxTitle.setFillColor(Color(255, 100, 100));
    monsterStatBoxTitle.setStyle(Text::Bold);
    FloatRect monsterTitleBounds = monsterStatBoxTitle.getLocalBounds();
    monsterStatBoxTitle.setOrigin({ monsterTitleBounds.size.x / 2.f, 0.f });
    monsterStatBoxTitle.setPosition({ monsterStatBoxX + 100.f, monsterStatBoxY + 5.f });

    monsterLevelTextBox.setString("Level: " + to_string(monsterStats.level));
    monsterLevelTextBox.setCharacterSize(14);
    monsterLevelTextBox.setFillColor(Color::White);
    monsterLevelTextBox.setPosition({ monsterStatBoxX + 10.f, monsterStatBoxY + 25.f });

    monsterAttackTextBox.setString("Attack: " + to_string(static_cast<int>(monsterStats.attack)));
    monsterAttackTextBox.setCharacterSize(14);
    monsterAttackTextBox.setFillColor(Color(255, 100, 100));
    monsterAttackTextBox.setPosition({ monsterStatBoxX + 10.f, monsterStatBoxY + 45.f });

    monsterDefenseTextBox.setString("Armor: " + to_string(static_cast<int>(monsterStats.attack)));
    monsterDefenseTextBox.setCharacterSize(14);
    monsterDefenseTextBox.setFillColor(Color(150, 150, 200));
    monsterDefenseTextBox.setPosition({ monsterStatBoxX + 10.f, monsterStatBoxY + 65.f });

    monsterDexterityTextBox.setString("Dexterity: " + to_string(static_cast<int>(monsterStats.dexterity)));
    monsterDexterityTextBox.setCharacterSize(14);
    monsterDexterityTextBox.setFillColor(Color(200, 100, 255));
    monsterDexterityTextBox.setPosition({ monsterStatBoxX + 10.f, monsterStatBoxY + 85.f });

    monsterIntelligenceTextBox.setString("Intelligence: " + to_string(static_cast<int>(monsterStats.intelligence)));
    monsterIntelligenceTextBox.setCharacterSize(14);
    monsterIntelligenceTextBox.setFillColor(Color(100, 200, 255));
    monsterIntelligenceTextBox.setPosition({ monsterStatBoxX + 10.f, monsterStatBoxY + 105.f });
}

void CombatScene::updateHealthBars() {
    // Health bars stay in fixed positions - don't track character movement
    // Only update the bar size based on current health

    // Update health bar sizes based on current health
    double playerHealthPercent = playerCurrentHealth / playerSheet->getStat("Health");
    if (playerHealthPercent < 0.0) playerHealthPercent = 0.0;
    playerHealthBar.setSize({ 200.f * static_cast<float>(playerHealthPercent), 20.f });

    // Update shield bar (grey bar on top of health) - check all class systems
    int classIndex = playerSheet->getClassIndex();
    double currentShield = 0.0;

    if (classIndex == 2 && mageEquipmentEffects) {
        currentShield = mageEquipmentEffects->getShield();
    }
    else if (classIndex == 1 && bruiserEquipmentEffects) {
        currentShield = bruiserEquipmentEffects->getShield();
    }
    else if (equipmentEffects) {
        currentShield = equipmentEffects->getShield();
    }

    if (currentShield > 0.0) {
        double maxHealth = playerSheet->getStat("Health");

        // Shield is displayed as a percentage of max health
        double shieldPercent = currentShield / maxHealth;
        if (shieldPercent > 1.0) shieldPercent = 1.0;  // Cap at 100% of bar

        // Position shield bar to start where health bar ends
        float healthBarWidth = 200.f * static_cast<float>(playerHealthPercent);
        float shieldBarWidth = 200.f * static_cast<float>(shieldPercent);

        playerShieldBar.setSize({ shieldBarWidth, 20.f });
        playerShieldBar.setPosition({
            playerHealthBarBg.getPosition().x + healthBarWidth,
            playerHealthBarBg.getPosition().y
            });
    }
    else {
        playerShieldBar.setSize({ 0.f, 20.f });  // No shield
    }

    double monsterHealthPercent = monsterStats.health / monsterStats.maxHealth;
    if (monsterHealthPercent < 0.0) monsterHealthPercent = 0.0;
    monsterHealthBar.setSize({ 200.f * static_cast<float>(monsterHealthPercent), 20.f });

    // Update health text (show shield value if present)
    string playerHealthString = "HP: " + to_string(static_cast<int>(playerCurrentHealth)) +
        "/" + to_string(static_cast<int>(playerSheet->getStat("Health")));

    if (currentShield > 0) {
        playerHealthString += " [Shield: " + to_string(static_cast<int>(currentShield)) + "]";
    }

    playerHealthText.setString(playerHealthString);

    monsterHealthText.setString("HP: " + to_string(static_cast<int>(monsterStats.health)) +
        "/" + to_string(static_cast<int>(monsterStats.maxHealth)));
}

void CombatScene::draw(RenderWindow& window) {
    // Draw menu
    if (menu) {
        menu->draw(window);
    }

    // Draw sprites
    if (playerSprite) {
        window.draw(*playerSprite);
    }
    if (monsterSprite) {
        window.draw(*monsterSprite);
    }

    // Draw UI
    window.draw(playerNameText);
    window.draw(playerHealthText);
    window.draw(playerStatsText);
    window.draw(playerHealthBarBg);
    window.draw(playerHealthBar);
    window.draw(playerShieldBar);  // Draw shield on top of health bar

    window.draw(monsterNameText);
    window.draw(monsterHealthText);
    window.draw(monsterStatsText);
    window.draw(monsterHealthBarBg);
    window.draw(monsterHealthBar);

    // Draw currency
    window.draw(silverDisplayText);
    window.draw(goldDisplayText);
    window.draw(torchesDisplayText);

    // Draw speed buttons
    window.draw(speed1xButton);
    window.draw(speed1xText);
    window.draw(speed2xButton);
    window.draw(speed2xText);
    window.draw(speed3xButton);
    window.draw(speed3xText);

    // Draw character button
    window.draw(characterIconButton);
    window.draw(characterIconText);

    // Draw stat boxes
    window.draw(playerStatBox);
    window.draw(playerStatBoxTitle);
    window.draw(playerLevelText);
    window.draw(playerAttackText);
    window.draw(playerDefenseText);
    window.draw(playerDexterityText);
    window.draw(playerIntelligenceText);
    window.draw(playerLuckText);

    window.draw(monsterStatBox);
    window.draw(monsterStatBoxTitle);
    window.draw(monsterLevelTextBox);
    window.draw(monsterAttackTextBox);
    window.draw(monsterDefenseTextBox);
    window.draw(monsterDexterityTextBox);
    window.draw(monsterIntelligenceTextBox);

    // Draw character sheet if visible
    if (playerSheet) {
        playerSheet->draw(window);
    }

    // Draw end screen if combat is over
    if (showEndScreen) {
        window.draw(endScreenBackground);
        window.draw(endScreenTitle);
        window.draw(endScreenMessage);
        window.draw(continueButton);
        window.draw(continueButtonText);
    }

    // Draw floating texts LAST (on top of everything)
    for (auto& floatingText : floatingTexts) {
        floatingText.draw(window);
    }
}

void CombatScene::update(float deltaTime) {
    if (!combatEnded) {
        // Cap deltaTime to prevent huge movement spikes (especially after tabbing back in)
        if (deltaTime > 0.05f) {
            deltaTime = 0.05f; // Maximum 50ms per frame for combat
        }

        // Apply combat speed multiplier
        updateBattle(deltaTime * combatSpeed);
    }
}

void CombatScene::updateBattle(float deltaTime) {
    // Update character positions for floating text
    if (equipmentEffects) {
        Vector2f playerPos = playerSprite->getPosition();
        Vector2f enemyPos = monsterSprite->getPosition();
        equipmentEffects->setCharacterPositions(playerPos, enemyPos);
    }

    // Update floating texts
    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        it->update(deltaTime);
        if (it->isExpired()) {
            it = floatingTexts.erase(it);
        }
        else {
            ++it;
        }
    }

    switch (battleState) {
    case BattleState::Waiting:
        attackTimer -= deltaTime;
        if (attackTimer <= 0.0f) {
            // Trigger turn start effects (class-specific)
            int classIndex = playerSheet->getClassIndex();
            if (classIndex == 2 && mageEquipmentEffects) {
                mageEquipmentEffects->onTurnStart(playerCurrentHealth, playerSheet->getStat("Health"));
            }
            else if (classIndex == 1 && bruiserEquipmentEffects) {
                bruiserEquipmentEffects->onTurnStart(playerCurrentHealth, playerSheet->getStat("Health"));
            }
            else if (equipmentEffects) {
                equipmentEffects->onTurnStart();
            }

            if (playerTurn) {
                battleState = BattleState::PlayerAttacking;
            }
            else {
                battleState = BattleState::MonsterAttacking;
            }
        }
        break;

    case BattleState::PlayerAttacking: {
        // Move player towards monster
        Vector2f currentPos = playerSprite->getPosition();
        Vector2f direction = monsterBasePosition - playerBasePosition;
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0) {
            direction.x /= distance;
            direction.y /= distance;
        }

        // Calculate movement step
        float moveAmount = attackSpeed * deltaTime;

        // Check if we'll overshoot - if so, just go to target
        Vector2f toMonster = monsterBasePosition - currentPos;
        float distToMonster = sqrt(toMonster.x * toMonster.x + toMonster.y * toMonster.y);

        if (moveAmount >= distToMonster || distToMonster < 50.0f) {
            // Close enough or would overshoot - deal damage
            calculateDamage(true);
            battleState = BattleState::PlayerReturning;
        }
        else {
            // Move towards monster
            Vector2f newPos = {
                currentPos.x + direction.x * moveAmount,
                currentPos.y + direction.y * moveAmount
            };
            playerSprite->setPosition(newPos);
        }
        break;
    }

    case BattleState::PlayerReturning: {
        // Move player back to base position
        Vector2f currentPos = playerSprite->getPosition();
        Vector2f direction = playerBasePosition - currentPos;
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        // Check if close enough to snap back to base
        if (distance < 5.0f) {
            playerSprite->setPosition(playerBasePosition);

            // If Last Stand has attacks remaining, attack again immediately (Bruiser only)
            if (lastStandTriggered && lastStandAttacksRemaining > 0) {
                playerTurn = true;
                attackTimer = 0.0f; // Attack immediately
                battleState = BattleState::Waiting;
            }
            else {
                playerTurn = false;
                attackTimer = 0.2f; // 1.0f -> 0.2f (5x faster)
                battleState = BattleState::Waiting;
                // EQUIPMENT EFFECTS: Apply Poison
                if (equipmentEffects) {
                    equipmentEffects->applyPoisonDamage(monsterStats.health);

                    // CRITICAL FIX: Check if poison killed the monster
                    if (monsterStats.health <= 0.0 && !combatEnded) {
                        monsterStats.health = 0.0;
                        combatEnded = true;
                        playerWon = true;

                        cout << "[POISON KILL!] Monster defeated by poison damage!" << endl;

                        // Process equipment effects for combat end
                        int classIndex = playerSheet->getClassIndex();
                        int bonusXP = 0;
                        int bonusSilver = 0; // Note: accumulatedBonusSilver not accessible here

                        if (classIndex == 2 && mageEquipmentEffects) {
                            mageEquipmentEffects->onCombatEnd(true, bonusXP, bonusSilver, isArenaFight, 0);
                        }
                        else if (classIndex == 1 && bruiserEquipmentEffects) {
                            bruiserEquipmentEffects->onCombatEnd(true, bonusXP, bonusSilver, isArenaFight, 0);
                        }
                        else if (equipmentEffects) {
                            equipmentEffects->onCombatEnd(true, bonusXP, bonusSilver, isArenaFight);
                        }

                        if (bonusXP > 0 && playerSheet) {
                            cout << "[Equipment Bonus] +" << bonusXP << " XP!" << endl;
                            playerSheet->addXP(bonusXP);
                        }

                        setupEndScreen();
                        updateHealthBars();
                    }
                }

            }
        }
        else {
            // Calculate movement
            float moveAmount = returnSpeed * deltaTime;

            // Check if we'll overshoot
            if (moveAmount >= distance) {
                // Just snap to position
                playerSprite->setPosition(playerBasePosition);

                if (lastStandTriggered && lastStandAttacksRemaining > 0) {
                    playerTurn = true;
                    attackTimer = 0.0f;
                    battleState = BattleState::Waiting;
                }
                else {
                    playerTurn = false;
                    attackTimer = 0.2f;
                    battleState = BattleState::Waiting;
                }
            }
            else {
                // Move towards base
                if (distance > 0) {
                    direction.x /= distance;
                    direction.y /= distance;
                }

                Vector2f newPos = {
                    currentPos.x + direction.x * moveAmount,
                    currentPos.y + direction.y * moveAmount
                };
                playerSprite->setPosition(newPos);
            }
        }
        break;
    }

    case BattleState::MonsterAttacking: {
        // Move monster towards player
        Vector2f currentPos = monsterSprite->getPosition();
        Vector2f direction = playerBasePosition - monsterBasePosition;
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0) {
            direction.x /= distance;
            direction.y /= distance;
        }

        // Calculate movement step
        float moveAmount = attackSpeed * deltaTime;

        // Check if we'll overshoot or close enough
        Vector2f toPlayer = playerBasePosition - currentPos;
        float distToPlayer = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

        if (moveAmount >= distToPlayer || distToPlayer < 50.0f) {
            // Close enough or would overshoot - deal damage
            calculateDamage(false);
            battleState = BattleState::MonsterReturning;
        }
        else {
            // Move towards player
            Vector2f newPos = {
                currentPos.x + direction.x * moveAmount,
                currentPos.y + direction.y * moveAmount
            };
            monsterSprite->setPosition(newPos);
        }
        break;
    }

    case BattleState::MonsterReturning: {
        // Move monster back to base position
        Vector2f currentPos = monsterSprite->getPosition();
        Vector2f direction = monsterBasePosition - currentPos;
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        // Check if close enough to snap back
        if (distance < 5.0f) {
            monsterSprite->setPosition(monsterBasePosition);
            playerTurn = true;
            attackTimer = 0.2f; // 1.0f -> 0.2f (5x faster)
            battleState = BattleState::Waiting;
        }
        else {
            // Calculate movement
            float moveAmount = returnSpeed * deltaTime;

            // Check if we'll overshoot
            if (moveAmount >= distance) {
                // Just snap to position
                monsterSprite->setPosition(monsterBasePosition);
                playerTurn = true;
                attackTimer = 0.2f;
                battleState = BattleState::Waiting;
            }
            else {
                // Move towards base
                if (distance > 0) {
                    direction.x /= distance;
                    direction.y /= distance;
                }

                Vector2f newPos = {
                    currentPos.x + direction.x * moveAmount,
                    currentPos.y + direction.y * moveAmount
                };
                monsterSprite->setPosition(newPos);
            }
        }
        break;
    }
    }
}

double CombatScene::calculateArmorReduction(double armor, int level) {
    // Armor Reduction % = Armor / (Armor + 100 + (Level × 2))
    return armor / (armor + 100.0 + (level * 2.0));
}

void CombatScene::calculateDamage(bool isPlayerAttacking) {
    if (isPlayerAttacking) {
        // Player attacks monster
        int classIndex = playerSheet->getClassIndex();

        // EQUIPMENT EFFECTS: Player Attack Start
        double equipmentAttackBonus = 0.0;
        bool forceDoubleTap = false;
        bool forceCrit = false;
        int accumulatedBonusSilver = 0;

        if (classIndex == 2 && mageEquipmentEffects) {
            // Mage equipment effects
            mageEquipmentEffects->onPlayerAttackStart(equipmentAttackBonus, isArenaFight);
            // Add conditional bonuses based on HP
            equipmentAttackBonus += mageEquipmentEffects->getConditionalAttackBonus(
                playerCurrentHealth,
                playerSheet->getStat("Health"),
                isArenaFight
            );
            // Apply burn damage at start of player turn
            mageEquipmentEffects->applyBurnDamage(monsterStats.health);
        }
        else if (classIndex == 1 && bruiserEquipmentEffects) {
            // Bruiser equipment effects
            bruiserEquipmentEffects->onPlayerAttackStart(equipmentAttackBonus, forceCrit);
            // Add conditional bonuses based on HP
            equipmentAttackBonus += bruiserEquipmentEffects->getConditionalAttackBonus(
                playerCurrentHealth,
                playerSheet->getStat("Health")
            );
        }
        else if (equipmentEffects) {
            equipmentEffects->onPlayerAttackStart(equipmentAttackBonus, forceDoubleTap, forceCrit);
            // Add conditional bonuses based on HP
            equipmentAttackBonus += equipmentEffects->getConditionalAttackBonus(
                playerCurrentHealth,
                playerSheet->getStat("Health")
            );
        }

        // Last Stand counter-attack with guaranteed crits (Bruiser only)
        if (lastStandTriggered && lastStandAttacksRemaining > 0) {
            double baseDamage = playerSheet->getStat("Attack");

            // Orc Blood Fury applies to Last Stand attacks too
            double bloodFuryBonus = 1.0;
            if (playerRace == "Orc") {
                double maxHP = playerSheet->getStat("Health");
                double currentHPPercent = (playerCurrentHealth / maxHP) * 100.0;
                double missingHPPercent = 100.0 - currentHPPercent;

                int damageBonus = static_cast<int>(missingHPPercent / 10.0);
                if (damageBonus > 10) damageBonus = 10;

                bloodFuryBonus = 1.0 + (damageBonus / 100.0);
            }

            double armorReduction = calculateArmorReduction(monsterStats.armor, monsterStats.level);
            double finalDamage = baseDamage * 1.75 * bloodFuryBonus * (1.0 - armorReduction) * playerDamageModifier;

            monsterStats.health -= finalDamage;

            // EQUIPMENT EFFECTS: On Hit
            if (equipmentEffects) {
                equipmentEffects->onPlayerAttackHit(playerCurrentHealth, accumulatedBonusSilver, playerSheet->getStat("Health"));
            }

            lastStandAttacksRemaining--;
            cout << "[BRUISER LAST STAND] Critical Hit! Damage: " << finalDamage
                << " (Attacks remaining: " << lastStandAttacksRemaining << ")" << endl;
        }
        // Check if Bruiser Last Stand is active
        else if (bruiserLastStandActive && classIndex == 1) { // Bruiser
            // Last Stand attack: 100% crit, 175% crit damage
            double baseDamage = playerSheet->getStat("Attack") + equipmentAttackBonus;

            // Orc Blood Fury applies to Last Stand attacks too
            double bloodFuryBonus = 1.0;
            if (playerRace == "Orc") {
                double maxHP = playerSheet->getStat("Health");
                double currentHPPercent = (playerCurrentHealth / maxHP) * 100.0;
                double missingHPPercent = 100.0 - currentHPPercent;

                int damageBonus = static_cast<int>(missingHPPercent / 10.0);
                if (damageBonus > 10) damageBonus = 10;

                bloodFuryBonus = 1.0 + (damageBonus / 100.0);
            }

            double armorReduction = calculateArmorReduction(monsterStats.armor, monsterStats.level);
            double finalDamage = baseDamage * 1.75 * bloodFuryBonus * (1.0 - armorReduction) * playerDamageModifier;

            monsterStats.health -= finalDamage;

            // EQUIPMENT EFFECTS: On Hit
            if (equipmentEffects) {
                equipmentEffects->onPlayerAttackHit(playerCurrentHealth, accumulatedBonusSilver, playerSheet->getStat("Health"));
            }

            cout << "[LAST STAND] Critical Hit! Damage: " << finalDamage << endl;

            bruiserLastStandAttackCount++;
            if (bruiserLastStandAttackCount >= 2) {
                bruiserLastStandActive = false;
                // Next attack will be lethal - handled in else block below
            }
        }
        else if (bruiserLastStandUsed && !bruiserLastStandActive && classIndex == 1) {
            // Bruiser's third attack after Last Stand - LETHAL
            cout << "[LETHAL STRIKE!] Instant kill!" << endl;
            monsterStats.health = 0.0;
        }
        else if (classIndex == 2 && !mageFirstAttackUsed) { // Mage - First Attack Only
            // Mage ability: 250% Attack, Ignores armor, Cannot crit (FIRST ATTACK ONLY)
            double baseDamage = playerSheet->getStat("Attack") + equipmentAttackBonus;

            // Orc Blood Fury applies to Mage attacks too
            double bloodFuryBonus = 1.0;
            if (playerRace == "Orc") {
                double maxHP = playerSheet->getStat("Health");
                double currentHPPercent = (playerCurrentHealth / maxHP) * 100.0;
                double missingHPPercent = 100.0 - currentHPPercent;

                int damageBonus = static_cast<int>(missingHPPercent / 10.0);
                if (damageBonus > 10) damageBonus = 10;

                bloodFuryBonus = 1.0 + (damageBonus / 100.0);

                if (damageBonus > 0) {
                    cout << "[Blood Fury] Orc ability activated! +" << damageBonus << "% damage bonus" << endl;
                }
            }

            double finalDamage = baseDamage * 2.5 * bloodFuryBonus * playerDamageModifier;

            // Elf Arcane Affinity: +10% intelligence (already applied in getStat)
            if (playerRace == "Elf") {
                cout << "[Arcane Affinity] Elf racial bonus active (+10% intelligence)" << endl;
            }

            monsterStats.health -= finalDamage;

            // EQUIPMENT EFFECTS: On Hit (Mage)
            if (mageEquipmentEffects) {
                mageEquipmentEffects->onPlayerAttackHit(playerCurrentHealth, accumulatedBonusSilver, playerSheet->getStat("Health"), monsterStats.health);
            }

            mageFirstAttackUsed = true; // Mark first attack as used
            cout << "[ARCANE BLAST] Spell damage (ignores armor): " << finalDamage << endl;
        }
        else {
            // Normal attack with crit chance
            double baseDamage = playerSheet->getStat("Attack") + equipmentAttackBonus;

            // Orc Blood Fury: +1% damage for each 10% missing health (max +10%)
            double bloodFuryBonus = 1.0;
            if (playerRace == "Orc") {
                double maxHP = playerSheet->getStat("Health");
                double currentHPPercent = (playerCurrentHealth / maxHP) * 100.0;
                double missingHPPercent = 100.0 - currentHPPercent;

                // +1% damage per 10% missing health
                int damageBonus = static_cast<int>(missingHPPercent / 10.0);
                if (damageBonus > 10) damageBonus = 10; // Cap at +10%

                bloodFuryBonus = 1.0 + (damageBonus / 100.0);

                if (damageBonus > 0) {
                    cout << "[Blood Fury] Orc ability activated! +" << damageBonus << "% damage bonus ("
                        << (int)missingHPPercent << "% missing HP)" << endl;
                }
            }

            // Check for crit (all classes have 25% crit chance, 175% crit damage)
            // forceCrit flag from equipment (Rogue Cat) guarantees crit
            bool isCrit = forceCrit || ((rand() % 100) < playerSheet->getCritChance());
            double critMultiplier = isCrit ? (playerSheet->getCritDamage() / 100.0) : 1.0;

            // Apply armor reduction, damage modifier, and Blood Fury
            double armorReduction = calculateArmorReduction(monsterStats.armor, monsterStats.level);
            double finalDamage = baseDamage * critMultiplier * bloodFuryBonus * (1.0 - armorReduction) * playerDamageModifier;

            if (finalDamage < 1.0) finalDamage = 1.0;

            monsterStats.health -= finalDamage;

            // EQUIPMENT EFFECTS: On Hit (class-specific)
            if (classIndex == 2 && mageEquipmentEffects) {
                mageEquipmentEffects->onPlayerAttackHit(playerCurrentHealth, accumulatedBonusSilver, playerSheet->getStat("Health"), monsterStats.health);
            }
            else if (classIndex == 1 && bruiserEquipmentEffects) {
                bruiserEquipmentEffects->onPlayerAttackHit(playerCurrentHealth, accumulatedBonusSilver, playerSheet->getStat("Health"), monsterStats.health);
            }
            else if (equipmentEffects) {
                equipmentEffects->onPlayerAttackHit(playerCurrentHealth, accumulatedBonusSilver, playerSheet->getStat("Health"));
            }

            if (isCrit) {
                // EQUIPMENT EFFECTS: On Crit (class-specific)
                if (classIndex == 2 && mageEquipmentEffects) {
                    double critAttackBonus = 0.0;
                    mageEquipmentEffects->onPlayerCrit(critAttackBonus, playerCurrentHealth, playerSheet->getStat("Health"));
                }
                else if (classIndex == 1 && bruiserEquipmentEffects) {
                    double critAttackBonus = 0.0;
                    bruiserEquipmentEffects->onPlayerCrit(critAttackBonus, playerCurrentHealth, playerSheet->getStat("Health"));
                }
                else if (equipmentEffects) {
                    double critAttackBonus = 0.0;
                    equipmentEffects->onPlayerCrit(critAttackBonus, playerCurrentHealth, playerSheet->getStat("Health"));
                }
                cout << "[CRITICAL HIT!] Damage: " << finalDamage << endl;
            }
            else {
                cout << "[ATTACK] Damage: " << finalDamage << endl;
            }
        }

        // Check if monster is defeated
        if (monsterStats.health <= 0.0) {
            monsterStats.health = 0.0;
            combatEnded = true;
            playerWon = true;

            // NOTE: Human Silver Tongue (+5% silver gain) should be applied in the reward calculation
            // This would be implemented in the code that handles silver rewards after combat
            if (playerRace == "Human") {
                cout << "[Silver Tongue] Human ability: Remember to apply +5% silver bonus to rewards!" << endl;
            }

            // EQUIPMENT EFFECTS: Combat End (class-specific)
            int bonusXP = 0;
            int bonusSilver = accumulatedBonusSilver;

            if (classIndex == 2 && mageEquipmentEffects) {
                // Mage combat end - needs dungeon count (pass 0 for now, should be tracked externally)
                mageEquipmentEffects->onCombatEnd(true, bonusXP, bonusSilver, isArenaFight, 0);

                // Apply bonus XP
                if (bonusXP > 0 && playerSheet) {
                    cout << "[Equipment Bonus] +" << bonusXP << " XP!" << endl;
                    playerSheet->addXP(bonusXP);
                }

                // Bonus silver is handled externally
                if (bonusSilver > 0) {
                    cout << "[Equipment Bonus] Total +" << bonusSilver << " Silver from equipment!" << endl;
                }
            }
            else if (classIndex == 1 && bruiserEquipmentEffects) {
                // Bruiser combat end
                bruiserEquipmentEffects->onCombatEnd(true, bonusXP, bonusSilver, isArenaFight, 0);

                // Apply bonus XP
                if (bonusXP > 0 && playerSheet) {
                    cout << "[Equipment Bonus] +" << bonusXP << " XP!" << endl;
                    playerSheet->addXP(bonusXP);
                }

                // Bonus silver is handled externally
                if (bonusSilver > 0) {
                    cout << "[Equipment Bonus] Total +" << bonusSilver << " Silver from equipment!" << endl;
                }
            }
            else if (equipmentEffects) {
                equipmentEffects->onCombatEnd(true, bonusXP, bonusSilver, isArenaFight);

                // Apply bonus XP
                if (bonusXP > 0 && playerSheet) {
                    cout << "[Equipment Bonus] +" << bonusXP << " XP!" << endl;
                    playerSheet->addXP(bonusXP);
                }

                // Bonus silver is handled externally
                if (bonusSilver > 0) {
                    cout << "[Equipment Bonus] Total +" << bonusSilver << " Silver from equipment!" << endl;
                }
            }

            // CRITICAL FIX: Call setupEndScreen() immediately to stop combat
            setupEndScreen();
        }
    }
    else {
        // Monster attacks player

        // EQUIPMENT EFFECTS: Enemy Attack Start (class-specific)
        bool enemyShouldMiss = false;
        double enemyArmorReduction = 0.0;
        double flatDamageReduction = 0.0;
        int classIndex = playerSheet->getClassIndex();

        if (classIndex == 2 && mageEquipmentEffects) {
            mageEquipmentEffects->onEnemyAttackStart(enemyShouldMiss, enemyArmorReduction);

            if (enemyShouldMiss) {
                cout << "[Equipment Effect] Enemy attack blocked!" << endl;
                updateHealthBars();
                return;
            }
        }
        else if (classIndex == 1 && bruiserEquipmentEffects) {
            bruiserEquipmentEffects->onEnemyAttackStart(enemyShouldMiss, flatDamageReduction);

            if (enemyShouldMiss) {
                cout << "[Equipment Effect] Enemy attack blocked!" << endl;
                updateHealthBars();
                return;
            }
        }
        else if (equipmentEffects) {
            equipmentEffects->onEnemyAttackStart(enemyShouldMiss, enemyArmorReduction);

            if (enemyShouldMiss) {
                cout << "[Equipment Effect] Enemy attack missed!" << endl;
                updateHealthBars();
                return;
            }
        }

        // Assassin dodge check (classIndex == 0)
        if (classIndex == 0) {
            double dodgeChance = playerSheet->getDodgeChance();
            double dodgeRoll = (rand() % 10000) / 100.0; // 0.00 to 99.99

            if (dodgeRoll < dodgeChance) {
                // EQUIPMENT EFFECTS: On Dodge
                if (equipmentEffects) {
                    double dexBonus = 0.0;
                    equipmentEffects->onPlayerDodge(dexBonus);
                }

                cout << "[DODGE!] Attack evaded!";
                // Show Goblin bonus if at cap
                if (playerRace == "Goblin" && dodgeChance >= 50.0) {
                    cout << " (Slippery: dodge cap at 55%)";
                }
                cout << endl;
                updateHealthBars();
                return; // No damage taken
            }
        }


        // EQUIPMENT EFFECTS: Player Taking Damage (class-specific)
        double dexterityBonus = 0.0;
        bool shouldDodge = false;
        double damageReductionPercent = 0.0;

        if (classIndex == 2 && mageEquipmentEffects) {
            double intelBonus = 0.0;
            bool shouldBlock = false;
            mageEquipmentEffects->onPlayerTakeDamage(intelBonus, shouldBlock);

            if (shouldBlock) {
                cout << "[Equipment Effect] Attack blocked!" << endl;
                updateHealthBars();
                return;
            }
        }
        else if (classIndex == 1 && bruiserEquipmentEffects) {
            double incomingDamage = monsterStats.attack; // Estimate for now
            bool shouldBlock = false;
            bruiserEquipmentEffects->onPlayerTakeDamage(damageReductionPercent, shouldBlock, incomingDamage, playerCurrentHealth, playerSheet->getStat("Health"));

            if (shouldBlock) {
                cout << "[Equipment Effect] Attack blocked!" << endl;
                updateHealthBars();
                return;
            }
        }
        else if (equipmentEffects) {
            equipmentEffects->onPlayerTakeDamage(dexterityBonus, shouldDodge);

            if (shouldDodge) {
                cout << "[Equipment Effect] Auto-dodge activated!" << endl;
                updateHealthBars();
                return;
            }
        }

        // Calculate monster damage
        double baseDamage = monsterStats.attack;

        // Dwarf Arcane Shield: First attack has 50% chance to deal 50% less damage
        if (playerRace == "Dwarf" && isFirstCombatAttack) {
            isFirstCombatAttack = false; // Mark first attack as used
            int roll = rand() % 100;
            if (roll < 50) { // 50% chance
                baseDamage *= 0.5;
                cout << "[Arcane Shield] Dwarf ability activated! Enemy's first attack damage reduced by 50%!" << endl;
            }
            else {
                cout << "[Arcane Shield] Dwarf's shield failed to activate this time." << endl;
            }
        }

        // Bruiser ability check: When health drops below 20%, activate Last Stand
        if (classIndex == 1 && !bruiserLastStandUsed) { // Bruiser
            double healthPercent = (playerCurrentHealth / playerSheet->getStat("Health")) * 100.0;
            if (healthPercent <= 20.0) {
                cout << "[LAST STAND ACTIVATED!] Health dropped below 20%!" << endl;
                bruiserLastStandUsed = true;
                bruiserLastStandActive = true;
                bruiserLastStandAttackCount = 0;
            }
        }

        // Apply player armor reduction
        double playerArmor = playerSheet->getStat("Armor");
        // NOTE: Golem Stone Skin (+10%/+5% armor) should be applied permanently in CharacterSheet
        // not during combat. It's a base stat modifier, not a combat effect.

        double armorReduction = calculateArmorReduction(playerArmor,
            playerSheet->getLevel());
        double finalDamage = baseDamage * (1.0 - armorReduction);

        if (finalDamage < 1.0) finalDamage = 1.0;

        // Check if Last Stand is exhausted - if so, ANY hit is lethal (Bruiser only)
        if (lastStandActive && lastStandAttacksRemaining <= 0) {
            cout << "[LETHAL HIT!] Last Stand exhausted - any damage is fatal!" << endl;
            playerCurrentHealth = 0.0;
            combatEnded = true;
            playerWon = false;
            setupEndScreen();
            updateHealthBars();
            return;
        }

        // Apply Bruiser damage reduction percentage
        if (classIndex == 1 && bruiserEquipmentEffects) {
            double reductionPercent = bruiserEquipmentEffects->getDamageReductionPercent();
            if (reductionPercent > 0.0) {
                finalDamage *= (1.0 - reductionPercent);
                cout << "[Bruiser DR] Reduced damage by " << (reductionPercent * 100) << "%" << endl;
            }
            // Apply flat damage reduction from first attack effects
            finalDamage -= flatDamageReduction;
            if (finalDamage < 1.0) finalDamage = 1.0;
        }

        // EQUIPMENT EFFECTS: Damage Shield First (class-specific)
        if (classIndex == 2 && mageEquipmentEffects) {
            mageEquipmentEffects->damageShield(finalDamage);
        }
        else if (classIndex == 1 && bruiserEquipmentEffects) {
            bruiserEquipmentEffects->damageShield(finalDamage);
        }
        else if (equipmentEffects) {
            equipmentEffects->damageShield(finalDamage);
        }



        playerCurrentHealth -= finalDamage;
        cout << "[MONSTER ATTACK] Damage: " << finalDamage << endl;

        // Check if player is defeated
        if (playerCurrentHealth <= 0.0) {
            // Last Stand: Survive at 1 HP and get 2 attacks (BRUISER ONLY)
            if (!lastStandActive && classIndex == 1) { // Only Bruisers get Last Stand
                playerCurrentHealth = 1.0;
                lastStandActive = true;
                lastStandAttacksRemaining = 2;
                lastStandTriggered = true;

                if (isArenaFight) {
                    cout << "[BRUISER LAST STAND!] You survive at 1 HP! Striking back with 2 critical hits!" << endl;
                }
                else {
                    cout << "[BRUISER LAST STAND!] You survive at 1 HP! Striking back with 2 critical hits!" << endl;
                }

                // Set up immediate counter-attack
                playerTurn = true;
                battleState = BattleState::Waiting;
                attackTimer = 0.0f; // Attack immediately
            }
            // Last Stand already used OR not a Bruiser - this is lethal
            else {
                if (lastStandActive && lastStandAttacksRemaining <= 0) {
                    cout << "[LETHAL HIT!] Last stand exhausted!" << endl;
                }
                playerCurrentHealth = 0.0;
                combatEnded = true;
                playerWon = false;
                setupEndScreen();
            }
        }
    }

    updateHealthBars();
}

void CombatScene::setupEndScreen() {
    showEndScreen = true;

    // Semi-transparent background
    endScreenBackground.setSize({ windowWidth, windowHeight });
    endScreenBackground.setFillColor(Color(0, 0, 0, 200));

    // Title
    if (playerWon) {
        endScreenTitle.setString("VICTORY!");
        endScreenTitle.setFillColor(Color::Green);
        endScreenMessage.setString("You have defeated " + monsterName + "!");
    }
    else {
        endScreenTitle.setString("DEFEAT");
        endScreenTitle.setFillColor(Color::Red);
        endScreenMessage.setString("You were defeated by " + monsterName + "...");
    }

    endScreenTitle.setCharacterSize(80);
    FloatRect titleBounds = endScreenTitle.getLocalBounds();
    endScreenTitle.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
    endScreenTitle.setPosition({ windowWidth / 2.f, windowHeight / 2.f - 100.f });

    // Message
    endScreenMessage.setCharacterSize(36);
    endScreenMessage.setFillColor(Color::White);
    FloatRect messageBounds = endScreenMessage.getLocalBounds();
    endScreenMessage.setOrigin({ messageBounds.size.x / 2.f, messageBounds.size.y / 2.f });
    endScreenMessage.setPosition({ windowWidth / 2.f, windowHeight / 2.f });

    // Continue button
    continueButton.setSize({ 200.f, 60.f });
    continueButton.setFillColor(Color(50, 50, 150));
    continueButton.setOutlineThickness(3.f);
    continueButton.setOutlineColor(Color::White);
    continueButton.setPosition({ windowWidth / 2.f - 100.f, windowHeight / 2.f + 100.f });

    continueButtonText.setString("Continue");
    continueButtonText.setCharacterSize(32);
    continueButtonText.setFillColor(Color::White);
    FloatRect buttonTextBounds = continueButtonText.getLocalBounds();
    continueButtonText.setOrigin({ buttonTextBounds.size.x / 2.f, buttonTextBounds.size.y / 2.f });
    continueButtonText.setPosition({ windowWidth / 2.f, windowHeight / 2.f + 130.f });
}

bool CombatScene::isContinueButtonClicked(const RenderWindow& window) const {
    if (!showEndScreen) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return continueButton.getGlobalBounds().contains(mousePosF);
}

bool CombatScene::isContinueClicked(const RenderWindow& window) const {
    return isContinueButtonClicked(window);
}

int CombatScene::checkSpeedButtonClick(const RenderWindow& window) const {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (speed1xButton.getGlobalBounds().contains(mousePosF)) {
        return 1; // 1x speed
    }
    if (speed2xButton.getGlobalBounds().contains(mousePosF)) {
        return 2; // 2x speed
    }
    if (speed3xButton.getGlobalBounds().contains(mousePosF)) {
        return 3; // 3x speed
    }

    return 0; // No button clicked
}

void CombatScene::setSpeed(int speed) {
    if (speed == 1) {
        combatSpeed = 1.0f;
        speed1xButton.setFillColor(Color(100, 200, 100)); // Green
        speed2xButton.setFillColor(Color(100, 100, 100)); // Gray
        speed3xButton.setFillColor(Color(100, 100, 100)); // Gray
    }
    else if (speed == 2) {
        combatSpeed = 2.0f;
        speed1xButton.setFillColor(Color(100, 100, 100)); // Gray
        speed2xButton.setFillColor(Color(100, 200, 100)); // Green
        speed3xButton.setFillColor(Color(100, 100, 100)); // Gray
    }
    else if (speed == 3) {
        combatSpeed = 3.0f;
        speed1xButton.setFillColor(Color(100, 100, 100)); // Gray
        speed2xButton.setFillColor(Color(100, 100, 100)); // Gray
        speed3xButton.setFillColor(Color(100, 200, 100)); // Green
    }
}