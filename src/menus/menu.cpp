#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "menu.h"


using namespace std;
using namespace sf;

Menu::Menu(float width, float height) : selectedItemIndex(0) {  // FIX: Initialize selectedItemIndex
    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font! Ensure 'arial.ttf' is in the working directory." << endl;
    }

    string options[MAX_ITEMS] = { "Dungeons",
        "Arena",
        "Shop",
        "Brewer",
        "Rider",
        "Fortress",
        "Curse",
        "Conquerors",
        "Guild" };

    const float BUTTON_WIDTH = 300.0f;
    const float BUTTON_HEIGHT = 70.0f;
    const float BUTTON_X_OFFSET = 20.0f;
    const float VERTICAL_SPACING = height / (MAX_ITEMS + 1);

    const Color BUTTON_COLOR_DEFAULT = Color(0, 0, 100, 180);
    const Color BUTTON_COLOR_HOVER = Color(0, 50, 200, 255);
    const Color OUTLINE_COLOR = Color::Black;

    for (int i = 0; i < MAX_ITEMS; ++i) {
        float centerY = VERTICAL_SPACING * (i + 1);
        float rectY = centerY - BUTTON_HEIGHT / 2.0f;
        float textX = BUTTON_X_OFFSET + BUTTON_WIDTH / 2.f;

        Text buttonText(font);
        buttonText.setString(options[i]);
        buttonText.setCharacterSize(40);
        buttonText.setFillColor(Color::White);

        FloatRect textRect = buttonText.getLocalBounds();
        float originX = textRect.position.x + textRect.size.x / 2.f;
        float originY = textRect.position.y + textRect.size.y / 2.f;
        buttonText.setOrigin({ originX, originY });
        buttonText.setPosition({ textX, centerY });

        RectangleShape buttonRect({ BUTTON_WIDTH, BUTTON_HEIGHT });
        buttonRect.setFillColor(BUTTON_COLOR_DEFAULT);
        buttonRect.setOutlineThickness(3.0f);
        buttonRect.setOutlineColor(OUTLINE_COLOR);
        buttonRect.setPosition({ BUTTON_X_OFFSET, rectY });

        menuItems.push_back({
            buttonText,
            buttonRect
            });
    }

    // Set first button as selected
    menuItems[0].buttonRect.setFillColor(BUTTON_COLOR_HOVER);
}


Menu::~Menu() {
}

void Menu::draw(RenderWindow& window) {
    for (const auto& item : menuItems) {
        window.draw(item.buttonRect);
        window.draw(item.text);
    }
}

void Menu::checkMousePosition(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    const Color BUTTON_COLOR_DEFAULT = Color(0, 0, 100, 180);
    const Color BUTTON_COLOR_HOVER = Color(0, 50, 200, 255);

    int oldSelectedIndex = selectedItemIndex;
    bool mouseOverAnyButton = false;

    for (int i = 0; i < MAX_ITEMS; ++i) {
        FloatRect bounds = menuItems[i].buttonRect.getGlobalBounds();

        if (bounds.contains({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {
            mouseOverAnyButton = true;

            if (selectedItemIndex != i) {
                // Deselect old button
                if (oldSelectedIndex >= 0 && oldSelectedIndex < MAX_ITEMS) {
                    menuItems[oldSelectedIndex].buttonRect.setFillColor(BUTTON_COLOR_DEFAULT);
                }

                // Select new button
                selectedItemIndex = i;
                menuItems[selectedItemIndex].buttonRect.setFillColor(BUTTON_COLOR_HOVER);
            }
            return;
        }
    }

    // Mouse is not over any button - reset all to default
    if (!mouseOverAnyButton) {
        for (int i = 0; i < MAX_ITEMS; ++i) {
            menuItems[i].buttonRect.setFillColor(BUTTON_COLOR_DEFAULT);
        }
        selectedItemIndex = -1;
    }
}

int Menu::getClickedItemIndex(const RenderWindow& window) const {
    Vector2i mousePos = Mouse::getPosition(window);

    for (int i = 0; i < MAX_ITEMS; ++i) {
        FloatRect bounds = menuItems[i].buttonRect.getGlobalBounds();

        if (bounds.contains({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {
            return i;
        }
    }
    return -1;
}