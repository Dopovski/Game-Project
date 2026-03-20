#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace sf;

#define MAX_ITEMS 9

struct MenuItem {
    Text text;
    RectangleShape buttonRect;
};

class Menu {
public:
    Menu(float width, float height);
    ~Menu();
    void draw(RenderWindow& window);
    void checkMousePosition(const RenderWindow& window);
    int getClickedItemIndex(const RenderWindow& window) const;
private:
    int selectedItemIndex;
    Font font;
    vector<MenuItem> menuItems;
};