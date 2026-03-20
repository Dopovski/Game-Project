#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;
using namespace std;

struct ArenaPlayer {
    string name;
    string imagePath;
    int level;
    int health;
    int attack;
    int defense;
    Texture* texture;
    Sprite* sprite;

    ArenaPlayer() : texture(nullptr), sprite(nullptr) {}

    ~ArenaPlayer() {
        delete sprite;
        delete texture;
    }

    // Move constructor
    ArenaPlayer(ArenaPlayer&& other) noexcept
        : name(std::move(other.name))
        , imagePath(std::move(other.imagePath))
        , level(other.level)
        , health(other.health)
        , attack(other.attack)
        , defense(other.defense)
        , texture(other.texture)
        , sprite(other.sprite) {
        other.texture = nullptr;
        other.sprite = nullptr;
    }

    // Move assignment
    ArenaPlayer& operator=(ArenaPlayer&& other) noexcept {
        if (this != &other) {
            delete sprite;
            delete texture;

            name = std::move(other.name);
            imagePath = std::move(other.imagePath);
            level = other.level;
            health = other.health;
            attack = other.attack;
            defense = other.defense;
            texture = other.texture;
            sprite = other.sprite;

            other.texture = nullptr;
            other.sprite = nullptr;
        }
        return *this;
    }

    // Delete copy operations
    ArenaPlayer(const ArenaPlayer&) = delete;
    ArenaPlayer& operator=(const ArenaPlayer&) = delete;
};

class ArenaScreen {
private:
    Font font;
    RectangleShape background;
    Text* titleText;

    vector<ArenaPlayer> players;
    vector<RectangleShape> playerBoxes;
    vector<Text> playerNames;
    vector<Text> playerStats;
    vector<RectangleShape> fightButtons;
    vector<Text> fightButtonTexts;

    float screenWidth;
    float screenHeight;
    bool visible;
    int selectedPlayerIndex;

    void loadPlayers();
    void setupUI();

public:
    ArenaScreen(float width, float height);
    ~ArenaScreen();

    void show();
    void hide();
    bool isVisible() const;
    bool isClickedOutside(const RenderWindow& window) const;
    int getFightButtonClicked(const RenderWindow& window) const;

    const ArenaPlayer& getSelectedPlayer(int index) const { return players[index]; }

    void draw(RenderWindow& window);
};