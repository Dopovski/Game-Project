#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "GameStateManager.h"

using namespace sf;
using namespace std;

class RiderScreen {
private:
    Font font;
    RectangleShape background;
    Text* titleText;

    RectangleShape horseButton;
    Text horseButtonText{ font };
    Text horseStatsText{ font };

    RectangleShape griffinButton;
    Text griffinButtonText{ font };
    Text griffinStatsText{ font };

    float screenWidth;
    float screenHeight;
    bool visible;

    void setupUI();

public:
    RiderScreen(float width, float height);
    ~RiderScreen();

    void show();
    void hide();
    bool isVisible() const { return visible; }
    bool isClickedOutside(const RenderWindow& window) const;
    int getMountButtonClicked(const RenderWindow& window) const;

    void draw(RenderWindow& window);
};