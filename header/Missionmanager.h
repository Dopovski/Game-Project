#pragma once
#include <SFML/Graphics.hpp>
#include "Mission.h"
#include "MissionScreen.h"
#include "DoorSelection.h"
#include "CombatScene.h"

class MissionManager {
private:
    Mission* currentMission;
    MissionScreen* missionScreen;
    DoorSelection* doorSelection;
    CombatScene* combatScene;

    bool inArenaCombat;

public:
    MissionManager()
        : currentMission(nullptr), missionScreen(nullptr),
        doorSelection(nullptr), combatScene(nullptr),
        inArenaCombat(false) {
    }

    ~MissionManager() {
        delete currentMission;
        delete missionScreen;
        delete doorSelection;
        delete combatScene;
    }

    // Create and start a new mission
    void startMission(
        const NPCMissionData& npc,
        const std::string& monsterName,
        const std::string& monsterTexture,
        int monsterLevel,
        const std::string& playerRace,
        float windowWidth,
        float windowHeight,
        Menu* menu,
        CharacterSheet* characterSheet,
        int* silver,
        int* gold,
        int* torches,
        sf::CircleShape* characterIconButton,
        sf::Text* characterIconText)
    {
        // Create mission
        currentMission = new Mission(npc.missionType, npc.npcName, monsterName, monsterTexture, monsterLevel);
        currentMission->setPlayerRace(playerRace);
        currentMission->start();

        // Create mission screen
        missionScreen = new MissionScreen(
            windowWidth, windowHeight,
            currentMission,
            menu,
            characterSheet,
            silver, gold, torches,
            characterIconButton,
            characterIconText
        );
    }

    // Create door selection (when mission completes)
    void createDoorSelection(
        float windowWidth,
        float windowHeight,
        Menu* menu,
        CharacterSheet* characterSheet,
        int silver,
        int gold,
        int torches)
    {
        doorSelection = new DoorSelection(
            windowWidth, windowHeight,
            menu, characterSheet,
            silver, gold, torches
        );

        // Clean up mission screen
        delete missionScreen;
        missionScreen = nullptr;
    }

    // Create combat scene (from door selection or arena)
    void createCombat(
        float windowWidth,
        float windowHeight,
        CharacterSheet* characterSheet,
        const std::string& monsterTexture,
        const std::string& monsterName,
        int monsterLevel,
        Menu* menu,
        int silver,
        int gold,
        int torches,
        double damageModifier,
        bool isArenaCombat,
        const std::string& playerRace)
    {
        combatScene = new CombatScene(
            windowWidth, windowHeight,
            characterSheet,
            monsterTexture,
            monsterName,
            monsterLevel,
            menu,
            silver, gold, torches,
            damageModifier,
            isArenaCombat,
            playerRace
        );

        if (isArenaCombat) {
            inArenaCombat = true;
        }
    }

    // Clean up combat
    void cleanupCombat() {
        delete combatScene;
        combatScene = nullptr;
        inArenaCombat = false;
    }

    // Clean up door selection
    void cleanupDoorSelection() {
        delete doorSelection;
        doorSelection = nullptr;
    }

    // Clean up mission
    void cleanupMission() {
        delete currentMission;
        currentMission = nullptr;
    }

    // Update mission screen
    void updateMissionScreen() {
        if (missionScreen) {
            missionScreen->update();
        }
    }

    // Update door selection
    void updateDoorSelection(float deltaTime) {
        if (doorSelection) {
            doorSelection->update(deltaTime);
        }
    }

    // Update combat
    void updateCombat(float deltaTime) {
        if (combatScene) {
            combatScene->update(deltaTime);
        }
    }

    // Getters
    Mission* getCurrentMission() { return currentMission; }
    MissionScreen* getMissionScreen() { return missionScreen; }
    DoorSelection* getDoorSelection() { return doorSelection; }
    CombatScene* getCombatScene() { return combatScene; }

    bool isInArenaCombat() const { return inArenaCombat; }
    void setInArenaCombat(bool value) { inArenaCombat = value; }

    bool isMissionComplete() const {
        return currentMission && currentMission->isComplete();
    }

    bool isDoorConfirmationFinished() const {
        return doorSelection && doorSelection->isConfirmationFinished();
    }
};