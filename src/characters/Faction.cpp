#include "Faction.h"
#include <SFML/Config.hpp>
#include <iostream>

using namespace std;
using namespace sf;

FactionSelection::FactionSelection(float winWidth, float winHeight)
    : windowWidth(winWidth), windowHeight(winHeight), selectedFaction(Faction::None),
    ententeSprite(nullptr), throngSprite(nullptr) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for faction selection!" << endl;
    }

    // Load background images
    if (!ententeTexture.loadFromFile("Assets/Entente.jpg")) {
        cerr << "Error loading Entente background image!" << endl;
    }
    if (!throngTexture.loadFromFile("Assets/Throng.jpg")) {
        cerr << "Error loading Throng background image!" << endl;
    }

    ententeTexture.setSmooth(true);
    throngTexture.setSmooth(true);

    // Setup Entente sprite (left half - 960x1080)
    ententeSprite = new Sprite(ententeTexture);
    ententeSprite->setPosition({ 0.f, 0.f });

    // Scale to fit 960x1080
    float ententeScaleX = 960.f / ententeTexture.getSize().x;
    float ententeScaleY = 1080.f / ententeTexture.getSize().y;
    ententeSprite->setScale({ ententeScaleX, ententeScaleY });

    // Setup Throng sprite (right half - 960x1080)
    throngSprite = new Sprite(throngTexture);
    throngSprite->setPosition({ 960.f, 0.f });

    // Scale to fit 960x1080
    float throngScaleX = 960.f / throngTexture.getSize().x;
    float throngScaleY = 1080.f / throngTexture.getSize().y;
    throngSprite->setScale({ throngScaleX, throngScaleY });

    // Setup hover overlays (for fade effect)
    ententeOverlay.setSize({ 960.f, 1080.f });
    ententeOverlay.setPosition({ 0.f, 0.f });
    ententeOverlay.setFillColor(Color(0, 0, 0, 0)); // Start transparent

    throngOverlay.setSize({ 960.f, 1080.f });
    throngOverlay.setPosition({ 960.f, 0.f });
    throngOverlay.setFillColor(Color(0, 0, 0, 0)); // Start transparent

    // Title text - centered at top
    titleText.setString("Choose Your Faction");
    titleText.setCharacterSize(70);
    titleText.setFillColor(Color::White);
    titleText.setStyle(Text::Bold);
    titleText.setOutlineThickness(3.f);
    titleText.setOutlineColor(Color::Black);
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
    titleText.setPosition({ windowWidth / 2.f, 80.f });

    // ENTENTE - Left side text (centered)
    ententeTitle.setString("ENTENTE");
    ententeTitle.setCharacterSize(60);
    ententeTitle.setFillColor(Color::White);
    ententeTitle.setStyle(Text::Bold);
    ententeTitle.setOutlineThickness(3.f);
    ententeTitle.setOutlineColor(Color::Black);
    FloatRect entTitleBounds = ententeTitle.getLocalBounds();
    ententeTitle.setOrigin({ entTitleBounds.size.x / 2.f, entTitleBounds.size.y / 2.f });
    ententeTitle.setPosition({ 480.f, windowHeight / 2.f - 100.f }); // Centered in left half

    ententeDescription.setString("The noble alliance\nof order and honor.\n\nFight for justice\nand protect the realm.");
    ententeDescription.setCharacterSize(30);
    ententeDescription.setFillColor(Color(240, 240, 240));
    ententeDescription.setStyle(Text::Bold);
    ententeDescription.setOutlineThickness(2.f);
    ententeDescription.setOutlineColor(Color::Black);
    FloatRect entDescBounds = ententeDescription.getLocalBounds();
    ententeDescription.setOrigin({ entDescBounds.size.x / 2.f, 0.f });
    ententeDescription.setPosition({ 480.f, windowHeight / 2.f }); // Centered in left half

    // THRONG - Right side text (centered)
    throngTitle.setString("THRONG");
    throngTitle.setCharacterSize(60);
    throngTitle.setFillColor(Color::White);
    throngTitle.setStyle(Text::Bold);
    throngTitle.setOutlineThickness(3.f);
    throngTitle.setOutlineColor(Color::Black);
    FloatRect thrTitleBounds = throngTitle.getLocalBounds();
    throngTitle.setOrigin({ thrTitleBounds.size.x / 2.f, thrTitleBounds.size.y / 2.f });
    throngTitle.setPosition({ 1440.f, windowHeight / 2.f - 100.f }); // Centered in right half (960 + 480)

    throngDescription.setString("The fierce horde\nof chaos and power.\n\nConquer with strength\nand claim your destiny.");
    throngDescription.setCharacterSize(30);
    throngDescription.setFillColor(Color(240, 240, 240));
    throngDescription.setStyle(Text::Bold);
    throngDescription.setOutlineThickness(2.f);
    throngDescription.setOutlineColor(Color::Black);
    FloatRect thrDescBounds = throngDescription.getLocalBounds();
    throngDescription.setOrigin({ thrDescBounds.size.x / 2.f, 0.f });
    throngDescription.setPosition({ 1440.f, windowHeight / 2.f }); // Centered in right half
}

FactionSelection::~FactionSelection() {
    delete ententeSprite;
    delete throngSprite;
}

void FactionSelection::update(float deltaTime) {
    // Fade in Entente text when hovering
    if (isHoveringEntente) {
        ententeTextAlpha += fadeSpeed * deltaTime;
        if (ententeTextAlpha > 255.f) ententeTextAlpha = 255.f;
    }
    else {
        ententeTextAlpha -= fadeSpeed * deltaTime;
        if (ententeTextAlpha < 0.f) ententeTextAlpha = 0.f;
    }

    // Fade in Throng text when hovering
    if (isHoveringThrong) {
        throngTextAlpha += fadeSpeed * deltaTime;
        if (throngTextAlpha > 255.f) throngTextAlpha = 255.f;
    }
    else {
        throngTextAlpha -= fadeSpeed * deltaTime;
        if (throngTextAlpha < 0.f) throngTextAlpha = 0.f;
    }
}

void FactionSelection::draw(RenderWindow& window) {
    // Draw background images
    if (ententeSprite) window.draw(*ententeSprite);
    if (throngSprite) window.draw(*throngSprite);

    // Draw hover overlays (fade effect)
    window.draw(ententeOverlay);
    window.draw(throngOverlay);

    // Draw title
    window.draw(titleText);

    // Draw Entente text with fade
    if (ententeTextAlpha > 0.f) {
        unsigned char alpha = static_cast<unsigned char>(ententeTextAlpha);

        Color titleColor = ententeTitle.getFillColor();
        titleColor.a = alpha;
        ententeTitle.setFillColor(titleColor);

        Color descColor = ententeDescription.getFillColor();
        descColor.a = alpha;
        ententeDescription.setFillColor(descColor);

        Color titleOutline = ententeTitle.getOutlineColor();
        titleOutline.a = alpha;
        ententeTitle.setOutlineColor(titleOutline);

        Color descOutline = ententeDescription.getOutlineColor();
        descOutline.a = alpha;
        ententeDescription.setOutlineColor(descOutline);

        window.draw(ententeTitle);
        window.draw(ententeDescription);
    }

    // Draw Throng text with fade
    if (throngTextAlpha > 0.f) {
        unsigned char alpha = static_cast<unsigned char>(throngTextAlpha);

        Color titleColor = throngTitle.getFillColor();
        titleColor.a = alpha;
        throngTitle.setFillColor(titleColor);

        Color descColor = throngDescription.getFillColor();
        descColor.a = alpha;
        throngDescription.setFillColor(descColor);

        Color titleOutline = throngTitle.getOutlineColor();
        titleOutline.a = alpha;
        throngTitle.setOutlineColor(titleOutline);

        Color descOutline = throngDescription.getOutlineColor();
        descOutline.a = alpha;
        throngDescription.setOutlineColor(descOutline);

        window.draw(throngTitle);
        window.draw(throngDescription);
    }
}

Faction FactionSelection::checkClick(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Check if clicking on left half (Entente)
    if (mousePosF.x >= 0.f && mousePosF.x < 960.f &&
        mousePosF.y >= 0.f && mousePosF.y < 1080.f) {
        selectedFaction = Faction::Entente;
        return Faction::Entente;
    }

    // Check if clicking on right half (Throng)
    if (mousePosF.x >= 960.f && mousePosF.x < 1920.f &&
        mousePosF.y >= 0.f && mousePosF.y < 1080.f) {
        selectedFaction = Faction::Throng;
        return Faction::Throng;
    }

    return Faction::None;
}

void FactionSelection::checkHover(const RenderWindow& window) {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Reset hover states
    isHoveringEntente = false;
    isHoveringThrong = false;

    // Check hover on left half (Entente)
    if (mousePosF.x >= 0.f && mousePosF.x < 960.f &&
        mousePosF.y >= 0.f && mousePosF.y < 1080.f) {
        isHoveringEntente = true;
        ententeOverlay.setFillColor(Color(0, 0, 0, 150)); // Stronger darken/fade effect
    }
    else {
        ententeOverlay.setFillColor(Color(0, 0, 0, 0)); // Transparent
    }

    // Check hover on right half (Throng)
    if (mousePosF.x >= 960.f && mousePosF.x < 1920.f &&
        mousePosF.y >= 0.f && mousePosF.y < 1080.f) {
        isHoveringThrong = true;
        throngOverlay.setFillColor(Color(0, 0, 0, 150)); // Stronger darken/fade effect
    }
    else {
        throngOverlay.setFillColor(Color(0, 0, 0, 0)); // Transparent
    }
}