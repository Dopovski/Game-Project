#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace sf;

class CurrencyManager {
private:
    int silver;
    int gold;
    int torches;

    // UI Text elements
    Font font;
    Text* silverDisplayText;
    Text* goldDisplayText;
    Text* torchesDisplayText;

public:
    CurrencyManager()
        : silver(0), gold(25), torches(0),
        silverDisplayText(nullptr),
        goldDisplayText(nullptr),
        torchesDisplayText(nullptr) {
    }

    ~CurrencyManager() {
        delete silverDisplayText;
        delete goldDisplayText;
        delete torchesDisplayText;
    }

    // Initialize UI
    void initUI() {
        if (!font.openFromFile("arial.ttf")) {
            std::cerr << "Error loading currency font!" << std::endl;
        }

        silverDisplayText = new Text(font);
        silverDisplayText->setString("Silver: " + std::to_string(silver));
        silverDisplayText->setCharacterSize(24);
        silverDisplayText->setFillColor(Color(192, 192, 192));
        silverDisplayText->setPosition({ 20.f, 20.f });

        goldDisplayText = new Text(font);
        goldDisplayText->setString("Gold: " + std::to_string(gold));
        goldDisplayText->setCharacterSize(24);
        goldDisplayText->setFillColor(Color(255, 215, 0));
        goldDisplayText->setPosition({ 180.f, 20.f });

        torchesDisplayText = new Text(font);
        torchesDisplayText->setString("Torches: " + std::to_string(torches));
        torchesDisplayText->setCharacterSize(24);
        torchesDisplayText->setFillColor(Color(255, 140, 0));
        torchesDisplayText->setPosition({ 20.f, 50.f });
    }

    // Currency getters
    int getSilver() const { return silver; }
    int getGold() const { return gold; }
    int getTorches() const { return torches; }

    // Currency setters with UI update
    void setSilver(int amount) {
        silver = amount;
        if (silverDisplayText) {
            silverDisplayText->setString("Silver: " + std::to_string(silver));
        }
    }

    void setGold(int amount) {
        gold = amount;
        if (goldDisplayText) {
            goldDisplayText->setString("Gold: " + std::to_string(gold));
        }
    }

    void setTorches(int amount) {
        torches = amount;
        if (torchesDisplayText) {
            torchesDisplayText->setString("Torches: " + std::to_string(torches));
        }
    }

    // Currency modifiers
    void addSilver(int amount) { setSilver(silver + amount); }
    void addGold(int amount) { setGold(gold + amount); }
    void addTorches(int amount) { setTorches(torches + amount); }

    void removeSilver(int amount) { setSilver(std::max(0, silver - amount)); }
    void removeGold(int amount) { setGold(std::max(0, gold - amount)); }
    void removeTorches(int amount) { setTorches(std::max(0, torches - amount)); }

    // Direct access to pointers (for passing to other systems)
    int* getSilverPtr() { return &silver; }
    int* getGoldPtr() { return &gold; }
    int* getTorchesPtr() { return &torches; }

    // Draw UI
    void draw(RenderWindow& window) const {
        if (silverDisplayText) window.draw(*silverDisplayText);
        if (goldDisplayText) window.draw(*goldDisplayText);
        if (torchesDisplayText) window.draw(*torchesDisplayText);
    }

    // Get text pointers (for cleanup or direct manipulation)
    Text* getSilverText() { return silverDisplayText; }
    Text* getGoldText() { return goldDisplayText; }
    Text* getTorchesText() { return torchesDisplayText; }
};