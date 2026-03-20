#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "CharacterData.h"

using namespace std;
using namespace sf;

class CharacterSwitchScreen {
public:
    CharacterSwitchScreen(float winWidth, float winHeight, const vector<CharacterSaveData>& chars)
        : windowWidth(winWidth), windowHeight(winHeight), characters(chars),
        selectedIndex(-1), createNewRequested(false), deleteIndex(-1), showDeleteConfirm(false) {

        if (!font.openFromFile("arial.ttf")) {
            cerr << "Error loading font for character switch!" << endl;
        }

        setupUI();
    }

    void draw(RenderWindow& window) {
        window.draw(background);

        if (showDeleteConfirm) {
            // Draw confirmation dialog
            window.draw(confirmBackground);
            window.draw(confirmText);
            window.draw(confirmYesButton);
            window.draw(confirmYesText);
            window.draw(confirmNoButton);
            window.draw(confirmNoText);
        }
        else {
            window.draw(titleText);

            for (const auto& slot : characterSlots) {
                window.draw(slot);
            }

            for (const auto& text : characterTexts) {
                window.draw(text);
            }

            for (const auto& btn : deleteButtons) {
                window.draw(btn);
            }

            for (const auto& text : deleteButtonTexts) {
                window.draw(text);
            }

            window.draw(createNewButton);
            window.draw(createNewButtonText);
        }
    }

    int checkSlotClick(const RenderWindow& window) {
        if (showDeleteConfirm) return -1;

        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        for (int i = 0; i < characterSlots.size(); ++i) {
            if (characterSlots[i].getGlobalBounds().contains(mousePosF)) {
                return i;
            }
        }

        return -1;
    }

    int checkDeleteClick(const RenderWindow& window) {
        if (showDeleteConfirm) return -1;

        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        for (int i = 0; i < deleteButtons.size(); ++i) {
            if (deleteButtons[i].getGlobalBounds().contains(mousePosF)) {
                deleteIndex = i;
                showDeleteConfirm = true;
                return i;
            }
        }

        return -1;
    }

    bool isDeleteConfirmed(const RenderWindow& window) {
        if (!showDeleteConfirm) return false;

        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        if (confirmYesButton.getGlobalBounds().contains(mousePosF)) {
            showDeleteConfirm = false;
            return true;
        }

        if (confirmNoButton.getGlobalBounds().contains(mousePosF)) {
            showDeleteConfirm = false;
            deleteIndex = -1;
        }

        return false;
    }

    bool isCreateNewClicked(const RenderWindow& window) {
        if (showDeleteConfirm) return false;

        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        return createNewButton.getGlobalBounds().contains(mousePosF);
    }

    int getSelectedIndex() const { return selectedIndex; }
    int getDeleteIndex() const { return deleteIndex; }
    bool isCreateNewRequested() const { return createNewRequested; }
    bool isShowingDeleteConfirm() const { return showDeleteConfirm; }

private:
    Font font;
    float windowWidth;
    float windowHeight;
    vector<CharacterSaveData> characters;
    int selectedIndex;
    bool createNewRequested;
    int deleteIndex;
    bool showDeleteConfirm;

    RectangleShape background;
    Text titleText{ font };
    vector<RectangleShape> characterSlots;
    vector<Text> characterTexts;
    vector<RectangleShape> deleteButtons;
    vector<Text> deleteButtonTexts;
    RectangleShape createNewButton;
    Text createNewButtonText{ font };

    // Confirmation dialog
    RectangleShape confirmBackground;
    Text confirmText{ font };
    RectangleShape confirmYesButton;
    Text confirmYesText{ font };
    RectangleShape confirmNoButton;
    Text confirmNoText{ font };

    void setupUI() {
        // Background
        background.setSize({ windowWidth, windowHeight });
        background.setFillColor(Color(20, 20, 40));

        // Title
        titleText.setString("Select Character");
        titleText.setCharacterSize(60);
        titleText.setFillColor(Color::White);
        FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
        titleText.setPosition({ windowWidth / 2.f, 100.f });

        // Character slots
        float slotWidth = 600.f;
        float slotHeight = 100.f;
        float startY = 250.f;
        float spacing = 130.f;

        for (int i = 0; i < characters.size(); ++i) {
            RectangleShape slot;
            slot.setSize({ slotWidth, slotHeight });
            slot.setPosition({ (windowWidth - slotWidth) / 2.f, startY + spacing * i });
            slot.setFillColor(Color(50, 50, 80));
            slot.setOutlineThickness(3.f);
            slot.setOutlineColor(Color::White);
            characterSlots.push_back(slot);

            string displayText = characters[i].characterName + " - " +
                characters[i].raceName + " - Level " +
                to_string(characters[i].level);

            Text charText(font);
            charText.setString(displayText);
            charText.setCharacterSize(35);
            charText.setFillColor(Color::White);
            charText.setPosition({ (windowWidth - slotWidth) / 2.f + 20.f, startY + spacing * i + 30.f });
            characterTexts.push_back(charText);

            // Delete button (X)
            RectangleShape deleteBtn;
            deleteBtn.setSize({ 50.f, 50.f });
            deleteBtn.setPosition({ (windowWidth + slotWidth) / 2.f - 70.f, startY + spacing * i + 25.f });
            deleteBtn.setFillColor(Color(150, 30, 30));
            deleteBtn.setOutlineThickness(2.f);
            deleteBtn.setOutlineColor(Color::White);
            deleteButtons.push_back(deleteBtn);

            Text deleteText(font);
            deleteText.setString("X");
            deleteText.setCharacterSize(35);
            deleteText.setFillColor(Color::White);
            FloatRect deleteBounds = deleteText.getLocalBounds();
            deleteText.setOrigin({ deleteBounds.size.x / 2.f, deleteBounds.size.y / 2.f });
            deleteText.setPosition({ (windowWidth + slotWidth) / 2.f - 45.f, startY + spacing * i + 50.f });
            deleteButtonTexts.push_back(deleteText);
        }

        // Create new button
        float buttonWidth = 300.f;
        float buttonHeight = 80.f;
        createNewButton.setSize({ buttonWidth, buttonHeight });
        createNewButton.setFillColor(Color(50, 150, 50));
        createNewButton.setOutlineThickness(3.f);
        createNewButton.setOutlineColor(Color::White);
        createNewButton.setPosition({ (windowWidth - buttonWidth) / 2.f, windowHeight - 150.f });

        createNewButtonText.setString("Create New");
        createNewButtonText.setCharacterSize(35);
        createNewButtonText.setFillColor(Color::White);
        FloatRect btnBounds = createNewButtonText.getLocalBounds();
        createNewButtonText.setOrigin({ btnBounds.size.x / 2.f, btnBounds.size.y / 2.f });
        createNewButtonText.setPosition({ windowWidth / 2.f, windowHeight - 110.f });

        // Setup confirmation dialog
        float confirmWidth = 500.f;
        float confirmHeight = 250.f;
        confirmBackground.setSize({ confirmWidth, confirmHeight });
        confirmBackground.setPosition({ (windowWidth - confirmWidth) / 2.f, (windowHeight - confirmHeight) / 2.f });
        confirmBackground.setFillColor(Color(30, 30, 50));
        confirmBackground.setOutlineThickness(5.f);
        confirmBackground.setOutlineColor(Color(200, 50, 50));

        confirmText.setString("Delete this character?\nThis cannot be undone!");
        confirmText.setCharacterSize(30);
        confirmText.setFillColor(Color::White);
        FloatRect confirmBounds = confirmText.getLocalBounds();
        confirmText.setOrigin({ confirmBounds.size.x / 2.f, confirmBounds.size.y / 2.f });
        confirmText.setPosition({ windowWidth / 2.f, windowHeight / 2.f - 40.f });

        // Yes button
        float yesNoWidth = 150.f;
        float yesNoHeight = 60.f;
        confirmYesButton.setSize({ yesNoWidth, yesNoHeight });
        confirmYesButton.setPosition({ windowWidth / 2.f - yesNoWidth - 20.f, windowHeight / 2.f + 50.f });
        confirmYesButton.setFillColor(Color(150, 30, 30));
        confirmYesButton.setOutlineThickness(2.f);
        confirmYesButton.setOutlineColor(Color::White);

        confirmYesText.setString("YES");
        confirmYesText.setCharacterSize(30);
        confirmYesText.setFillColor(Color::White);
        FloatRect yesBounds = confirmYesText.getLocalBounds();
        confirmYesText.setOrigin({ yesBounds.size.x / 2.f, yesBounds.size.y / 2.f });
        confirmYesText.setPosition({ windowWidth / 2.f - yesNoWidth / 2.f - 20.f, windowHeight / 2.f + 80.f });

        // No button
        confirmNoButton.setSize({ yesNoWidth, yesNoHeight });
        confirmNoButton.setPosition({ windowWidth / 2.f + 20.f, windowHeight / 2.f + 50.f });
        confirmNoButton.setFillColor(Color(50, 150, 50));
        confirmNoButton.setOutlineThickness(2.f);
        confirmNoButton.setOutlineColor(Color::White);

        confirmNoText.setString("NO");
        confirmNoText.setCharacterSize(30);
        confirmNoText.setFillColor(Color::White);
        FloatRect noBounds = confirmNoText.getLocalBounds();
        confirmNoText.setOrigin({ noBounds.size.x / 2.f, noBounds.size.y / 2.f });
        confirmNoText.setPosition({ windowWidth / 2.f + yesNoWidth / 2.f + 20.f, windowHeight / 2.f + 80.f });
    }
};