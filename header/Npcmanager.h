#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Character.h"
#include "NPCData.h"
#include "MissionRewardData.h"

class NPCManager {
private:
    Character* npc1;
    Character* npc2;
    Character* npc3;
    std::vector<NPCMissionData> allNPCs;

    // Quest tracking
    std::string npc1CurrentQuest;
    std::string npc2CurrentQuest;
    std::string npc3CurrentQuest;

    int selectedNPCIndex;
    NPCMissionData selectedNPC;

public:
    NPCManager()
        : npc1(nullptr), npc2(nullptr), npc3(nullptr),
        selectedNPCIndex(-1) {
    }

    ~NPCManager() {
        delete npc1;
        delete npc2;
        delete npc3;
    }

    void initializeNPCs(float windowX, float windowY) {
        // Get all NPCs from database
        allNPCs = NPCDatabase::getAllNPCs();

        // Calculate NPC positions - evenly spaced horizontally
        float startX = windowX / 2.f + 150.f;
        float spacing = 350.f;
        float npcY = windowY / 2.f + 50.f;

        // Create all 3 NPCs
        npc1 = new Character(allNPCs[0].texturePath, 350.f, 450.f, startX, npcY);
        npc2 = new Character(allNPCs[1].texturePath, 350.f, 450.f, startX + spacing, npcY);
        npc3 = new Character(allNPCs[2].texturePath, 350.f, 450.f, startX + spacing * 2, npcY);
    }

    void drawNPCs(sf::RenderWindow& window) const {
        if (npc1) npc1->draw(window);
        if (npc2) npc2->draw(window);
        if (npc3) npc3->draw(window);
    }

    // Check if any NPC was clicked, returns NPC index or -1
    int checkNPCClick(const sf::RenderWindow& window) const {
        if (npc1 && npc1->isClicked(window)) return 0;
        if (npc2 && npc2->isClicked(window)) return 1;
        if (npc3 && npc3->isClicked(window)) return 2;
        return -1;
    }

    // Get quest for NPC, generate if needed
    std::string getQuestForNPC(int npcIndex, int playerLevel) {
        if (npcIndex < 0 || npcIndex >= 3) return "";

        std::string* currentQuest = nullptr;
        if (npcIndex == 0) currentQuest = &npc1CurrentQuest;
        else if (npcIndex == 1) currentQuest = &npc2CurrentQuest;
        else if (npcIndex == 2) currentQuest = &npc3CurrentQuest;

        // Generate quest if empty
        if (currentQuest->empty()) {
            int monsterLevel = std::max(1, std::min(100, playerLevel));
            MonsterReward reward = MissionRewardDatabase::getReward(monsterLevel, allNPCs[npcIndex].missionType);
            *currentQuest = "Hunt the " + reward.monsterName + "!";
        }

        return *currentQuest;
    }

    // Generate new quest for NPC
    void generateNewQuestForNPC(int npcIndex, int playerLevel) {
        if (npcIndex < 0 || npcIndex >= 3) return;

        int nextMonsterLevel = std::max(1, std::min(100, playerLevel));
        MonsterReward nextReward = MissionRewardDatabase::getReward(nextMonsterLevel, allNPCs[npcIndex].missionType);
        std::string newQuest = "Hunt the " + nextReward.monsterName + "!";

        if (npcIndex == 0) npc1CurrentQuest = newQuest;
        else if (npcIndex == 1) npc2CurrentQuest = newQuest;
        else if (npcIndex == 2) npc3CurrentQuest = newQuest;
    }

    // Get quest with estimated rewards
    std::string getQuestWithRewards(int npcIndex, int playerLevel) {
        std::string quest = getQuestForNPC(npcIndex, playerLevel);

        if (npcIndex >= 0 && npcIndex < allNPCs.size()) {
            int previewLevel = std::max(1, std::min(100, playerLevel));
            int previewSilver = MissionRewardDatabase::getSilverReward(previewLevel, allNPCs[npcIndex].missionType);
            int previewXP = MissionRewardDatabase::getXPReward(previewLevel, allNPCs[npcIndex].missionType);

            return quest + "\n\nEstimated Rewards:\nSilver: ~" + std::to_string(previewSilver) +
                "\nXP: ~" + std::to_string(previewXP);
        }

        return quest;
    }

    // Selection management
    void selectNPC(int index) {
        if (index >= 0 && index < allNPCs.size()) {
            selectedNPCIndex = index;
            selectedNPC = allNPCs[index];
        }
    }

    void clearSelection() {
        selectedNPCIndex = -1;
    }

    int getSelectedNPCIndex() const { return selectedNPCIndex; }
    const NPCMissionData& getSelectedNPC() const { return selectedNPC; }
    const std::vector<NPCMissionData>& getAllNPCs() const { return allNPCs; }
};