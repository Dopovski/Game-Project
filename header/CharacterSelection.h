#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Faction.h"

using namespace std;
using namespace sf;

struct CharacterData {
    string name;
    string className;
    string iconPath;
    string fullImagePath;
    string description;
};

class CharacterSelection {
public:
    CharacterSelection(float windowWidth, float windowHeight, Faction faction);
    ~CharacterSelection();

    void draw(RenderWindow& window);
    int checkIconClick(const RenderWindow& window);
    void selectCharacter(int index);
    bool isConfirmClicked(const RenderWindow& window);
    int getSelectedCharacterIndex() const { return selectedCharacterIndex; }
    bool isSelectionConfirmed() const { return confirmed; }

private:
    Font font;
    float windowWidth;
    float windowHeight;
    Faction playerFaction;

    int selectedCharacterIndex;
    bool confirmed;

    // Character data
    vector<CharacterData> characters;

    // UI Elements
    Text titleText{ font };

    // Character icons at bottom
    vector<RectangleShape> iconBoxes;
    vector<Sprite*> iconSprites;
    vector<Texture> iconTextures;

    // Large character display
    Sprite* largeCharacterSprite;
    Texture largeCharacterTexture;

    // Character info
    Text characterNameText{ font };
    Text characterClassText{ font };
    Text characterDescText{ font };

    // Confirm button
    RectangleShape confirmButton;
    Text confirmButtonText{ font };

    void loadCharacterData();
    void setupUI();
};