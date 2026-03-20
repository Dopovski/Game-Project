#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "menu.h"
#include "CharacterSheet.h"

using namespace std;
using namespace sf;

enum class DoorPath {
    Normal,        // Path 1: Normal fight
    Ambushed,      // Path 2: Lose 5% silver
    Ambush         // Path 3: Deal 5% more damage
};

class DoorSelection {
public:
    DoorSelection(float windowWidth, float windowHeight,
        Menu* menu, CharacterSheet* characterSheet,
        int silver, int gold, int torches);
    ~DoorSelection();

    void draw(RenderWindow& window);
    void update(float deltaTime);
    int checkDoorClick(const RenderWindow& window) const;
    int checkTorchButtonClick(const RenderWindow& window) const;
    bool hasEnoughTorches() const { return currentTorches > 0; }
    void useTorch(int doorIndex);
    bool isDoorRevealed(int doorIndex) const;
    DoorPath getDoorPath(int doorIndex) const;
    int getCurrentTorches() const { return currentTorches; }
    void updateTorchDisplay(int torches);

    // Confirmation screen
    void showConfirmation(int doorIndex);
    bool isConfirmationVisible() const { return showingConfirmation; }
    bool isConfirmationFinished() const { return confirmationTimer <= 0.0f && showingConfirmation; }
    int getSelectedDoor() const { return selectedDoorIndex; }

    Menu* getMenu() { return menu; }
    CharacterSheet* getCharacterSheet() { return characterSheet; }
    CircleShape& getCharacterButton() { return characterIconButton; }

private:
    Font font;
    float windowWidth;
    float windowHeight;

    Menu* menu;
    CharacterSheet* characterSheet;

    vector<RectangleShape> doors;
    vector<Text> doorLabels;
    vector<RectangleShape> torchButtons;
    vector<Text> torchButtonTexts;
    vector<Text> doorInfoTexts;
    vector<DoorPath> doorPaths;
    vector<bool> doorRevealed;

    int currentTorches;

    Text titleText{ font };

    CircleShape characterIconButton;
    Text characterIconText{ font };

    Text silverDisplayText{ font };
    Text goldDisplayText{ font };
    Text torchesDisplayText{ font };

    // Confirmation screen
    bool showingConfirmation;
    int selectedDoorIndex;
    float confirmationTimer;
    RectangleShape confirmationBackground;
    RectangleShape confirmationBox;
    Text confirmationTitle{ font };
    Text confirmationMessage{ font };

    void assignRandomPaths();
    string getPathDescription(DoorPath path) const;
    string getPathTitle(DoorPath path) const;
};