#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class NameInputScreen {
public:
    NameInputScreen(float winWidth, float winHeight)
        : windowWidth(winWidth), windowHeight(winHeight), confirmed(false) {

        if (!font.openFromFile("arial.ttf")) {
            cerr << "Error loading font for name input!" << endl;
        }

        setupUI();
    }

    void handleTextInput(const Event::TextEntered& textEvent) {
        if (textEvent.unicode == 8) { // Backspace
            if (!inputName.empty()) {
                inputName.pop_back();
                updateInputDisplay();
            }
        }
        else if (textEvent.unicode == 13) { // Enter
            if (!inputName.empty()) {
                confirmed = true;
            }
        }
        else if (textEvent.unicode < 128 && inputName.size() < 20) {
            char c = static_cast<char>(textEvent.unicode);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ') {
                inputName += c;
                updateInputDisplay();
            }
        }
    }

    void draw(RenderWindow& window) {
        window.draw(background);
        window.draw(titleText);
        window.draw(instructionText);
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(confirmButton);
        window.draw(confirmButtonText);
    }

    bool isConfirmClicked(const RenderWindow& window) {
        if (inputName.empty()) return false;

        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        return confirmButton.getGlobalBounds().contains(mousePosF);
    }

    string getInputName() const { return inputName; }
    bool isConfirmed() const { return confirmed; }

private:
    Font font;
    float windowWidth;
    float windowHeight;
    string inputName;
    bool confirmed;

    RectangleShape background;
    Text titleText{ font };
    Text instructionText{ font };
    RectangleShape inputBox;
    Text inputText{ font };
    RectangleShape confirmButton;
    Text confirmButtonText{ font };

    void setupUI() {
        // Background
        background.setSize({ windowWidth, windowHeight });
        background.setFillColor(Color(20, 20, 40));

        // Title
        titleText.setString("Name Your Character");
        titleText.setCharacterSize(60);
        titleText.setFillColor(Color::White);
        FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
        titleText.setPosition({ windowWidth / 2.f, 200.f });

        // Instruction
        instructionText.setString("Enter your character's name:");
        instructionText.setCharacterSize(35);
        instructionText.setFillColor(Color(200, 200, 200));
        FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin({ instrBounds.size.x / 2.f, instrBounds.size.y / 2.f });
        instructionText.setPosition({ windowWidth / 2.f, 350.f });

        // Input box
        float boxWidth = 600.f;
        float boxHeight = 80.f;
        inputBox.setSize({ boxWidth, boxHeight });
        inputBox.setPosition({ (windowWidth - boxWidth) / 2.f, 450.f });
        inputBox.setFillColor(Color(50, 50, 80));
        inputBox.setOutlineThickness(3.f);
        inputBox.setOutlineColor(Color::White);

        // Input text
        inputText.setCharacterSize(40);
        inputText.setFillColor(Color::White);
        inputText.setPosition({ (windowWidth - boxWidth) / 2.f + 20.f, 470.f });

        // Confirm button
        float buttonWidth = 250.f;
        float buttonHeight = 70.f;
        confirmButton.setSize({ buttonWidth, buttonHeight });
        confirmButton.setFillColor(Color(50, 150, 50));
        confirmButton.setOutlineThickness(3.f);
        confirmButton.setOutlineColor(Color::White);
        confirmButton.setPosition({ (windowWidth - buttonWidth) / 2.f, 650.f });

        confirmButtonText.setString("Confirm");
        confirmButtonText.setCharacterSize(35);
        confirmButtonText.setFillColor(Color::White);
        FloatRect btnBounds = confirmButtonText.getLocalBounds();
        confirmButtonText.setOrigin({ btnBounds.size.x / 2.f, btnBounds.size.y / 2.f });
        confirmButtonText.setPosition({ windowWidth / 2.f, 685.f });
    }

    void updateInputDisplay() {
        inputText.setString(inputName + "_");
    }
};