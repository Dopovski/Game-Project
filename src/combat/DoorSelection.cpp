#include "DoorSelection.h"
#include <iostream>
#include <algorithm>
#include <random>

DoorSelection::DoorSelection(float winWidth, float winHeight,
    Menu* menu, CharacterSheet* characterSheet,
    int silver, int gold, int torches)
    : windowWidth(winWidth), windowHeight(winHeight),
    menu(menu), characterSheet(characterSheet),
    currentTorches(torches), showingConfirmation(false),
    selectedDoorIndex(-1), confirmationTimer(0.0f) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for door selection!" << endl;
    }

    // Title
    titleText.setString("Choose Your Path");
    titleText.setCharacterSize(60);
    titleText.setFillColor(Color::White);
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
    titleText.setPosition({ windowWidth / 2.f, 100.f });

    // Create 3 doors (Red, Green, Blue)
    float doorWidth = 250.f;
    float doorHeight = 400.f;
    float spacing = 100.f;
    float totalWidth = (doorWidth * 3) + (spacing * 2);
    float startX = (windowWidth - totalWidth) / 2.f;
    float doorY = windowHeight / 2.f - doorHeight / 2.f;

    Color doorColors[] = {
        Color(200, 50, 50),   // Red
        Color(50, 200, 50),   // Green
        Color(50, 50, 200)    // Blue
    };

    string doorNames[] = { "Red Door", "Green Door", "Blue Door" };

    // Assign random paths to doors
    assignRandomPaths();

    for (int i = 0; i < 3; ++i) {
        // Create door
        RectangleShape door;
        door.setSize({ doorWidth, doorHeight });
        door.setFillColor(doorColors[i]);
        door.setOutlineThickness(5.f);
        door.setOutlineColor(Color::White);
        door.setPosition({ startX + (doorWidth + spacing) * i, doorY });
        doors.push_back(door);

        // Create door label
        Text label(font);
        label.setString(doorNames[i]);
        label.setCharacterSize(32);
        label.setFillColor(Color::White);
        FloatRect labelBounds = label.getLocalBounds();
        label.setOrigin({ labelBounds.size.x / 2.f, labelBounds.size.y / 2.f });
        label.setPosition({
            startX + (doorWidth + spacing) * i + doorWidth / 2.f,
            doorY + doorHeight + 50.f
            });
        doorLabels.push_back(label);

        // Create torch button above each door
        RectangleShape torchButton;
        torchButton.setSize({ 120.f, 50.f });
        torchButton.setFillColor(Color(255, 140, 0)); // Orange for torch
        torchButton.setOutlineThickness(3.f);
        torchButton.setOutlineColor(Color::White);
        torchButton.setPosition({
            startX + (doorWidth + spacing) * i + doorWidth / 2.f - 60.f,
            doorY - 80.f
            });
        torchButtons.push_back(torchButton);

        // Create torch button text
        Text torchText(font);
        torchText.setString("Torch");
        torchText.setCharacterSize(24);
        torchText.setFillColor(Color::White);
        FloatRect torchBounds = torchText.getLocalBounds();
        torchText.setOrigin({ torchBounds.size.x / 2.f, torchBounds.size.y / 2.f });
        torchText.setPosition({
            startX + (doorWidth + spacing) * i + doorWidth / 2.f,
            doorY - 55.f
            });
        torchButtonTexts.push_back(torchText);

        // Create door info text (initially hidden)
        Text infoText(font);
        infoText.setString("");
        infoText.setCharacterSize(20);
        infoText.setFillColor(Color::Yellow);
        infoText.setPosition({
            startX + (doorWidth + spacing) * i + 10.f,
            doorY + doorHeight / 2.f - 10.f
            });
        doorInfoTexts.push_back(infoText);

        // Initialize revealed state
        doorRevealed.push_back(false);
    }

    // Setup character icon button
    characterIconButton.setRadius(40.f);
    characterIconButton.setFillColor(Color(100, 100, 200));
    characterIconButton.setOutlineThickness(3.f);
    characterIconButton.setOutlineColor(Color::White);
    characterIconButton.setPosition({ 120.f, windowHeight - 100.f });

    characterIconText.setString("C");
    characterIconText.setCharacterSize(35);
    characterIconText.setFillColor(Color::White);
    FloatRect iconTextBounds = characterIconText.getLocalBounds();
    characterIconText.setOrigin({ iconTextBounds.size.x / 2.f, iconTextBounds.size.y / 2.f });
    characterIconText.setPosition({ 160.f, windowHeight - 65.f });

    // Setup currency display
    silverDisplayText.setString("Silver: " + to_string(silver));
    silverDisplayText.setCharacterSize(24);
    silverDisplayText.setFillColor(Color(192, 192, 192));
    silverDisplayText.setPosition({ 20.f, 20.f });

    goldDisplayText.setString("Gold: " + to_string(gold));
    goldDisplayText.setCharacterSize(24);
    goldDisplayText.setFillColor(Color(255, 215, 0));
    goldDisplayText.setPosition({ 180.f, 20.f });

    torchesDisplayText.setString("Torches: " + to_string(torches));
    torchesDisplayText.setCharacterSize(24);
    torchesDisplayText.setFillColor(Color(255, 140, 0));
    torchesDisplayText.setPosition({ 20.f, 50.f });

    // Setup confirmation screen
    confirmationBackground.setSize({ windowWidth, windowHeight });
    confirmationBackground.setFillColor(Color(0, 0, 0, 180));

    confirmationBox.setSize({ 600.f, 300.f });
    confirmationBox.setFillColor(Color(40, 40, 60));
    confirmationBox.setOutlineThickness(5.f);
    confirmationBox.setOutlineColor(Color::White);
    confirmationBox.setPosition({ windowWidth / 2.f - 300.f, windowHeight / 2.f - 150.f });

    confirmationTitle.setCharacterSize(48);
    confirmationTitle.setFillColor(Color::Yellow);

    confirmationMessage.setCharacterSize(32);
    confirmationMessage.setFillColor(Color::White);
}

DoorSelection::~DoorSelection() {
}

void DoorSelection::assignRandomPaths() {
    // Create a vector with one of each path type
    doorPaths = { DoorPath::Normal, DoorPath::Ambushed, DoorPath::Ambush };

    // Shuffle to randomize which door gets which path
    random_device rd;
    mt19937 gen(rd());
    shuffle(doorPaths.begin(), doorPaths.end(), gen);
}

string DoorSelection::getPathDescription(DoorPath path) const {
    switch (path) {
    case DoorPath::Normal:
        return "Normal Fight";
    case DoorPath::Ambushed:
        return "You'll be\nAmbushed!\nLose 5% Silver";
    case DoorPath::Ambush:
        return "Ambush the\nMonster!\n+5% Damage";
    default:
        return "Unknown";
    }
}

string DoorSelection::getPathTitle(DoorPath path) const {
    switch (path) {
    case DoorPath::Normal:
        return "Normal Fight!";
    case DoorPath::Ambushed:
        return "Ambushed!";
    case DoorPath::Ambush:
        return "Ambush!";
    default:
        return "Unknown";
    }
}

void DoorSelection::draw(RenderWindow& window) {
    // Draw menu
    if (menu) {
        menu->draw(window);
    }

    // Draw title
    window.draw(titleText);

    // Draw doors
    for (int i = 0; i < doors.size(); ++i) {
        window.draw(doors[i]);
        window.draw(doorLabels[i]);

        // Draw torch buttons
        window.draw(torchButtons[i]);
        window.draw(torchButtonTexts[i]);

        // Draw door info if revealed
        if (doorRevealed[i]) {
            window.draw(doorInfoTexts[i]);
        }
    }

    // Draw currency
    window.draw(silverDisplayText);
    window.draw(goldDisplayText);
    window.draw(torchesDisplayText);

    // Draw character button
    window.draw(characterIconButton);
    window.draw(characterIconText);

    // Draw character sheet if visible
    if (characterSheet) {
        characterSheet->draw(window);
    }

    // Draw confirmation screen on top of everything if showing
    if (showingConfirmation) {
        window.draw(confirmationBackground);
        window.draw(confirmationBox);
        window.draw(confirmationTitle);
        window.draw(confirmationMessage);
    }
}

int DoorSelection::checkDoorClick(const RenderWindow& window) const {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    for (int i = 0; i < doors.size(); ++i) {
        if (doors[i].getGlobalBounds().contains(mousePosF)) {
            return i;
        }
    }

    return -1;
}

int DoorSelection::checkTorchButtonClick(const RenderWindow& window) const {
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    for (int i = 0; i < torchButtons.size(); ++i) {
        if (torchButtons[i].getGlobalBounds().contains(mousePosF)) {
            return i;
        }
    }

    return -1;
}

void DoorSelection::useTorch(int doorIndex) {
    if (doorIndex >= 0 && doorIndex < 3 && !doorRevealed[doorIndex] && currentTorches > 0) {
        doorRevealed[doorIndex] = true;
        currentTorches--;

        // Update the door info text to show what this door contains
        doorInfoTexts[doorIndex].setString(getPathDescription(doorPaths[doorIndex]));

        // Center the text on the door
        FloatRect textBounds = doorInfoTexts[doorIndex].getLocalBounds();
        Vector2f doorPos = doors[doorIndex].getPosition();
        Vector2f doorSize = doors[doorIndex].getSize();

        doorInfoTexts[doorIndex].setPosition({
            doorPos.x + doorSize.x / 2.f - textBounds.size.x / 2.f,
            doorPos.y + doorSize.y / 2.f - textBounds.size.y / 2.f
            });

        // Update torches display
        torchesDisplayText.setString("Torches: " + to_string(currentTorches));

        cout << "Torch used on door " << doorIndex << ". Remaining torches: " << currentTorches << endl;
    }
}

bool DoorSelection::isDoorRevealed(int doorIndex) const {
    if (doorIndex >= 0 && doorIndex < 3) {
        return doorRevealed[doorIndex];
    }
    return false;
}

DoorPath DoorSelection::getDoorPath(int doorIndex) const {
    if (doorIndex >= 0 && doorIndex < 3) {
        return doorPaths[doorIndex];
    }
    return DoorPath::Normal;
}

void DoorSelection::updateTorchDisplay(int torches) {
    currentTorches = torches;
    torchesDisplayText.setString("Torches: " + to_string(currentTorches));
}

void DoorSelection::showConfirmation(int doorIndex) {
    showingConfirmation = true;
    selectedDoorIndex = doorIndex;
    confirmationTimer = 1.5f; // 1.5 seconds

    cout << "Showing confirmation screen! Timer set to 1.5 seconds" << endl;

    DoorPath path = doorPaths[doorIndex];

    // Set title based on path
    confirmationTitle.setString(getPathTitle(path));
    FloatRect titleBounds = confirmationTitle.getLocalBounds();
    confirmationTitle.setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
    confirmationTitle.setPosition({ windowWidth / 2.f, windowHeight / 2.f - 80.f });

    // Set message based on path
    string message;
    if (path == DoorPath::Normal) {
        message = "A normal battle awaits.";
    }
    else if (path == DoorPath::Ambushed) {
        message = "The enemy was waiting!\nYou lose 5% of your silver.";
    }
    else if (path == DoorPath::Ambush) {
        message = "You caught them off guard!\nYou'll deal 5% more damage.";
    }

    cout << "Confirmation title: " << getPathTitle(path) << endl;
    cout << "Confirmation message: " << message << endl;

    confirmationMessage.setString(message);
    FloatRect messageBounds = confirmationMessage.getLocalBounds();
    confirmationMessage.setOrigin({ messageBounds.size.x / 2.f, messageBounds.size.y / 2.f });
    confirmationMessage.setPosition({ windowWidth / 2.f, windowHeight / 2.f + 20.f });
}

void DoorSelection::update(float deltaTime) {
    if (showingConfirmation && confirmationTimer > 0.0f) {
        confirmationTimer -= deltaTime;
        cout << "Confirmation timer: " << confirmationTimer << " seconds remaining" << endl;

        if (confirmationTimer <= 0.0f) {
            cout << "Timer finished! Ready to proceed to combat." << endl;
        }
    }
}