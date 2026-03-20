#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class FortressScreen {
public:
    FortressScreen(float windowWidth, float windowHeight);
    ~FortressScreen();

    void draw(RenderWindow& window);
    void show();
    void hide();
    bool isVisible() const { return visible; }

    // Check if clicked outside the screen (to close it)
    bool checkClickOutside(const RenderWindow& window);

private:
    bool visible;
    float windowWidth;
    float windowHeight;

    Font font;

    // UI Elements
    RectangleShape background;
    Text* titleText;
    Text* messageText;
};