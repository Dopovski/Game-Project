#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Faction.h"

class CharacterSheet;
class Menu;
class Character;
class QuestDialog;
class Mission;
class MissionScreen;
class DoorSelection;
class CombatScene;
class ArenaScreen;
class Shop;
class FatigueSystem;
class FatiguedDialog;
class BrewerSystem;
class PotionInventory;
class FactionSelection;
class CharacterSelection;
class ClassSelection;

enum class GameState {
    FactionSelection,
    RaceSelection,
    ClassSelection,
    NameInput,
    MainGame,
    OnMission,
    DoorChoice,
    Combat,
    CurseSelection
};


enum class MountType {
    None,
    Horse,
    Griffin
};

struct MountData {
    MountType type;
    int cost;
    float speedBonus;
    int durationDays;
    int remainingDays;
};

struct NPCQuestState {
    std::string npc1CurrentQuest;
    std::string npc2CurrentQuest;
    std::string npc3CurrentQuest;
    int selectedNPCIndex;
};

class GameStateManager {
private:
    GameState currentState;
    GameState previousState;

    std::string playerRaceName;
    int playerRaceIndex;
    int playerClassIndex;
    Faction playerFaction;

    NPCQuestState questState;

    bool inArenaCombat;

    MountData currentMount;

public:
    GameStateManager()
        : currentState(GameState::FactionSelection),
        previousState(GameState::FactionSelection),
        playerRaceIndex(-1),
        playerClassIndex(-1),
        playerFaction(Faction::None),
        inArenaCombat(false) {
        questState.selectedNPCIndex = -1;

        currentMount.type = MountType::None;
        currentMount.cost = 0;
        currentMount.speedBonus = 0.0f;
        currentMount.durationDays = 0;
        currentMount.remainingDays = 0;
    }

    GameState getCurrentState() const { return currentState; }
    GameState getPreviousState() const { return previousState; }

    void setState(GameState newState) {
        previousState = currentState;
        currentState = newState;
    }

    const std::string& getPlayerRaceName() const { return playerRaceName; }
    void setPlayerRaceName(const std::string& name) { playerRaceName = name; }

    int getPlayerRaceIndex() const { return playerRaceIndex; }
    void setPlayerRaceIndex(int index) { playerRaceIndex = index; }

    int getPlayerClassIndex() const { return playerClassIndex; }
    void setPlayerClassIndex(int index) { playerClassIndex = index; }

    Faction getPlayerFaction() const { return playerFaction; }
    void setPlayerFaction(Faction faction) { playerFaction = faction; }

    NPCQuestState& getQuestState() { return questState; }
    const NPCQuestState& getQuestState() const { return questState; }

    bool isInArenaCombat() const { return inArenaCombat; }
    void setInArenaCombat(bool value) { inArenaCombat = value; }

    MountData& getCurrentMount() { return currentMount; }
    const MountData& getCurrentMount() const { return currentMount; }

    void setCurrentMount(const MountData& mount) { currentMount = mount; }

    float getMountSpeedBonus() const {
        if (currentMount.remainingDays > 0) {
            return currentMount.speedBonus;
        }
        return 0.0f;
    }

    void decrementMountDuration() {
        if (currentMount.remainingDays > 0) {
            currentMount.remainingDays--;
            if (currentMount.remainingDays <= 0) {
                currentMount.type = MountType::None;
                currentMount.speedBonus = 0.0f;
            }
        }
    }
};