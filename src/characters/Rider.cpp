#include "Rider.h"
#include <iostream>

RiderScreen::RiderScreen(float width, float height)
    : screenWidth(width), screenHeight(height), visible(false), titleText(nullptr) {

    if (!font.openFromFile("arial.ttf")) {
        cout << "Error loading font for Rider!" << endl;
    }

    titleText = new Text(font);
    titleText->setString("RIDER - Choose Your Mount");
    titleText->setCharacterSize(40);
    titleText->setFillColor(Color(255, 215, 0));
    titleText->setStyle(Text::Bold);
    FloatRect titleBounds = titleText->getLocalBounds();
    titleText->setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });

    float riderWidth = 1600.f;
    float riderHeight = 1080.f;
    background.setSize({ riderWidth, riderHeight });
    background.setFillColor(Color(20, 20, 30, 255));
    background.setOutlineThickness(3.f);
    background.setOutlineColor(Color(200, 150, 50));
    background.setPosition({ screenWidth - riderWidth, 0.f });

    titleText->setPosition({
        screenWidth - riderWidth / 2.f,
        50.f
        });

    setupUI();
}

RiderScreen::~RiderScreen() {
    delete titleText;
}

void RiderScreen::setupUI() {
    float riderWidth = 1600.f;
    float riderLeft = screenWidth - riderWidth;

    float buttonWidth = 500.f;
    float buttonHeight = 400.f;
    float spacing = 100.f;
    float totalWidth = buttonWidth * 2 + spacing;
    float startX = riderLeft + (riderWidth - totalWidth) / 2.f;
    float startY = 250.f;

    // Horse button
    horseButton.setSize({ buttonWidth, buttonHeight });
    horseButton.setFillColor(Color(25, 25, 35, 255));
    horseButton.setOutlineThickness(3.f);
    horseButton.setOutlineColor(Color(200, 150, 50));
    horseButton.setPosition({ startX, startY });

    horseButtonText.setString("HORSE");
    horseButtonText.setCharacterSize(50);
    horseButtonText.setFillColor(Color(255, 215, 0));
    horseButtonText.setStyle(Text::Bold);
    FloatRect horseBtnBounds = horseButtonText.getLocalBounds();
    horseButtonText.setOrigin({ horseBtnBounds.size.x / 2.f, horseBtnBounds.size.y / 2.f });
    horseButtonText.setPosition({
        startX + buttonWidth / 2.f,
        startY + 60.f
        });

    horseStatsText.setString("Cost: 10 Gold\n\n+50% Mission Speed\n\nDuration: 7 Days");
    horseStatsText.setCharacterSize(32);
    horseStatsText.setFillColor(Color(255, 255, 255));
    horseStatsText.setStyle(Text::Bold);
    horseStatsText.setPosition({
        startX + 50.f,
        startY + 150.f
        });

    // Griffin button
    griffinButton.setSize({ buttonWidth, buttonHeight });
    griffinButton.setFillColor(Color(25, 25, 35, 255));
    griffinButton.setOutlineThickness(3.f);
    griffinButton.setOutlineColor(Color(200, 150, 50));
    griffinButton.setPosition({ startX + buttonWidth + spacing, startY });

    griffinButtonText.setString("GRIFFIN");
    griffinButtonText.setCharacterSize(50);
    griffinButtonText.setFillColor(Color(255, 215, 0));
    griffinButtonText.setStyle(Text::Bold);
    FloatRect griffinBtnBounds = griffinButtonText.getLocalBounds();
    griffinButtonText.setOrigin({ griffinBtnBounds.size.x / 2.f, griffinBtnBounds.size.y / 2.f });
    griffinButtonText.setPosition({
        startX + buttonWidth + spacing + buttonWidth / 2.f,
        startY + 60.f
        });

    griffinStatsText.setString("Cost: 20 Gold\n\n+75% Mission Speed\n\nDuration: 14 Days");
    griffinStatsText.setCharacterSize(32);
    griffinStatsText.setFillColor(Color(255, 255, 255));
    griffinStatsText.setStyle(Text::Bold);
    griffinStatsText.setPosition({
        startX + buttonWidth + spacing + 50.f,
        startY + 150.f
        });
}

void RiderScreen::show() {
    visible = true;
}

void RiderScreen::hide() {
    visible = false;
}

bool RiderScreen::isClickedOutside(const RenderWindow& window) const {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    float mouseX = static_cast<float>(mousePos.x);

    float riderLeft = screenWidth - 1600.f;

    return mouseX < riderLeft;
}

int RiderScreen::getMountButtonClicked(const RenderWindow& window) const {
    if (!visible) return -1;

    Vector2i mousePos = Mouse::getPosition(window);

    if (horseButton.getGlobalBounds().contains({
        static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y)
        })) {
        return 0; // Horse
    }

    if (griffinButton.getGlobalBounds().contains({
        static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y)
        })) {
        return 1; // Griffin
    }

    return -1;
}

void RiderScreen::draw(RenderWindow& window) {
    if (!visible) return;

    window.draw(background);

    if (titleText) {
        window.draw(*titleText);
    }

    window.draw(horseButton);
    window.draw(horseButtonText);
    window.draw(horseStatsText);

    window.draw(griffinButton);
    window.draw(griffinButtonText);
    window.draw(griffinStatsText);
}