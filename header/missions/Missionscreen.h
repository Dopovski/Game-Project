#pragma once
#include <SFML/Graphics.hpp>
#include "Mission.h"
#include "menu.h"
#include "CharacterSheet.h"

using namespace std;
using namespace sf;

class MissionScreen {
public:
    MissionScreen(float windowWidth, float windowHeight, Mission* mission,
        Menu* menu, CharacterSheet* characterSheet,
        int* silver, int* gold, int* torches,
        CircleShape* charButton, Text* charButtonText);
    ~MissionScreen();

    void draw(RenderWindow& window);
    void update();
    bool isSkipButtonClicked(const RenderWindow& window) const;
    bool isCharacterButtonClicked(const RenderWindow& window) const;

    Menu* getMenu() { return menu; }
    CharacterSheet* getCharacterSheet() { return characterSheet; }

private:
    Font font;
    float windowWidth;
    float windowHeight;
    Mission* mission;
    Menu* menu;
    CharacterSheet* characterSheet;

    // Currency pointers
    int* silver;
    int* gold;
    int* torches;

    // Character button
    CircleShape* characterButton;
    Text* characterButtonText;

    // Currency display
    Text silverText{ font };
    Text goldText{ font };
    Text torchesText{ font };

    Text titleText{ font };
    Text npcNameText{ font };
    Text monsterNameText{ font };
    Text timerText{ font };
    Text distanceText{ font };

    RectangleShape progressBarBg;
    RectangleShape progressBarFill;

    RectangleShape skipButton;
    Text skipButtonText{ font };

    void updateTimer();
};