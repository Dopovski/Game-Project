#include "Character.h"

Character::Character(const string& texturePath, float width, float height, float posX, float posY) {
    if (!texture.loadFromFile(texturePath)) {
        cerr << "Error loading character texture: " << texturePath << endl;
    }
    texture.setSmooth(true);

    sprite = Sprite(texture);
    sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f });

    float scaleX = width / texture.getSize().x;
    float scaleY = height / texture.getSize().y;
    sprite.setScale({ scaleX, scaleY });

    sprite.setPosition({ posX, posY });
}

Character::~Character() {
}

void Character::draw(RenderWindow& window) {
    window.draw(sprite);
}

bool Character::isClicked(const RenderWindow& window) const {
    Vector2i mousePos = Mouse::getPosition(window);
    FloatRect bounds = sprite.getGlobalBounds();

    return bounds.contains({ static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y) });
}

void Character::setPosition(float x, float y) {
    sprite.setPosition({ x, y });
}