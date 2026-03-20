#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;
using namespace sf;

class QuestDialog {
public:
    QuestDialog(float windowWidth, float windowHeight);
    ~QuestDialog();

    void show(const string& npcName, const string& questText);
    void hide();
    void draw(RenderWindow& window);
    bool isAcceptButtonClicked(const RenderWindow& window) const;
    bool isVisible() const { return visible; }
    FloatRect getDialogBounds() const { return dialogBox.getGlobalBounds(); }

private:
    string wrapText(const string& text, float maxWidth);

    bool visible;
    Font font;

    RectangleShape dialogBox;
    Text npcNameText{ font };
    Text questText{ font };
    RectangleShape acceptButton;
    Text acceptButtonText{ font };

    float windowWidth;
    float windowHeight;
};