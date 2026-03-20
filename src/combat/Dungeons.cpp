#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include "menu.h"
#include "Character.h"
#include "QuestDialog.h"
#include "NPCData.h"

using namespace std;
using namespace sf;

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    RenderWindow window(VideoMode::getDesktopMode(), "Entente and Throng", State::Fullscreen);

    Texture background;
    if (!background.loadFromFile("background/bar1.jpg"))
    {
        cout << "Error loading background!" << endl;
        return 1;
    }
    background.setSmooth(true);
    Sprite sprite(background);
    sprite.setOrigin({ background.getSize().x / 2.f,
        background.getSize().y / 2.f }
    );

    float targetW = 1600.f;
    float targetH = 1080.f;

    float scaleX = targetW / background.getSize().x;
    float scaleY = targetH / background.getSize().y;
    sprite.setScale({ scaleX, scaleY });

    Vector2u win = window.getSize();

    sprite.setPosition({
        static_cast<float>(win.x) - targetW / 2.f,
        static_cast<float>(win.y) / 2.f
        });

    // Get all NPCs from database
    vector<NPCData> allNPCs = NPCDatabase::getAllNPCs();

    // Pick a random NPC
    int randomNPCIndex = rand() % allNPCs.size();
    NPCData selectedNPC = allNPCs[randomNPCIndex];

    // Pick initial random quest
    int currentQuestIndex = rand() % selectedNPC.quests.size();
    string currentQuest = selectedNPC.quests[currentQuestIndex];

    // Fixed position for NPC
    float npcX = static_cast<float>(win.x) / 2.f + 400.f;
    float npcY = static_cast<float>(win.y) / 2.f + 50.f;

    // Fixed size for NPC
    float npcWidth = 400.f;
    float npcHeight = 500.f;

    // Create the character
    Character npc(selectedNPC.texturePath, npcWidth, npcHeight, npcX, npcY);

    QuestDialog questDialog(static_cast<float>(win.x), static_cast<float>(win.y));

    Menu menu(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {

            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (event->is<Event::MouseMoved>()) {
                menu.checkMousePosition(window);
            }

            if (const auto* mouseButton = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseButton->button == Mouse::Button::Left) {

                    if (questDialog.isAcceptButtonClicked(window)) {
                        cout << "Quest Accepted from " << selectedNPC.name << "!" << endl;
                        questDialog.hide();

                        // NOW pick a new random quest after accepting
                        currentQuestIndex = rand() % selectedNPC.quests.size();
                        currentQuest = selectedNPC.quests[currentQuestIndex];
                    }
                    else if (npc.isClicked(window) && !questDialog.isVisible()) {
                        // Show the current quest (doesn't change until accepted)
                        cout << selectedNPC.name << " clicked! Showing quest: " << currentQuestIndex << endl;
                        questDialog.show(selectedNPC.name, currentQuest);
                    }
                    else if (questDialog.isVisible()) {
                        // Check if clicked outside dialog - close it
                        Vector2i mousePos = Mouse::getPosition(window);
                        FloatRect dialogBounds = questDialog.getDialogBounds();

                        if (!dialogBounds.contains({ static_cast<float>(mousePos.x),
                            static_cast<float>(mousePos.y) })) {
                            cout << "Clicked outside dialog - closing" << endl;
                            questDialog.hide();
                        }
                    }
                    else if (!questDialog.isVisible()) {
                        int selected = menu.getClickedItemIndex(window);
                        if (selected >= 0) {
                            if (selected == 0) cout << "Dungeons Button has been pressed" << endl;
                            if (selected == 1) cout << "Arena Button has been pressed" << endl;
                            if (selected == 2) cout << "Shop Button has been pressed" << endl;
                            if (selected == 3) cout << "Brewer Button has been pressed" << endl;
                            if (selected == 4) cout << "Rider Button has been pressed" << endl;
                            if (selected == 5) cout << "Fortress Button has been pressed" << endl;
                            if (selected == 6) cout << "Curse Button has been pressed" << endl;
                            if (selected == 7) cout << "Conquerors Button has been pressed" << endl;
                            if (selected == 8) cout << "Guild Button has been pressed" << endl;
                        }
                    }
                }
            }
        }

        window.clear();
        window.draw(sprite);
        npc.draw(window);
        menu.draw(window);
        questDialog.draw(window);
        window.display();
    }

    return 0;
}