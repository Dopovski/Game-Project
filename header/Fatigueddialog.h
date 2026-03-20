#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Optional: A dialog that shows when player is too fatigued
class FatiguedDialog {
private:
    sf::RectangleShape background;
    sf::RectangleShape dialogBox;
    sf::Text* title;
    sf::Text* message;
    sf::Text* closeButtonText;
    sf::RectangleShape closeButton;
    sf::Font font;
    bool visible;

public:
    FatiguedDialog(float screenWidth, float screenHeight) {
        visible = false;
        title = nullptr;
        message = nullptr;
        closeButtonText = nullptr;

        // Semi-transparent background
        background.setSize({ screenWidth, screenHeight });
        background.setFillColor(sf::Color(0, 0, 0, 180));

        // Dialog box
        dialogBox.setSize({ 600.f, 300.f });
        dialogBox.setPosition({ screenWidth / 2.f - 300.f, screenHeight / 2.f - 150.f });
        dialogBox.setFillColor(sf::Color(40, 40, 40));
        dialogBox.setOutlineThickness(3.f);
        dialogBox.setOutlineColor(sf::Color::Red);

        // Load font
        if (font.openFromFile("arial.ttf")) {
            // Title
            title = new sf::Text(font);
            title->setFont(font);
            title->setString("TOO FATIGUED!");
            title->setCharacterSize(36);
            title->setFillColor(sf::Color::Red);
            sf::FloatRect titleBounds = title->getLocalBounds();
            title->setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
            title->setPosition({ screenWidth / 2.f, screenHeight / 2.f - 80.f });

            // Message
            message = new sf::Text(font);
            message->setFont(font);
            message->setCharacterSize(24);
            message->setFillColor(sf::Color::White);
            message->setPosition({ screenWidth / 2.f - 250.f, screenHeight / 2.f - 20.f });

            // Close button
            closeButton.setSize({ 150.f, 50.f });
            closeButton.setPosition({ screenWidth / 2.f - 75.f, screenHeight / 2.f + 80.f });
            closeButton.setFillColor(sf::Color(80, 80, 80));
            closeButton.setOutlineThickness(2.f);
            closeButton.setOutlineColor(sf::Color::White);

            closeButtonText = new sf::Text(font);
            closeButtonText->setFont(font);
            closeButtonText->setString("OK");
            closeButtonText->setCharacterSize(24);
            closeButtonText->setFillColor(sf::Color::White);
            sf::FloatRect btnTextBounds = closeButtonText->getLocalBounds();
            closeButtonText->setOrigin({ btnTextBounds.size.x / 2.f, btnTextBounds.size.y / 2.f });
            closeButtonText->setPosition({ screenWidth / 2.f, screenHeight / 2.f + 100.f });
        }
    }

    ~FatiguedDialog() {
        delete title;
        delete message;
        delete closeButtonText;
    }

    void show(const std::string& resetTime) {
        if (!message) return;

        std::string msg = "You are too exhausted to continue!\n\n";
        msg += "Your fatigue will reset at 8:00 AM.\n";
        msg += "Time until reset: " + resetTime;

        message->setString(msg);
        visible = true;
    }

    void hide() {
        visible = false;
    }

    bool isVisible() const {
        return visible;
    }

    bool isCloseButtonClicked(const sf::RenderWindow& window) {
        if (!visible) return false;

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return closeButton.getGlobalBounds().contains({
            static_cast<float>(mousePos.x),
            static_cast<float>(mousePos.y)
            });
    }

    void draw(sf::RenderWindow& window) {
        if (!visible) return;

        window.draw(background);
        window.draw(dialogBox);
        if (title) window.draw(*title);
        if (message) window.draw(*message);
        window.draw(closeButton);
        if (closeButtonText) window.draw(*closeButtonText);
    }
};