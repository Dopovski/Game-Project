#include "ClassSelection.h"
#include <iostream>

ClassSelection::ClassSelection(float winWidth, float winHeight)
    : windowWidth(winWidth), windowHeight(winHeight),
    selectedClassIndex(0), confirmed(false), largeClassSprite(nullptr) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for class selection!" << endl;
    }

    loadClassData();
    setupUI();
}

ClassSelection::~ClassSelection() {
    delete largeClassSprite;
    for (auto sprite : iconSprites) {
        delete sprite;
    }
}

void ClassSelection::loadClassData() {
    // Define 3 classes - any race can choose any class
    classes = {
        {
            "Assassin",
            "Stealth & Speed",
            "Assets/Icons/assassin_icon.jpg",
            "Assets/Characters/Goblin.jpg",
            "Masters of stealth and deadly precision.\nHigh speed and critical strikes.\nLow health but devastating damage."
        },
        {
            "Bruiser",
            "Tank & Strength",
            "Assets/Icons/bruiser_icon.jpg",
            "Assets/Characters/Dwarf.jpg",
            "Powerful frontline warriors.\nHigh health and defense.\nDeals consistent melee damage."
        },
        {
            "Mage",
            "Magic & Control",
            "Assets/Icons/mage_icon.jpg",
            "Assets/Characters/Orc.jpg",
            "Wielders of powerful arcane magic.\nHigh magical damage and area control.\nLow defense but strong ranged attacks."
        }
    };
}

void ClassSelection::setupUI() {
    // Title
    titleText.setString("Choose Your Class");
    titleText.setCharacterSize(60);
    titleText.setFillColor(Color::White);
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
    titleText.setPosition({ windowWidth / 2.f, 80.f });

    // Create icon boxes at bottom
    float iconSize = 150.f;
    float iconSpacing = 50.f;
    float totalWidth = (iconSize * 3) + (iconSpacing * 2);
    float startX = (windowWidth - totalWidth) / 2.f;
    float iconY = windowHeight - iconSize - 100.f;

    iconTextures.resize(3);

    for (int i = 0; i < 3; ++i) {
        // Icon box
        RectangleShape iconBox;
        iconBox.setSize({ iconSize, iconSize });
        iconBox.setPosition({ startX + (iconSize + iconSpacing) * i, iconY });
        iconBox.setFillColor(Color(50, 50, 50, 200));
        iconBox.setOutlineThickness(5.f);
        iconBox.setOutlineColor(i == 0 ? Color(255, 215, 0) : Color::White);
        iconBoxes.push_back(iconBox);

        // Icon sprite
        if (iconTextures[i].loadFromFile(classes[i].iconPath)) {
            iconTextures[i].setSmooth(true);
            Sprite* iconSprite = new Sprite(iconTextures[i]);

            // Scale to fit icon box
            float scaleX = iconSize / iconTextures[i].getSize().x;
            float scaleY = iconSize / iconTextures[i].getSize().y;
            float scale = min(scaleX, scaleY);
            iconSprite->setScale({ scale, scale });
            iconSprite->setPosition({ startX + (iconSize + iconSpacing) * i, iconY });

            iconSprites.push_back(iconSprite);
        }
        else {
            iconSprites.push_back(nullptr);
        }
    }

    // Class name
    classNameText.setCharacterSize(50);
    classNameText.setFillColor(Color(255, 215, 0));
    classNameText.setPosition({ 100.f, 200.f });

    // Class role
    classRoleText.setCharacterSize(35);
    classRoleText.setFillColor(Color(200, 200, 200));
    classRoleText.setPosition({ 100.f, 270.f });

    // Class description
    classDescText.setCharacterSize(28);
    classDescText.setFillColor(Color::White);
    classDescText.setPosition({ 100.f, 350.f });

    // Confirm button
    float buttonWidth = 250.f;
    float buttonHeight = 70.f;
    confirmButton.setSize({ buttonWidth, buttonHeight });
    confirmButton.setFillColor(Color(50, 150, 50));
    confirmButton.setOutlineThickness(3.f);
    confirmButton.setOutlineColor(Color::White);
    confirmButton.setPosition({ (windowWidth - buttonWidth) / 2.f, windowHeight - buttonHeight - 20.f });

    confirmButtonText.setString("Confirm");
    confirmButtonText.setCharacterSize(35);
    confirmButtonText.setFillColor(Color::White);
    FloatRect btnBounds = confirmButtonText.getLocalBounds();
    confirmButtonText.setOrigin({ btnBounds.size.x / 2.f, btnBounds.size.y / 2.f });
    confirmButtonText.setPosition({ windowWidth / 2.f, windowHeight - buttonHeight / 2.f - 20.f });

    // Load first class by default
    selectClass(0);
}

void ClassSelection::selectClass(int index) {
    if (index < 0 || index >= classes.size()) return;

    selectedClassIndex = index;

    // Update icon borders
    for (int i = 0; i < iconBoxes.size(); ++i) {
        if (i == index) {
            iconBoxes[i].setOutlineColor(Color(255, 215, 0)); // Gold
            iconBoxes[i].setOutlineThickness(7.f);
        }
        else {
            iconBoxes[i].setOutlineColor(Color::White);
            iconBoxes[i].setOutlineThickness(5.f);
        }
    }

    // Update class info
    classNameText.setString(classes[index].name);
    classRoleText.setString(classes[index].role);
    classDescText.setString(classes[index].description);

    // Load large class image
    delete largeClassSprite;
    largeClassSprite = nullptr;

    if (largeClassTexture.loadFromFile(classes[index].fullImagePath)) {
        largeClassTexture.setSmooth(true);
        largeClassSprite = new Sprite(largeClassTexture);

        // Scale to fill entire background (1920x1080)
        float scaleX = windowWidth / largeClassTexture.getSize().x;
        float scaleY = windowHeight / largeClassTexture.getSize().y;
        float scale = max(scaleX, scaleY); // Use max to ensure full coverage
        largeClassSprite->setScale({ scale, scale });

        // Position at top-left to avoid cutting the top
        largeClassSprite->setPosition({ 0.f, 0.f });
    }
}

void ClassSelection::draw(RenderWindow& window) {
    // Draw large class image FIRST (background)
    if (largeClassSprite) {
        window.draw(*largeClassSprite);
    }

    // Draw UI elements on top
    window.draw(titleText);
    window.draw(classNameText);
    window.draw(classRoleText);
    window.draw(classDescText);

    // Draw icon boxes
    for (const auto& box : iconBoxes) {
        window.draw(box);
    }

    // Draw icon sprites
    for (auto sprite : iconSprites) {
        if (sprite) {
            window.draw(*sprite);
        }
    }

    // Draw confirm button
    window.draw(confirmButton);
    window.draw(confirmButtonText);
}

int ClassSelection::checkIconClick(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    for (int i = 0; i < iconBoxes.size(); ++i) {
        if (iconBoxes[i].getGlobalBounds().contains(mousePosF)) {
            return i;
        }
    }

    return -1;
}

bool ClassSelection::isConfirmClicked(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return confirmButton.getGlobalBounds().contains(mousePosF);
}