#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

using namespace sf;

enum class ButtonID {
    None = -1,
    CharacterSheet = 0,
    Inventory = 1,
    CharacterSwitcher = 2
};

class ButtonController {
private:
    ButtonID activeButton;
    std::vector<CircleShape*> buttons;
    std::vector<ButtonID> buttonIDs;
    std::vector<std::function<void()>> onActivateCallbacks;
    std::vector<std::function<void()>> onDeactivateCallbacks;

    Color activeColor;
    Color inactiveColor;

public:
    ButtonController()
        : activeButton(ButtonID::None),
        activeColor(100, 150, 200),
        inactiveColor(100, 100, 150) {
    }

    void registerButton(CircleShape* button, ButtonID id,
        std::function<void()> onActivate,
        std::function<void()> onDeactivate) {
        buttons.push_back(button);
        buttonIDs.push_back(id);
        onActivateCallbacks.push_back(onActivate);
        onDeactivateCallbacks.push_back(onDeactivate);
        button->setFillColor(inactiveColor);
    }

    void handleClick(const RenderWindow& window) {
        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        for (size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i]->getGlobalBounds().contains(mousePosF)) {
                ButtonID clickedID = buttonIDs[i];

                if (activeButton == clickedID) {
                    deactivateButton(activeButton);
                    activeButton = ButtonID::None;
                }
                else {
                    if (activeButton != ButtonID::None) {
                        deactivateButton(activeButton);
                    }
                    activateButton(clickedID);
                    activeButton = clickedID;
                }
                return;
            }
        }
    }

    ButtonID getActiveButton() const {
        return activeButton;
    }

    void forceDeactivateAll() {
        if (activeButton != ButtonID::None) {
            deactivateButton(activeButton);
            activeButton = ButtonID::None;
        }
    }

private:
    void activateButton(ButtonID id) {
        for (size_t i = 0; i < buttonIDs.size(); ++i) {
            if (buttonIDs[i] == id) {
                buttons[i]->setFillColor(activeColor);
                if (onActivateCallbacks[i]) {
                    onActivateCallbacks[i]();
                }
                break;
            }
        }
    }

    void deactivateButton(ButtonID id) {
        for (size_t i = 0; i < buttonIDs.size(); ++i) {
            if (buttonIDs[i] == id) {
                buttons[i]->setFillColor(inactiveColor);
                if (onDeactivateCallbacks[i]) {
                    onDeactivateCallbacks[i]();
                }
                break;
            }
        }
    }
};