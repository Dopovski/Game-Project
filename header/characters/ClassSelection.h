#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace sf;

struct ClassData {
    string name;
    string role;
    string iconPath;
    string fullImagePath;
    string description;
};

class ClassSelection {
public:
    ClassSelection(float windowWidth, float windowHeight);
    ~ClassSelection();

    void draw(RenderWindow& window);
    int checkIconClick(const RenderWindow& window);
    void selectClass(int index);
    bool isConfirmClicked(const RenderWindow& window);
    int getSelectedClassIndex() const { return selectedClassIndex; }
    bool isSelectionConfirmed() const { return confirmed; }

private:
    Font font;
    float windowWidth;
    float windowHeight;

    int selectedClassIndex;
    bool confirmed;

    // Class data
    vector<ClassData> classes;

    // UI Elements
    Text titleText{ font };

    // Class icons at bottom
    vector<RectangleShape> iconBoxes;
    vector<Sprite*> iconSprites;
    vector<Texture> iconTextures;

    // Large class display
    Sprite* largeClassSprite;
    Texture largeClassTexture;

    // Class info
    Text classNameText{ font };
    Text classRoleText{ font };
    Text classDescText{ font };

    // Confirm button
    RectangleShape confirmButton;
    Text confirmButtonText{ font };

    void loadClassData();
    void setupUI();
};