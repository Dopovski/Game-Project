#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

using namespace sf;

class ResourceManager {
private:
    Texture backgroundTexture;
    Sprite* backgroundSprite;

    float windowWidth;
    float windowHeight;

public:
    ResourceManager(float winWidth, float winHeight)
        : backgroundSprite(nullptr),
        windowWidth(winWidth),
        windowHeight(winHeight) {
    }

    ~ResourceManager() {
        delete backgroundSprite;
    }

    bool loadBackground(const std::string& filepath) {
        if (!backgroundTexture.loadFromFile(filepath)) {
            std::cerr << "Error loading background: " << filepath << std::endl;
            return false;
        }

        backgroundTexture.setSmooth(true);
        backgroundSprite = new Sprite(backgroundTexture);

        // Set target size to 1600x1080
        float targetW = 1600.f;
        float targetH = 1080.f;
        float scaleX = targetW / backgroundTexture.getSize().x;
        float scaleY = targetH / backgroundTexture.getSize().y;
        backgroundSprite->setScale({ scaleX, scaleY });

        // Align to right side of screen
        float bgX = windowWidth - targetW;
        float bgY = (windowHeight - targetH) / 2.f; // Center vertically
        backgroundSprite->setPosition({ bgX, bgY });

        return true;
    }

    void drawBackground(RenderWindow& window) const {
        if (backgroundSprite) {
            window.draw(*backgroundSprite);
        }
    }

    Sprite* getBackgroundSprite() { return backgroundSprite; }
};