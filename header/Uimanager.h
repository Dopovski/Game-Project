#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "ButtonController.h"

using namespace sf;

class UIManager {
private:
    CircleShape characterIconButton;
    Font characterIconFont;
    Text* characterIconText;

    CircleShape inventoryIconButton;
    Text* inventoryIconText;

    CircleShape characterSwitchButton;
    Text* characterSwitchText;

    ButtonController buttonController;

    float windowWidth;
    float windowHeight;

    bool characterSheetVisible;
    bool inventoryVisible;
    bool switchScreenVisible;

public:
    UIManager(float winWidth, float winHeight)
        : windowWidth(winWidth),
        windowHeight(winHeight),
        characterIconText(nullptr),
        inventoryIconText(nullptr),
        characterSwitchText(nullptr),
        characterSheetVisible(false),
        inventoryVisible(false),
        switchScreenVisible(false) {
    }

    ~UIManager() {
        delete characterIconText;
        delete inventoryIconText;
        delete characterSwitchText;
    }

    void initCharacterButton() {
        if (!characterIconFont.openFromFile("arial.ttf")) {
            std::cerr << "Error loading character button font!" << std::endl;
        }

        characterIconButton.setRadius(40.f);
        characterIconButton.setFillColor(Color(100, 100, 150));
        characterIconButton.setOutlineThickness(3.f);
        characterIconButton.setOutlineColor(Color::White);
        characterIconButton.setPosition({ 20.f, windowHeight - 100.f });

        characterIconText = new Text(characterIconFont);
        characterIconText->setString("C");
        characterIconText->setCharacterSize(40);
        characterIconText->setFillColor(Color::White);
        FloatRect textRect = characterIconText->getLocalBounds();
        characterIconText->setOrigin({
            textRect.position.x + textRect.size.x / 2.f,
            textRect.position.y + textRect.size.y / 2.f
            });
        characterIconText->setPosition({ 60.f, windowHeight - 60.f });
    }

    void initInventoryButton() {
        if (!characterIconFont.openFromFile("arial.ttf")) {
            std::cerr << "Error loading inventory button font!" << std::endl;
        }

        inventoryIconButton.setRadius(40.f);
        inventoryIconButton.setFillColor(Color(100, 100, 150));
        inventoryIconButton.setOutlineThickness(3.f);
        inventoryIconButton.setOutlineColor(Color::White);
        inventoryIconButton.setPosition({ 120.f, windowHeight - 100.f });

        inventoryIconText = new Text(characterIconFont);
        inventoryIconText->setString("I");
        inventoryIconText->setCharacterSize(40);
        inventoryIconText->setFillColor(Color::White);
        FloatRect textRect = inventoryIconText->getLocalBounds();
        inventoryIconText->setOrigin({
            textRect.position.x + textRect.size.x / 2.f,
            textRect.position.y + textRect.size.y / 2.f
            });
        inventoryIconText->setPosition({ 160.f, windowHeight - 60.f });
    }

    void initCharacterSwitchButton(Font& font) {
        characterSwitchButton.setRadius(40.f);
        characterSwitchButton.setFillColor(Color(100, 100, 150));
        characterSwitchButton.setOutlineThickness(3.f);
        characterSwitchButton.setOutlineColor(Color::White);
        characterSwitchButton.setPosition({ 220.f, windowHeight - 100.f });

        characterSwitchText = new Text(font);
        characterSwitchText->setString("S");
        characterSwitchText->setCharacterSize(40);
        characterSwitchText->setFillColor(Color::White);
        FloatRect textRect = characterSwitchText->getLocalBounds();
        characterSwitchText->setOrigin({
            textRect.position.x + textRect.size.x / 2.f,
            textRect.position.y + textRect.size.y / 2.f
            });
        characterSwitchText->setPosition({ 260.f, windowHeight - 60.f });
    }

    void registerButtons() {
        buttonController.registerButton(
            &characterIconButton,
            ButtonID::CharacterSheet,
            [this]() { characterSheetVisible = true; },
            [this]() { characterSheetVisible = false; }
        );

        buttonController.registerButton(
            &inventoryIconButton,
            ButtonID::Inventory,
            [this]() { inventoryVisible = true; },
            [this]() { inventoryVisible = false; }
        );

        buttonController.registerButton(
            &characterSwitchButton,
            ButtonID::CharacterSwitcher,
            [this]() { switchScreenVisible = true; },
            [this]() { switchScreenVisible = false; }
        );
    }

    void handleButtonClick(const RenderWindow& window) {
        buttonController.handleClick(window);
    }

    bool isCharacterSheetActive() const {
        return characterSheetVisible;
    }

    bool isInventoryActive() const {
        return inventoryVisible;
    }

    bool isSwitchScreenActive() const {
        return switchScreenVisible;
    }

    void forceDeactivateAll() {
        buttonController.forceDeactivateAll();
    }

    void drawCharacterButton(RenderWindow& window) const {
        window.draw(characterIconButton);
        if (characterIconText) {
            window.draw(*characterIconText);
        }
    }

    void drawInventoryButton(RenderWindow& window) const {
        window.draw(inventoryIconButton);
        if (inventoryIconText) {
            window.draw(*inventoryIconText);
        }
    }

    void drawCharacterSwitchButton(RenderWindow& window) const {
        window.draw(characterSwitchButton);
        if (characterSwitchText) {
            window.draw(*characterSwitchText);
        }
    }

    CircleShape& getCharacterButton() { return characterIconButton; }
    Text* getCharacterText() { return characterIconText; }

    CircleShape& getInventoryButton() { return inventoryIconButton; }
    Text* getInventoryText() { return inventoryIconText; }

    CircleShape& getCharacterSwitchButton() { return characterSwitchButton; }
    Text* getCharacterSwitchText() { return characterSwitchText; }
};