#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

enum class Faction {
    None,
    Entente,
    Throng
};

class FactionSelection {
public:
    FactionSelection(float windowWidth, float windowHeight);
    ~FactionSelection();

    void draw(RenderWindow& window);
    void update(float deltaTime);
    Faction checkClick(const RenderWindow& window);
    void checkHover(const RenderWindow& window);
    bool isSelectionMade() const { return selectedFaction != Faction::None; }
    Faction getSelectedFaction() const { return selectedFaction; }

private:
    Font font;
    Faction selectedFaction;

    // Title
    Text titleText{ font };

    // Background textures and sprites
    Texture ententeTexture;
    Texture throngTexture;
    Sprite* ententeSprite;
    Sprite* throngSprite;

    // Hover overlays (for fade effect)
    RectangleShape ententeOverlay;
    RectangleShape throngOverlay;

    // Left side - Entente
    Text ententeTitle{ font };
    Text ententeDescription{ font };

    // Right side - Throng
    Text throngTitle{ font };
    Text throngDescription{ font };

    float windowWidth;
    float windowHeight;

    // Hover state
    bool isHoveringEntente = false;
    bool isHoveringThrong = false;

    // Text fade animation
    float ententeTextAlpha = 0.f;
    float throngTextAlpha = 0.f;
    const float fadeSpeed = 800.f; // Alpha units per second
};