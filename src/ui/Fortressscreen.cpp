#include "FortressScreen.h"
#include <iostream>

using namespace std;
using namespace sf;

FortressScreen::FortressScreen(float windowWidth, float windowHeight)
    : visible(false), windowWidth(windowWidth), windowHeight(windowHeight) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for FortressScreen!" << endl;
    }

    // Initialize background - 1600x1080, aligned to right
    float screenWidth = 1600.0f;
    float screenHeight = 1080.0f;
    background.setSize({ screenWidth, screenHeight });
    background.setFillColor(Color(20, 20, 40, 255));
    background.setOutlineThickness(0.0f);
    background.setPosition({
        windowWidth - screenWidth,  // Align to right
        0.0f
        });

    // Title text
    titleText = new Text(font);
    titleText->setFont(font);
    titleText->setString("FORTRESS");
    titleText->setCharacterSize(60);
    titleText->setFillColor(Color(255, 215, 0));  // Gold color
    titleText->setStyle(Text::Bold);
    FloatRect titleBounds = titleText->getLocalBounds();
    titleText->setOrigin({ titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f });
    titleText->setPosition({ windowWidth - screenWidth / 2.0f, 80.0f });

    // "Work in Progress" message text
    messageText = new Text(font);
    messageText->setFont(font);
    messageText->setString("Work in Progress\n\nComing Soon!");
    messageText->setCharacterSize(48);
    messageText->setFillColor(Color(200, 200, 200));
    messageText->setStyle(Text::Bold);
    FloatRect messageBounds = messageText->getLocalBounds();
    messageText->setOrigin({ messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f });
    messageText->setPosition({
        windowWidth - screenWidth / 2.0f,
        screenHeight / 2.0f
        });
}

FortressScreen::~FortressScreen() {
    delete titleText;
    delete messageText;
}

void FortressScreen::draw(RenderWindow& window) {
    if (!visible) return;

    window.draw(background);
    window.draw(*titleText);
    window.draw(*messageText);
}

void FortressScreen::show() {
    visible = true;
}

void FortressScreen::hide() {
    visible = false;
}

bool FortressScreen::checkClickOutside(const RenderWindow& window) {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    FloatRect bounds = background.getGlobalBounds();

    // If click is outside the background, return true
    if (!bounds.contains({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {
        return true;
    }

    return false;
}