#include "CharacterSelection.h"
#include <iostream>

CharacterSelection::CharacterSelection(float winWidth, float winHeight, Faction faction)
    : windowWidth(winWidth), windowHeight(winHeight), playerFaction(faction),
    selectedCharacterIndex(0), confirmed(false), largeCharacterSprite(nullptr) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for character selection!" << endl;
    }

    loadCharacterData();
    setupUI();
}

CharacterSelection::~CharacterSelection() {
    delete largeCharacterSprite;
    for (auto sprite : iconSprites) {
        delete sprite;
    }
}

void CharacterSelection::loadCharacterData() {
    if (playerFaction == Faction::Entente) {
        // ENTENTE RACES
        characters = {
            {
                "Human",
                "Versatile & Balanced",
                "Assets/Icons/human_icon.jpg",
                "Assets/Characters/Human.jpg",
                "Adaptable and resourceful warriors.\nBalanced stats across all attributes.\nExcellent for any class.\n\nRacial Passive: Silver Tongue\n+5% silver gain from all sources"
            },
            {
                "Dwarf",
                "Strong & Sturdy",
                "Assets/Icons/dwarf_icon.jpg",
                "Assets/Characters/Dwarf.jpg",
                "Hardy mountain folk with great endurance.\nHigh defense and strength.\nNatural resistance to magic.\n\nRacial Passive: Arcane Shield\nFirst attack at start of combat has 50% chance\nto deal 50% less damage"
            },
            {
                "Golem",
                "Powerful & Durable",
                "Assets/Icons/golem_icon.jpg",
                "Assets/Characters/Golem.jpg",
                "Ancient constructs of stone and magic.\nMassive health and defense.\nSlow but incredibly powerful.\n\nRacial Passive: Stone Skin\n+10% armor (Bruiser) or +5% armor (other classes)"
            }
        };
    }
    else {
        // THRONG RACES
        characters = {
            {
                "Goblin",
                "Quick & Cunning",
                "Assets/Icons/goblin_icon.jpg",
                "Assets/Characters/Goblin.jpg",
                "Small but deadly and quick.\nHigh speed and evasion.\nExcels at hit-and-run tactics.\n\nRacial Passive: Slippery\n+5% dodge cap (50% -> 55%)"
            },
            {
                "Elf",
                "Graceful & Mystical",
                "Assets/Icons/elf_icon.jpg",
                "Assets/Characters/Elf.jpg",
                "Ancient beings attuned to magic.\nHigh magical power and agility.\nMasters of both bow and spell.\n\nRacial Passive: Arcane Affinity\n+10% intelligence scaling"
            },
            {
                "Orc",
                "Fierce & Brutal",
                "Assets/Icons/orc_icon.jpg",
                "Assets/Characters/Orc.jpg",
                "Savage warriors of immense strength.\nHigh damage and health.\nFerocious in close combat.\n\nRacial Passive: Blood Fury\nGain +1% increased damage for each 10% missing health\nMaximum bonus: +10% damage at critical health\nBonus updates dynamically as health changes"
            }
        };
    }
}

void CharacterSelection::setupUI() {
    // Title
    titleText.setString("Choose Your Race");
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
        if (iconTextures[i].loadFromFile(characters[i].iconPath)) {
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

    // Character name
    characterNameText.setCharacterSize(50);
    characterNameText.setFillColor(Color(255, 215, 0));
    characterNameText.setPosition({ 100.f, 200.f });

    // Character class
    characterClassText.setCharacterSize(35);
    characterClassText.setFillColor(Color(200, 200, 200));
    characterClassText.setPosition({ 100.f, 270.f });

    // Character description
    characterDescText.setCharacterSize(28);
    characterDescText.setFillColor(Color::White);
    characterDescText.setPosition({ 100.f, 350.f });

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

    // Load first character by default
    selectCharacter(0);
}

void CharacterSelection::selectCharacter(int index) {
    if (index < 0 || index >= characters.size()) return;

    selectedCharacterIndex = index;

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

    // Update character info
    characterNameText.setString(characters[index].name);
    characterClassText.setString(characters[index].className);
    characterDescText.setString(characters[index].description);

    // Load large character image
    delete largeCharacterSprite;
    largeCharacterSprite = nullptr;

    if (largeCharacterTexture.loadFromFile(characters[index].fullImagePath)) {
        largeCharacterTexture.setSmooth(true);
        largeCharacterSprite = new Sprite(largeCharacterTexture);

        // Scale to fill entire background (1920x1080)
        float scaleX = windowWidth / largeCharacterTexture.getSize().x;
        float scaleY = windowHeight / largeCharacterTexture.getSize().y;
        float scale = max(scaleX, scaleY); // Use max to ensure full coverage
        largeCharacterSprite->setScale({ scale, scale });

        // Position at top-left to avoid cutting the top
        largeCharacterSprite->setPosition({ 0.f, 0.f });
    }
}

void CharacterSelection::draw(RenderWindow& window) {
    // Draw large character FIRST (background)
    if (largeCharacterSprite) {
        window.draw(*largeCharacterSprite);
    }

    // Draw UI elements on top
    window.draw(titleText);
    window.draw(characterNameText);
    window.draw(characterClassText);
    window.draw(characterDescText);

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

int CharacterSelection::checkIconClick(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    for (int i = 0; i < iconBoxes.size(); ++i) {
        if (iconBoxes[i].getGlobalBounds().contains(mousePosF)) {
            return i;
        }
    }

    return -1;
}

bool CharacterSelection::isConfirmClicked(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return confirmButton.getGlobalBounds().contains(mousePosF);
}