#include "CurseSelection.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace sf;

CurseSelection::CurseSelection(float winWidth, float winHeight)
    : windowWidth(winWidth), windowHeight(winHeight),
    hasCurrentCurse(false), isFirstCurse(false),
    curseAccepted(false), selectionMade(false),
    titleText(font), levelText(font), acceptButtonText(font), rejectButtonText(font),
    newCurseTitle(font), newCurseDescription(font), currentCurseTitle(font),
    currentCurseDescription(font), instructionText(font) {

    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Error loading font for curse selection!" << std::endl;
    }

    background.setSize({ 1200.f, 800.f });
    background.setFillColor(Color(20, 20, 30, 240));
    background.setOutlineThickness(5.f);
    background.setOutlineColor(Color(150, 50, 200));
    background.setPosition({ (windowWidth - 1200.f) / 2.f, (windowHeight - 800.f) / 2.f });

    titleText.setFont(font);
    titleText.setString("CURSE SELECTION");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color(200, 100, 255));
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({ titleBounds.size.x / 2.f, 0.f });
    titleText.setPosition({ windowWidth / 2.f, background.getPosition().y + 30.f });

    levelText.setFont(font);
    levelText.setCharacterSize(35);
    levelText.setFillColor(Color(100, 200, 255));

    acceptButton.setSize({ 250.f, 80.f });
    acceptButton.setFillColor(Color(50, 150, 50));
    acceptButton.setOutlineThickness(3.f);
    acceptButton.setOutlineColor(Color::White);
    acceptButton.setPosition({ windowWidth / 2.f - 300.f, background.getPosition().y + background.getSize().y - 120.f });

    acceptButtonText.setFont(font);
    acceptButtonText.setString("ACCEPT");
    acceptButtonText.setCharacterSize(30);
    acceptButtonText.setFillColor(Color::White);
    FloatRect acceptBounds = acceptButtonText.getLocalBounds();
    acceptButtonText.setOrigin({ acceptBounds.size.x / 2.f, acceptBounds.size.y / 2.f });
    acceptButtonText.setPosition({ acceptButton.getPosition().x + 125.f, acceptButton.getPosition().y + 40.f });

    rejectButton.setSize({ 250.f, 80.f });
    rejectButton.setFillColor(Color(150, 50, 50));
    rejectButton.setOutlineThickness(3.f);
    rejectButton.setOutlineColor(Color::White);
    rejectButton.setPosition({ windowWidth / 2.f + 50.f, background.getPosition().y + background.getSize().y - 120.f });

    rejectButtonText.setFont(font);
    rejectButtonText.setString("REJECT");
    rejectButtonText.setCharacterSize(30);
    rejectButtonText.setFillColor(Color::White);
    FloatRect rejectBounds = rejectButtonText.getLocalBounds();
    rejectButtonText.setOrigin({ rejectBounds.size.x / 2.f, rejectBounds.size.y / 2.f });
    rejectButtonText.setPosition({ rejectButton.getPosition().x + 125.f, rejectButton.getPosition().y + 40.f });

    instructionText.setFont(font);
    instructionText.setCharacterSize(25);
    instructionText.setFillColor(Color(255, 255, 100));

    newCurseTitle.setFont(font);
    newCurseTitle.setCharacterSize(32);
    newCurseTitle.setFillColor(Color(100, 255, 100));

    newCurseDescription.setFont(font);
    newCurseDescription.setCharacterSize(22);
    newCurseDescription.setFillColor(Color::White);

    currentCurseTitle.setFont(font);
    currentCurseTitle.setCharacterSize(32);
    currentCurseTitle.setFillColor(Color(255, 150, 150));

    currentCurseDescription.setFont(font);
    currentCurseDescription.setCharacterSize(22);
    currentCurseDescription.setFillColor(Color::White);
}

void CurseSelection::setupCurseChoice(const CurseData& newCurseData, const CurseData& currentCurseData,
    bool hasExistingCurse, int currentLevel, bool firstCurse) {

    newCurse = newCurseData;
    currentCurse = currentCurseData;
    hasCurrentCurse = hasExistingCurse;
    isFirstCurse = firstCurse;

    selectionMade = false;
    curseAccepted = false;

    levelText.setString("Level " + std::to_string(currentLevel) + " Curse");
    FloatRect levelBounds = levelText.getLocalBounds();
    levelText.setOrigin({ levelBounds.size.x / 2.f, 0.f });
    levelText.setPosition({ windowWidth / 2.f, background.getPosition().y + 90.f });

    float leftX = background.getPosition().x + 50.f;
    float rightX = windowWidth / 2.f + 50.f;
    float topY = background.getPosition().y + 160.f;

    newCurseTitle.setString("NEW CURSE");
    newCurseTitle.setPosition({ leftX, topY });

    newCurseDescription.setString(newCurse.curseName + "\n" + newCurse.description);
    newCurseDescription.setPosition({ leftX, topY + 50.f });

    newCurseStats.clear();
    float statY = topY + 130.f;
    const char* statNames[] = { "Health", "Attack", "Intelligence", "Dexterity", "Armor", "Luck" };
    double statValues[] = {
        newCurse.modifiers.healthModifier,
        newCurse.modifiers.attackModifier,
        newCurse.modifiers.intelligenceModifier,
        newCurse.modifiers.dexterityModifier,
        newCurse.modifiers.armorModifier,
        newCurse.modifiers.luckModifier
    };

    for (int i = 0; i < 6; ++i) {
        if (statValues[i] != 0.0) {
            Text statText(font);
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "%s: %+.1f%%", statNames[i], statValues[i] * 100.0);
            statText.setString(buffer);
            statText.setCharacterSize(24);
            statText.setFillColor(statValues[i] > 0 ? Color(100, 255, 100) : Color(255, 100, 100));
            statText.setPosition({ leftX + 20.f, statY });
            newCurseStats.push_back(statText);
            statY += 35.f;
        }
    }

    if (hasCurrentCurse && !isFirstCurse) {
        currentCurseTitle.setString("CURRENT CURSE");
        currentCurseTitle.setPosition({ rightX, topY });

        currentCurseDescription.setString(currentCurse.curseName + "\n" + currentCurse.description);
        currentCurseDescription.setPosition({ rightX, topY + 50.f });

        currentCurseStats.clear();
        statY = topY + 130.f;
        double currentStatValues[] = {
            currentCurse.modifiers.healthModifier,
            currentCurse.modifiers.attackModifier,
            currentCurse.modifiers.intelligenceModifier,
            currentCurse.modifiers.dexterityModifier,
            currentCurse.modifiers.armorModifier,
            currentCurse.modifiers.luckModifier
        };

        for (int i = 0; i < 6; ++i) {
            if (currentStatValues[i] != 0.0) {
                Text statText(font);
                char buffer[100];
                snprintf(buffer, sizeof(buffer), "%s: %+.1f%%", statNames[i], currentStatValues[i] * 100.0);
                statText.setString(buffer);
                statText.setCharacterSize(24);
                statText.setFillColor(currentStatValues[i] > 0 ? Color(100, 255, 100) : Color(255, 100, 100));
                statText.setPosition({ rightX + 20.f, statY });
                currentCurseStats.push_back(statText);
                statY += 35.f;
            }
        }

        instructionText.setString("Accept the new curse or keep your current curse");
    }
    else {
        currentCurseStats.clear();
        if (isFirstCurse) {
            instructionText.setString("First curse is automatic and permanent!");
            acceptButtonText.setString("CONTINUE");
            rejectButton.setFillColor(Color(80, 80, 80));
        }
        else {
            instructionText.setString("Accept the new curse or reject it");
        }
    }

    FloatRect instrBounds = instructionText.getLocalBounds();
    instructionText.setOrigin({ instrBounds.size.x / 2.f, 0.f });
    instructionText.setPosition({ windowWidth / 2.f, background.getPosition().y + background.getSize().y - 180.f });
}

void CurseSelection::draw(RenderWindow& window) {
    window.draw(background);
    window.draw(titleText);
    window.draw(levelText);
    window.draw(newCurseTitle);
    window.draw(newCurseDescription);

    for (const auto& stat : newCurseStats) {
        window.draw(stat);
    }

    if (hasCurrentCurse && !isFirstCurse) {
        window.draw(currentCurseTitle);
        window.draw(currentCurseDescription);
        for (const auto& stat : currentCurseStats) {
            window.draw(stat);
        }
    }

    window.draw(instructionText);
    window.draw(acceptButton);
    window.draw(acceptButtonText);

    if (!isFirstCurse) {
        window.draw(rejectButton);
        window.draw(rejectButtonText);
    }
}

void CurseSelection::handleClick(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (acceptButton.getGlobalBounds().contains(mousePosF)) {
        curseAccepted = true;
        selectionMade = true;
    }
    else if (!isFirstCurse && rejectButton.getGlobalBounds().contains(mousePosF)) {
        curseAccepted = false;
        selectionMade = true;
    }
}

CurseData CurseSelection::generateRandomCurse(int level, Faction faction) {
    int curseType = rand() % 10;

    CurseData curse;
    curse.curseId = curseType;

    double scaleFactor = 1.0 + (level / 100.0);

    switch (curseType) {
    case 0:
        curse.curseName = "Curse of Vitality";
        curse.description = "Increased health, reduced attack";
        curse.modifiers.healthModifier = 0.15 * scaleFactor;
        curse.modifiers.attackModifier = -0.05 * scaleFactor;
        break;
    case 1:
        curse.curseName = "Curse of Power";
        curse.description = "Increased attack, reduced health";
        curse.modifiers.attackModifier = 0.15 * scaleFactor;
        curse.modifiers.healthModifier = -0.05 * scaleFactor;
        break;
    case 2:
        curse.curseName = "Curse of Brilliance";
        curse.description = "Increased intelligence, reduced armor";
        curse.modifiers.intelligenceModifier = 0.15 * scaleFactor;
        curse.modifiers.armorModifier = -0.05 * scaleFactor;
        break;
    case 3:
        curse.curseName = "Curse of Agility";
        curse.description = "Increased dexterity, reduced luck";
        curse.modifiers.dexterityModifier = 0.15 * scaleFactor;
        curse.modifiers.luckModifier = -0.05 * scaleFactor;
        break;
    case 4:
        curse.curseName = "Curse of Fortune";
        curse.description = "Increased luck, reduced intelligence";
        curse.modifiers.luckModifier = 0.20 * scaleFactor;
        curse.modifiers.intelligenceModifier = -0.05 * scaleFactor;
        break;
    case 5:
        curse.curseName = "Curse of Defense";
        curse.description = "Increased armor, reduced dexterity";
        curse.modifiers.armorModifier = 0.15 * scaleFactor;
        curse.modifiers.dexterityModifier = -0.05 * scaleFactor;
        break;
    case 6:
        curse.curseName = "Curse of Balance";
        curse.description = "Moderate boost to all stats";
        curse.modifiers.healthModifier = 0.05 * scaleFactor;
        curse.modifiers.attackModifier = 0.05 * scaleFactor;
        curse.modifiers.intelligenceModifier = 0.05 * scaleFactor;
        curse.modifiers.dexterityModifier = 0.05 * scaleFactor;
        curse.modifiers.armorModifier = 0.05 * scaleFactor;
        curse.modifiers.luckModifier = 0.05 * scaleFactor;
        break;
    case 7:
        curse.curseName = "Curse of the Warrior";
        curse.description = "High attack and health, low intelligence";
        curse.modifiers.attackModifier = 0.12 * scaleFactor;
        curse.modifiers.healthModifier = 0.08 * scaleFactor;
        curse.modifiers.intelligenceModifier = -0.10 * scaleFactor;
        break;
    case 8:
        curse.curseName = "Curse of the Scholar";
        curse.description = "High intelligence, low attack";
        curse.modifiers.intelligenceModifier = 0.20 * scaleFactor;
        curse.modifiers.attackModifier = -0.10 * scaleFactor;
        break;
    case 9:
        curse.curseName = "Curse of Recklessness";
        curse.description = "High attack and dexterity, low armor";
        curse.modifiers.attackModifier = 0.10 * scaleFactor;
        curse.modifiers.dexterityModifier = 0.10 * scaleFactor;
        curse.modifiers.armorModifier = -0.15 * scaleFactor;
        break;
    }

    return curse;
}