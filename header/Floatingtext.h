#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <cmath>

class FloatingText {
private:
    inline static std::optional<sf::Font> sharedFont = std::nullopt;
    inline static bool fontInitialized = false;

    sf::Text displayText;
    sf::Vector2f startPosition;
    sf::Vector2f velocity;  // Random direction and speed
    float lifetime;
    float maxLifetime;
    bool aroundPlayer;

    // Helper to get initialized font reference
    static sf::Font& getFont() {
        if (!fontInitialized) {
            sharedFont.emplace();
            if (sharedFont->openFromFile("arial.ttf")) {
                fontInitialized = true;
            }
        }
        return *sharedFont;
    }

    // Generate random direction
    static sf::Vector2f getRandomDirection() {
        // Random angle in radians (0 to 2*PI)
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;

        // Random speed between 30 and 100 pixels per second
        float speed = 30.0f + static_cast<float>(rand() % 71);

        return sf::Vector2f(
            std::cos(angle) * speed,
            std::sin(angle) * speed
        );
    }

public:
    FloatingText(const std::string& text, sf::Vector2f position, sf::Color color, bool aroundPlayer)
        : displayText(getFont(), text, 24),
        lifetime(0.0f),
        maxLifetime(2.0f),
        aroundPlayer(aroundPlayer) {

        displayText.setFillColor(color);
        displayText.setOutlineColor(sf::Color::Black);
        displayText.setOutlineThickness(2.f);

        // Large random offset to prevent overlap (within 200x200 pixel area)
        float offsetX = static_cast<float>((rand() % 200) - 100);
        float offsetY = static_cast<float>((rand() % 200) - 100);

        startPosition.x = position.x + offsetX;
        startPosition.y = position.y + offsetY;

        // Random velocity direction
        velocity = getRandomDirection();

        displayText.setPosition(startPosition);
    }

    void update(float deltaTime) {
        lifetime += deltaTime;

        // Move in random direction
        float progress = lifetime / maxLifetime;

        sf::Vector2f newPos;
        newPos.x = startPosition.x + velocity.x * lifetime;
        newPos.y = startPosition.y + velocity.y * lifetime;
        displayText.setPosition(newPos);

        // Fade out
        sf::Color currentColor = displayText.getFillColor();
        int alpha = static_cast<int>(255 * (1.0f - progress));
        if (alpha < 0) alpha = 0;
        currentColor.a = static_cast<std::uint8_t>(alpha);
        displayText.setFillColor(currentColor);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(displayText);
    }

    bool isExpired() const {
        return lifetime >= maxLifetime;
    }
};