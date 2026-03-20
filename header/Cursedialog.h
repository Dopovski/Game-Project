#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class CurseDialog {
private:
    sf::Font font;
    sf::RectangleShape background;
    sf::RectangleShape closeButton;
    sf::Text titleText;
    sf::Text messageText;
    sf::Text closeButtonText;
    bool visible;
    float windowWidth;
    float windowHeight;

public:
    CurseDialog(float winWidth, float winHeight);

    void show(const std::string& message);
    void hide();
    bool isVisible() const { return visible; }

    void draw(sf::RenderWindow& window);
    bool isCloseButtonClicked(const sf::RenderWindow& window);
};