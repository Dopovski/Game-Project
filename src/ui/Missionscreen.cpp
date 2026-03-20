#include "MissionScreen.h"
#include <iostream>

MissionScreen::MissionScreen(float winWidth, float winHeight, Mission* mission,
    Menu* menu, CharacterSheet* characterSheet,
    int* silver, int* gold, int* torches,
    CircleShape* charButton, Text* charButtonText)
    : windowWidth(winWidth), windowHeight(winHeight), mission(mission),
    menu(menu), characterSheet(characterSheet),
    silver(silver), gold(gold), torches(torches),
    characterButton(charButton), characterButtonText(charButtonText) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading mission screen font!" << endl;
    }

    // Setup currency display
    silverText.setString("Silver: " + to_string(*silver));
    silverText.setCharacterSize(24);
    silverText.setFillColor(Color(192, 192, 192));
    silverText.setPosition({ 20.f, 20.f });

    goldText.setString("Gold: " + to_string(*gold));
    goldText.setCharacterSize(24);
    goldText.setFillColor(Color(255, 215, 0));
    goldText.setPosition({ 180.f, 20.f });

    torchesText.setString("Torches: " + to_string(*torches));
    torchesText.setCharacterSize(24);
    torchesText.setFillColor(Color(255, 140, 0));
    torchesText.setPosition({ 20.f, 50.f });

    // Title
    titleText.setString("Mission In Progress");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color(255, 215, 0));
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
    titleText.setPosition({ windowWidth / 2.f, 100.f });

    // NPC name
    npcNameText.setString("Quest from: " + mission->getNPCName());
    npcNameText.setCharacterSize(32);
    npcNameText.setFillColor(Color::White);
    FloatRect npcBounds = npcNameText.getLocalBounds();
    npcNameText.setOrigin({ npcBounds.size.x / 2.f, npcBounds.size.y / 2.f });
    npcNameText.setPosition({ windowWidth / 2.f, 200.f });

    // Monster name
    monsterNameText.setString("Hunting: " + mission->getMonsterName());
    monsterNameText.setCharacterSize(36);
    monsterNameText.setFillColor(Color(255, 100, 100));
    FloatRect monsterBounds = monsterNameText.getLocalBounds();
    monsterNameText.setOrigin({ monsterBounds.size.x / 2.f, monsterBounds.size.y / 2.f });
    monsterNameText.setPosition({ windowWidth / 2.f, 280.f });

    // Distance text
    string distanceStr;
    switch (mission->getType()) {
    case MissionType::Short:  distanceStr = "Distance: 1 km (12 minutes)"; break;
    case MissionType::Medium: distanceStr = "Distance: 2 km (24 minutes)"; break;
    case MissionType::Long:   distanceStr = "Distance: 3 km (36 minutes)"; break;
    }
    distanceText.setString(distanceStr);
    distanceText.setCharacterSize(28);
    distanceText.setFillColor(Color(200, 200, 200));
    FloatRect distBounds = distanceText.getLocalBounds();
    distanceText.setOrigin({ distBounds.size.x / 2.f, distBounds.size.y / 2.f });
    distanceText.setPosition({ windowWidth / 2.f, 360.f });

    // Timer text
    timerText.setCharacterSize(48);
    timerText.setFillColor(Color(100, 255, 100));
    FloatRect timerBounds = timerText.getLocalBounds();
    timerText.setOrigin({ timerBounds.size.x / 2.f, timerBounds.size.y / 2.f });
    timerText.setPosition({ windowWidth / 2.f, 460.f });

    // Progress bar
    float barWidth = 800.f;
    float barHeight = 50.f;
    progressBarBg.setSize({ barWidth, barHeight });
    progressBarBg.setFillColor(Color(50, 50, 60));
    progressBarBg.setOutlineThickness(3.f);
    progressBarBg.setOutlineColor(Color::White);
    progressBarBg.setPosition({ (windowWidth - barWidth) / 2.f, 560.f });

    progressBarFill.setSize({ 0.f, barHeight });
    progressBarFill.setFillColor(Color(50, 200, 50));
    progressBarFill.setPosition({ (windowWidth - barWidth) / 2.f, 560.f });

    // Skip button
    float skipWidth = 200.f;
    float skipHeight = 60.f;
    skipButton.setSize({ skipWidth, skipHeight });
    skipButton.setFillColor(Color(200, 100, 50));
    skipButton.setOutlineThickness(3.f);
    skipButton.setOutlineColor(Color::White);
    skipButton.setPosition({ (windowWidth - skipWidth) / 2.f, 700.f });

    skipButtonText.setString("Skip (Test)");
    skipButtonText.setCharacterSize(28);
    skipButtonText.setFillColor(Color::White);
    FloatRect skipBounds = skipButtonText.getLocalBounds();
    skipButtonText.setOrigin({ skipBounds.size.x / 2.f, skipBounds.size.y / 2.f });
    skipButtonText.setPosition({ windowWidth / 2.f, 730.f });
}

MissionScreen::~MissionScreen() {
}

void MissionScreen::update() {
    updateTimer();

    // Update progress bar
    double percent = mission->getProgressPercent() / 100.0;
    float barWidth = 800.f;
    progressBarFill.setSize({ barWidth * static_cast<float>(percent), 50.f });
}

void MissionScreen::updateTimer() {
    double remaining = mission->getRemainingSeconds();
    int minutes = static_cast<int>(remaining) / 60;
    int seconds = static_cast<int>(remaining) % 60;

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Time Remaining: %02d:%02d", minutes, seconds);
    timerText.setString(buffer);

    FloatRect timerBounds = timerText.getLocalBounds();
    timerText.setOrigin({ timerBounds.size.x / 2.f, timerBounds.size.y / 2.f });
}

void MissionScreen::draw(RenderWindow& window) {
    // Draw menu on the left
    if (menu) {
        menu->draw(window);
    }

    // Draw currency display (top-left)
    silverText.setString("Silver: " + to_string(*silver));
    goldText.setString("Gold: " + to_string(*gold));
    torchesText.setString("Torches: " + to_string(*torches));
    window.draw(silverText);
    window.draw(goldText);
    window.draw(torchesText);

    // Draw character button
    if (characterButton) {
        window.draw(*characterButton);
    }
    if (characterButtonText) {
        window.draw(*characterButtonText);
    }

    // Draw mission info (centered)
    window.draw(titleText);
    window.draw(npcNameText);
    window.draw(monsterNameText);
    window.draw(distanceText);
    window.draw(timerText);
    window.draw(progressBarBg);
    window.draw(progressBarFill);
    window.draw(skipButton);
    window.draw(skipButtonText);

    // Draw character sheet if visible (on top)
    if (characterSheet) {
        characterSheet->draw(window);
    }
}

bool MissionScreen::isCharacterButtonClicked(const RenderWindow& window) const {
    if (!characterButton) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return characterButton->getGlobalBounds().contains(mousePosF);
}

bool MissionScreen::isSkipButtonClicked(const RenderWindow& window) const {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return skipButton.getGlobalBounds().contains(mousePosF);
}