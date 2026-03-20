#include "ArenaScreen.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>
#include <utility>

ArenaScreen::ArenaScreen(float width, float height)
    : screenWidth(width), screenHeight(height), visible(false), titleText(nullptr), selectedPlayerIndex(-1) {

    // Load font first
    if (!font.openFromFile("arial.ttf")) {
        cout << "Error loading font for Arena!" << endl;
    }

    // Create and setup title text AFTER font is loaded
    titleText = new Text(font);
    titleText->setString("ARENA - Choose Your Opponent");
    titleText->setCharacterSize(40);
    titleText->setFillColor(Color(255, 215, 0));
    titleText->setStyle(Text::Bold);
    FloatRect titleBounds = titleText->getLocalBounds();
    titleText->setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });

    // Setup background - 1600x1080, aligned to right, fully opaque
    float arenaWidth = 1600.f;
    float arenaHeight = 1080.f;
    background.setSize({ arenaWidth, arenaHeight });
    background.setFillColor(Color(20, 20, 30, 255));  // Solid, not transparent
    background.setOutlineThickness(3.f);
    background.setOutlineColor(Color(200, 150, 50));
    // Position at right side of screen
    background.setPosition({ screenWidth - arenaWidth, 0.f });

    // Position title relative to arena background
    titleText->setPosition({
        screenWidth - arenaWidth / 2.f,
        50.f
        });

    loadPlayers();
    setupUI();
}

ArenaScreen::~ArenaScreen() {
    delete titleText;
}

void ArenaScreen::loadPlayers() {
    // Define player data (without textures/sprites)
    struct PlayerData {
        string name;
        string imagePath;
        int level;
        int health;
        int attack;
        int defense;
    };

    vector<PlayerData> allPlayerData = {
        {"Veron the Fierce", "Assets/NPC_Veron1.png", 5, 120, 35, 20},
        {"Orianna the Wise", "Assets/NPC_Orianna.jpg", 7, 150, 40, 25},
        {"Gorn the Brutal", "Assets/NPC_Gorn.jpg", 6, 180, 45, 30}
    };

    // Shuffle and select 3 random players
    random_device rd;
    mt19937 gen(rd());
    shuffle(allPlayerData.begin(), allPlayerData.end(), gen);

    // Take first 3 (or all if less than 3)
    int numPlayers = min(3, (int)allPlayerData.size());
    players.clear();

    for (int i = 0; i < numPlayers; i++) {
        ArenaPlayer player;
        player.name = allPlayerData[i].name;
        player.imagePath = allPlayerData[i].imagePath;
        player.level = allPlayerData[i].level;
        player.health = allPlayerData[i].health;
        player.attack = allPlayerData[i].attack;
        player.defense = allPlayerData[i].defense;

        // Create and load texture
        player.texture = new Texture();
        if (!player.texture->loadFromFile(player.imagePath)) {
            cout << "Error loading image: " << player.imagePath << endl;
            delete player.texture;
            player.texture = nullptr;
            continue;
        }
        player.texture->setSmooth(true);

        // Create and setup sprite
        player.sprite = new Sprite(*player.texture);

        // Scale image to fit in box (250x250 max for bigger display)
        FloatRect bounds = player.sprite->getLocalBounds();
        float scale = min(250.f / bounds.size.x, 250.f / bounds.size.y);
        player.sprite->setScale({ scale, scale });

        players.emplace_back(std::move(player));
    }
}

void ArenaScreen::setupUI() {
    playerBoxes.clear();
    playerNames.clear();
    playerStats.clear();
    fightButtons.clear();
    fightButtonTexts.clear();

    float arenaWidth = 1600.f;
    float arenaLeft = screenWidth - arenaWidth;

    float boxWidth = 400.f;
    float boxHeight = 650.f;  // Taller to accommodate image + stats + button
    float spacing = 50.f;
    float totalWidth = boxWidth * 3 + spacing * 2;
    float startX = arenaLeft + (arenaWidth - totalWidth) / 2.f;
    float startY = 150.f;

    for (size_t i = 0; i < players.size(); i++) {
        // Create player box
        RectangleShape box;
        box.setSize({ boxWidth, boxHeight });
        box.setFillColor(Color(25, 25, 35, 255));  // Darker, fully opaque
        box.setOutlineThickness(3.f);
        box.setOutlineColor(Color(200, 150, 50));  // Gold outline
        box.setPosition({
            startX + i * (boxWidth + spacing),
            startY
            });
        playerBoxes.push_back(box);

        // Position player sprite at top of box
        if (players[i].sprite) {
            FloatRect spriteBounds = players[i].sprite->getGlobalBounds();
            players[i].sprite->setPosition({
                box.getPosition().x + boxWidth / 2.f - spriteBounds.size.x / 2.f,
                box.getPosition().y + 30.f
                });
        }

        // Create NAME text BELOW the image
        Text nameText(font);
        nameText.setString(players[i].name);
        nameText.setCharacterSize(32);  // Large font for name
        nameText.setFillColor(Color(255, 215, 0));  // Gold color for name
        nameText.setStyle(Text::Bold);
        FloatRect nameBounds = nameText.getLocalBounds();
        nameText.setOrigin({ nameBounds.size.x / 2.f, 0.f });
        nameText.setPosition({
            box.getPosition().x + boxWidth / 2.f,
            box.getPosition().y + 310.f  // Below the image
            });
        playerNames.push_back(std::move(nameText));

        // Create STATS text below name
        Text statsText(font);
        string stats = "Level: " + to_string(players[i].level) + "\n";
        stats += "HP: " + to_string(players[i].health) + "\n";
        stats += "Attack: " + to_string(players[i].attack) + "\n";
        stats += "Defense: " + to_string(players[i].defense);
        statsText.setString(stats);
        statsText.setCharacterSize(30);  // Large, readable font
        statsText.setFillColor(Color(255, 255, 255));  // Pure white
        statsText.setStyle(Text::Bold);
        statsText.setPosition({
            box.getPosition().x + 50.f,
            box.getPosition().y + 360.f  // Below name
            });
        playerStats.push_back(std::move(statsText));

        // Create Fight button at bottom
        RectangleShape fightButton;
        fightButton.setSize({ 300.f, 80.f });  // Bigger button
        fightButton.setFillColor(Color(255, 255, 255));  // WHITE background
        fightButton.setOutlineThickness(4.f);
        fightButton.setOutlineColor(Color(200, 50, 50));  // Red outline
        fightButton.setPosition({
            box.getPosition().x + boxWidth / 2.f - 150.f,
            box.getPosition().y + boxHeight - 100.f
            });
        fightButtons.push_back(fightButton);

        // Create Fight button text
        Text fightText(font);
        fightText.setString("FIGHT");
        fightText.setCharacterSize(40);  // Large text
        fightText.setFillColor(Color(0, 0, 0));  // BLACK text on white
        fightText.setStyle(Text::Bold);
        FloatRect fightTextBounds = fightText.getLocalBounds();
        fightText.setOrigin({ fightTextBounds.size.x / 2.f, fightTextBounds.size.y / 2.f });
        fightText.setPosition({
            fightButton.getPosition().x + 150.f,
            fightButton.getPosition().y + 40.f
            });
        fightButtonTexts.push_back(std::move(fightText));
    }
}

void ArenaScreen::show() {
    visible = true;
    // Reload players for new randomization
    loadPlayers();
    setupUI();
}

void ArenaScreen::hide() {
    visible = false;
    selectedPlayerIndex = -1;
}

bool ArenaScreen::isVisible() const {
    return visible;
}

bool ArenaScreen::isClickedOutside(const RenderWindow& window) const {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    float mouseX = static_cast<float>(mousePos.x);

    // Arena is on the right side, starting at (screenWidth - 1600)
    float arenaLeft = screenWidth - 1600.f;

    // Only close if clicked on the LEFT side of the screen (dungeon area)
    // Not if clicked within or just outside the arena
    return mouseX < arenaLeft;
}

int ArenaScreen::getFightButtonClicked(const RenderWindow& window) const {
    if (!visible) return -1;

    Vector2i mousePos = Mouse::getPosition(window);

    for (size_t i = 0; i < fightButtons.size(); i++) {
        if (fightButtons[i].getGlobalBounds().contains({
            static_cast<float>(mousePos.x),
            static_cast<float>(mousePos.y)
            })) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void ArenaScreen::draw(RenderWindow& window) {
    if (!visible) return;

    // Draw background
    window.draw(background);

    // Draw title
    if (titleText) {
        window.draw(*titleText);
    }

    // Draw player boxes and content
    for (size_t i = 0; i < playerBoxes.size(); i++) {
        window.draw(playerBoxes[i]);

        if (i < players.size() && players[i].sprite) {
            window.draw(*players[i].sprite);
        }

        if (i < playerNames.size()) {
            window.draw(playerNames[i]);
        }

        if (i < playerStats.size()) {
            window.draw(playerStats[i]);
        }

        if (i < fightButtons.size()) {
            window.draw(fightButtons[i]);
        }

        if (i < fightButtonTexts.size()) {
            window.draw(fightButtonTexts[i]);
        }
    }
}