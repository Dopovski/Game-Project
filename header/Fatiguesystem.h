#pragma once
#pragma warning(disable : 4996)
#include <SFML/Graphics.hpp>
#include <ctime>
#include <string>
#include <iostream>

class FatigueSystem {
private:
    float currentFatigue;  // 0.0 to 100.0
    const float FATIGUE_PER_MISSION = 8.33f;  // 12 missions max per day
    const int RESET_HOUR = 8;  // 8 AM reset time

    sf::RectangleShape barBackground;
    sf::RectangleShape barFill;
    sf::Text* fatigueText;
    sf::Font font;

    // Refill button
    sf::RectangleShape refillButton;
    sf::Text* refillButtonText;
    bool isRefillButtonHovered;

    std::string lastResetDate;  // Format: "YYYY-MM-DD"

    // Get current date as string
    std::string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        char buffer[11];
        sprintf(buffer, "%04d-%02d-%02d",
            1900 + ltm->tm_year,
            1 + ltm->tm_mon,
            ltm->tm_mday);
        return std::string(buffer);
    }

    // Check if it's past reset time (8 AM) today
    bool isPastResetTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        return ltm->tm_hour >= RESET_HOUR;
    }

    // Check if we should reset fatigue
    bool shouldReset() {
        std::string today = getCurrentDate();

        // If last reset was yesterday or earlier, and it's past 8 AM
        if (lastResetDate != today && isPastResetTime()) {
            return true;
        }

        return false;
    }

public:
    FatigueSystem(float x, float y, float width = 300.f, float height = 30.f) {
        fatigueText = nullptr;
        refillButtonText = nullptr;
        isRefillButtonHovered = false;

        // Initialize to zero - will be loaded from character data
        currentFatigue = 0.0f;
        lastResetDate = getCurrentDate();

        // Setup bar background
        barBackground.setSize({ width, height });
        barBackground.setPosition({ x, y });
        barBackground.setFillColor(sf::Color(50, 50, 50));
        barBackground.setOutlineThickness(2.f);
        barBackground.setOutlineColor(sf::Color::White);

        // Setup bar fill
        barFill.setSize({ width * (currentFatigue / 100.f), height });
        barFill.setPosition({ x, y });
        updateBarColor();

        // Setup refill button (positioned above the bar on the right side)
        float buttonWidth = 200.f;
        float buttonHeight = 50.f;
        refillButton.setSize({ buttonWidth, buttonHeight });
        refillButton.setPosition({ x + width - buttonWidth - 10.f, y - buttonHeight - 10.f });
        refillButton.setFillColor(sf::Color(70, 130, 180));  // Steel blue
        refillButton.setOutlineThickness(2.f);
        refillButton.setOutlineColor(sf::Color::White);

        // Setup text
        if (font.openFromFile("arial.ttf")) {
            fatigueText = new sf::Text(font);
            fatigueText->setFont(font);

            // Scale text size based on bar height (larger bar = larger text)
            unsigned int textSize = static_cast<unsigned int>(height * 0.5f);  // 50% of bar height
            if (textSize < 18) textSize = 18;  // Minimum 18
            fatigueText->setCharacterSize(textSize);

            fatigueText->setFillColor(sf::Color::White);
            updateText();

            // Center text on bar
            sf::FloatRect textBounds = fatigueText->getLocalBounds();
            fatigueText->setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
            fatigueText->setPosition({ x + width / 2.f, y + height / 2.f });

            // Setup refill button text
            refillButtonText = new sf::Text(font);
            refillButtonText->setFont(font);
            refillButtonText->setCharacterSize(20);
            refillButtonText->setString("Refill 20% (1 Gold)");
            refillButtonText->setFillColor(sf::Color::White);

            // Center text on button
            sf::FloatRect btnTextBounds = refillButtonText->getLocalBounds();
            refillButtonText->setOrigin({ btnTextBounds.size.x / 2.f, btnTextBounds.size.y / 2.f });
            refillButtonText->setPosition({
                x + width - buttonWidth / 2.f - 10.f,
                y - buttonHeight / 2.f - 10.f
                });
        }
    }

    ~FatigueSystem() {
        delete fatigueText;
        delete refillButtonText;
    }

    void updateBarColor() {
        if (currentFatigue < 50.f) {
            barFill.setFillColor(sf::Color::Green);
        }
        else if (currentFatigue < 80.f) {
            barFill.setFillColor(sf::Color::Yellow);
        }
        else {
            barFill.setFillColor(sf::Color::Red);
        }
    }

    void updateText() {
        if (!fatigueText) return;

        int percentage = static_cast<int>(currentFatigue);
        fatigueText->setString("Fatigue: " + std::to_string(percentage) + "%");

        // Re-center text
        sf::FloatRect textBounds = fatigueText->getLocalBounds();
        fatigueText->setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
    }

    // Add fatigue after completing a mission
    // speedPotionReduction: 0.0 to 1.0 (e.g., 0.15 = 15% reduction from potions)
    // For mounts: Horse (0.5 speed) = 6.25% fatigue, Griffin (0.75 speed) = 5% fatigue
    void addMissionFatigue(double speedPotionReduction = 0.0) {
        if (currentFatigue < 100.f) {
            float fatigueToAdd = FATIGUE_PER_MISSION;

            // Check if this is a mount bonus (>= 0.5)
            if (speedPotionReduction >= 0.5) {
                // Mount speed bonus - use direct fatigue values
                if (speedPotionReduction >= 0.75) {
                    // Griffin: 5% fatigue (75% speed bonus)
                    fatigueToAdd = 5.0f;
                    std::cout << "Griffin mount active! Fatigue reduced to 5%" << std::endl;
                }
                else {
                    // Horse: 6.25% fatigue (50% speed bonus)
                    fatigueToAdd = 6.25f;
                    std::cout << "Horse mount active! Fatigue reduced to 6.25%" << std::endl;
                }
            }
            // Apply speed potion reduction if any (and no mount)
            else if (speedPotionReduction > 0.0) {
                fatigueToAdd *= (1.0f - static_cast<float>(speedPotionReduction));
                std::cout << "Speed potion active! Reduced fatigue gain by "
                    << (int)(speedPotionReduction * 100) << "%" << std::endl;
            }

            currentFatigue += fatigueToAdd;
            if (currentFatigue > 100.f) currentFatigue = 100.f;

            // Update visuals
            float barWidth = barBackground.getSize().x;
            barFill.setSize({ barWidth * (currentFatigue / 100.f), barBackground.getSize().y });
            updateBarColor();
            updateText();
        }
    }

    // Refill fatigue by a percentage (0-100)
    void refillFatigue(float percentage) {
        if (percentage <= 0.f) return;

        currentFatigue -= percentage;
        if (currentFatigue < 0.f) currentFatigue = 0.f;

        // Update visuals
        float barWidth = barBackground.getSize().x;
        barFill.setSize({ barWidth * (currentFatigue / 100.f), barBackground.getSize().y });
        updateBarColor();
        updateText();
    }

    // Check if refill button is clicked
    bool isRefillButtonClicked(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        return refillButton.getGlobalBounds().contains(mousePosF);
    }

    // Update button hover state (call this in mouse move event)
    void checkHover(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        bool wasHovered = isRefillButtonHovered;
        isRefillButtonHovered = refillButton.getGlobalBounds().contains(mousePosF);

        // Update button color based on hover
        if (isRefillButtonHovered) {
            refillButton.setFillColor(sf::Color(100, 160, 210));  // Lighter blue
        }
        else {
            refillButton.setFillColor(sf::Color(70, 130, 180));  // Original blue
        }
    }

    // Check if player can start a new mission
    bool canStartMission() const {
        return currentFatigue < 100.f;
    }

    // Get remaining missions available today
    int getRemainingMissions() const {
        return static_cast<int>((100.f - currentFatigue) / FATIGUE_PER_MISSION);
    }

    // Get time until next reset
    std::string getTimeUntilReset() {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        // Calculate time until 8 AM tomorrow
        tm nextReset = *ltm;
        nextReset.tm_hour = RESET_HOUR;
        nextReset.tm_min = 0;
        nextReset.tm_sec = 0;

        // If it's already past 8 AM, set to tomorrow
        if (ltm->tm_hour >= RESET_HOUR) {
            nextReset.tm_mday += 1;
        }

        time_t nextResetTime = mktime(&nextReset);
        int secondsUntil = static_cast<int>(difftime(nextResetTime, now));

        int hours = secondsUntil / 3600;
        int minutes = (secondsUntil % 3600) / 60;

        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }

    // Update method - call this each frame to check for auto-reset
    void update() {
        if (shouldReset()) {
            currentFatigue = 0.0f;
            lastResetDate = getCurrentDate();

            // Update visuals
            float barWidth = barBackground.getSize().x;
            barFill.setSize({ barWidth * (currentFatigue / 100.f), barBackground.getSize().y });
            updateBarColor();
            updateText();
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(barBackground);
        window.draw(barFill);
        if (fatigueText) {
            window.draw(*fatigueText);
        }

        // Draw refill button
        window.draw(refillButton);
        if (refillButtonText) {
            window.draw(*refillButtonText);
        }
    }

    float getCurrentFatigue() const {
        return currentFatigue;
    }

    // Get last reset date for saving with character
    std::string getLastResetDate() const {
        return lastResetDate;
    }

    // Set fatigue data when loading a character
    void setFatigueData(float fatigue, const std::string& resetDate) {
        currentFatigue = fatigue;
        lastResetDate = resetDate;

        // Update visuals
        float barWidth = barBackground.getSize().x;
        barFill.setSize({ barWidth * (currentFatigue / 100.f), barBackground.getSize().y });
        updateBarColor();
        updateText();
    }
};