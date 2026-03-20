#include "QuestDialog.h"

QuestDialog::QuestDialog(float winWidth, float winHeight)
    : visible(false), windowWidth(winWidth), windowHeight(winHeight) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for quest dialog!" << endl;
    }

    // Create dialog box (centered, 800x400)
    float dialogWidth = 800.f;
    float dialogHeight = 400.f;
    dialogBox.setSize({ dialogWidth, dialogHeight });
    dialogBox.setFillColor(Color(20, 20, 20, 230));
    dialogBox.setOutlineThickness(5.f);
    dialogBox.setOutlineColor(Color(200, 150, 50));
    dialogBox.setPosition({
        (windowWidth - dialogWidth) / 2.f,
        (windowHeight - dialogHeight) / 2.f
        });

    // Create NPC name text (top left of dialog)
    npcNameText.setCharacterSize(32);
    npcNameText.setFillColor(Color(255, 215, 0)); // Gold color
    npcNameText.setPosition({
        dialogBox.getPosition().x + 20.f,
        dialogBox.getPosition().y + 15.f
        });

    // Create quest text
    questText.setCharacterSize(22);
    questText.setFillColor(Color::White);
    questText.setPosition({
        dialogBox.getPosition().x + 50.f,
        dialogBox.getPosition().y + 80.f
        });

    // Create accept button
    float buttonWidth = 200.f;
    float buttonHeight = 60.f;
    acceptButton.setSize({ buttonWidth, buttonHeight });
    acceptButton.setFillColor(Color(50, 150, 50));
    acceptButton.setOutlineThickness(3.f);
    acceptButton.setOutlineColor(Color::White);
    acceptButton.setPosition({
        dialogBox.getPosition().x + (dialogWidth - buttonWidth) / 2.f,
        dialogBox.getPosition().y + dialogHeight - buttonHeight - 30.f
        });

    // Create button text
    acceptButtonText.setString("Accept Quest");
    acceptButtonText.setCharacterSize(28);
    acceptButtonText.setFillColor(Color::White);

    FloatRect textRect = acceptButtonText.getLocalBounds();
    acceptButtonText.setOrigin({
        textRect.size.x / 2.f,
        textRect.size.y / 2.f
        });
    acceptButtonText.setPosition({
        acceptButton.getPosition().x + buttonWidth / 2.f,
        acceptButton.getPosition().y + buttonHeight / 2.f
        });
}

QuestDialog::~QuestDialog() {
}

void QuestDialog::show(const string& npcName, const string& text) {
    visible = true;
    npcNameText.setString(npcName);

    // Wrap text to fit in dialog box (700 pixels max width)
    string wrappedText = wrapText(text, 700.f);
    questText.setString(wrappedText);
}

void QuestDialog::hide() {
    visible = false;
}

string QuestDialog::wrapText(const string& text, float maxWidth) {
    string result = "";
    string currentLine = "";

    Text tempText(font);
    tempText.setCharacterSize(22);

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];

        // Handle newlines
        if (c == '\n') {
            result += currentLine + "\n";
            currentLine = "";
            continue;
        }

        currentLine += c;
        tempText.setString(currentLine);

        // Check if line is too wide
        if (tempText.getLocalBounds().size.x > maxWidth) {
            // Find last space to break at
            size_t lastSpace = currentLine.find_last_of(' ');
            if (lastSpace != string::npos && lastSpace > 0) {
                result += currentLine.substr(0, lastSpace) + "\n";
                currentLine = currentLine.substr(lastSpace + 1);
            }
            else {
                // No space found, force break
                result += currentLine + "\n";
                currentLine = "";
            }
        }
    }

    result += currentLine;
    return result;
}

void QuestDialog::draw(RenderWindow& window) {
    if (!visible) return;

    window.draw(dialogBox);
    window.draw(npcNameText);
    window.draw(questText);
    window.draw(acceptButton);
    window.draw(acceptButtonText);
}

bool QuestDialog::isAcceptButtonClicked(const RenderWindow& window) const {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    FloatRect buttonBounds = acceptButton.getGlobalBounds();

    return buttonBounds.contains({
        static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y)
        });
}