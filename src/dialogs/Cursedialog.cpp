#include "CurseDialog.h"

CurseDialog::CurseDialog(float winWidth, float winHeight)
    : windowWidth(winWidth), windowHeight(winHeight), visible(false),
    titleText(font), messageText(font), closeButtonText(font) {

    if (!font.openFromFile("arial.ttf")) {
        // Handle font loading error
    }

    background.setSize({ 600.f, 300.f });
    background.setFillColor(sf::Color(30, 30, 40, 250));
    background.setOutlineThickness(5.f);
    background.setOutlineColor(sf::Color(150, 50, 200));
    background.setPosition({ (windowWidth - 600.f) / 2.f, (windowHeight - 300.f) / 2.f });

    titleText.setFont(font);
    titleText.setString("CURSE SYSTEM");
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color(200, 100, 255));
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({ titleBounds.size.x / 2.f, 0.f });
    titleText.setPosition({ windowWidth / 2.f, background.getPosition().y + 30.f });

    messageText.setFont(font);
    messageText.setCharacterSize(28);
    messageText.setFillColor(sf::Color::White);

    closeButton.setSize({ 150.f, 60.f });
    closeButton.setFillColor(sf::Color(80, 80, 120));
    closeButton.setOutlineThickness(3.f);
    closeButton.setOutlineColor(sf::Color::White);
    closeButton.setPosition({ windowWidth / 2.f - 75.f, background.getPosition().y + 220.f });

    closeButtonText.setFont(font);
    closeButtonText.setString("OK");
    closeButtonText.setCharacterSize(30);
    closeButtonText.setFillColor(sf::Color::White);
    sf::FloatRect closeBounds = closeButtonText.getLocalBounds();
    closeButtonText.setOrigin({ closeBounds.size.x / 2.f, closeBounds.size.y / 2.f });
    closeButtonText.setPosition({ closeButton.getPosition().x + 75.f, closeButton.getPosition().y + 30.f });
}

void CurseDialog::show(const std::string& message) {
    visible = true;
    messageText.setString(message);
    sf::FloatRect msgBounds = messageText.getLocalBounds();
    messageText.setOrigin({ msgBounds.size.x / 2.f, 0.f });
    messageText.setPosition({ windowWidth / 2.f, background.getPosition().y + 110.f });
}

void CurseDialog::hide() {
    visible = false;
}

void CurseDialog::draw(sf::RenderWindow& window) {
    if (!visible) return;

    window.draw(background);
    window.draw(titleText);
    window.draw(messageText);
    window.draw(closeButton);
    window.draw(closeButtonText);
}

bool CurseDialog::isCloseButtonClicked(const sf::RenderWindow& window) {
    if (!visible) return false;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return closeButton.getGlobalBounds().contains(mousePosF);
}