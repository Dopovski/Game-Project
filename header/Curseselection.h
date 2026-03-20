#pragma once
#include <SFML/Graphics.hpp>
#include "CurseData.h"
#include "Faction.h"
#include <vector>

class CurseSelection {
private:
    sf::Font font;
    sf::RectangleShape background;
    sf::Text titleText;
    sf::Text levelText;

    CurseData newCurse;
    CurseData currentCurse;
    bool hasCurrentCurse;
    bool isFirstCurse;

    sf::RectangleShape acceptButton;
    sf::Text acceptButtonText;

    sf::RectangleShape rejectButton;
    sf::Text rejectButtonText;

    sf::Text newCurseTitle;
    sf::Text newCurseDescription;
    std::vector<sf::Text> newCurseStats;

    sf::Text currentCurseTitle;
    sf::Text currentCurseDescription;
    std::vector<sf::Text> currentCurseStats;

    sf::Text instructionText;

    bool curseAccepted;
    bool selectionMade;

    float windowWidth;
    float windowHeight;

public:
    CurseSelection(float winWidth, float winHeight);

    void setupCurseChoice(const CurseData& newCurseData, const CurseData& currentCurseData,
        bool hasExistingCurse, int currentLevel, bool firstCurse = false);

    void draw(sf::RenderWindow& window);
    void handleClick(const sf::RenderWindow& window);

    bool isAccepted() const { return curseAccepted; }
    bool isSelectionMade() const { return selectionMade; }
    void resetSelection() { selectionMade = false; curseAccepted = false; }

    CurseData generateRandomCurse(int level, Faction faction);
};