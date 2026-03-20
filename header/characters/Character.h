#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;
using namespace sf;

class Character {
public:
    Character(const string& texturePath, float width, float height, float posX, float posY);
    ~Character();

    void draw(RenderWindow& window);
    bool isClicked(const RenderWindow& window) const;
    void setPosition(float x, float y);

private:
    Texture texture;
    Sprite sprite{ texture }; 
};