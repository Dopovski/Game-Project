#pragma once
#include <string>
#include <vector>
#include "Mission.h"
#include "MissionRewardData.h"

using namespace std;

struct NPCMissionData {
    string npcName;
    string texturePath;
    MissionType missionType;
    int minLevel;
    int maxLevel;
    string monsterTexture;  // Default monster texture for this NPC
};

class NPCDatabase {
public:
    // Get monster name for player's level
    static string getMonsterForLevel(int playerLevel) {
        // Monster Level = Player Level (PDF rule)
        int monsterLevel = playerLevel;

        // Clamp between 1-100
        if (monsterLevel < 1) monsterLevel = 1;
        if (monsterLevel > 100) monsterLevel = 100;

        // Get monster name from reward database
        MonsterReward reward = MissionRewardDatabase::getReward(monsterLevel, MissionType::Short);
        return reward.monsterName;
    }

    // Get all 3 NPCs (always available at any level)
    static vector<NPCMissionData> getAllNPCs() {
        return {
            // SCOUT - SHORT MISSIONS (Fast XP, active play)
            // Available for ALL levels 1-100
            {
                "Scout Veron",
                "Assets/NPC_Veron1.png",
                MissionType::Short,
                1,    // minLevel
                100,  // maxLevel - all levels!
                "Assets/NPC_Veron1.png"  // monster texture
            },

            // CAPTAIN - MEDIUM MISSIONS (Balanced)
            // Available for ALL levels 1-100
            {
                "Captain Gorn",
                "Assets/NPC_Gorn.jpg",
                MissionType::Medium,
                1,    // minLevel
                100,  // maxLevel - all levels!
                "Assets/NPC_Gorn.jpg"  // monster texture
            },

            // COMMANDER - LONG MISSIONS (Best rewards, idle play)
            // Available for ALL levels 1-100
            {
                "Commander Orianna",
                "Assets/NPC_Orianna.jpg",
                MissionType::Long,
                1,    // minLevel
                100,  // maxLevel - all levels!
                "Assets/NPC_Orianna.jpg"  // monster texture
            }
        };
    }

    // Get NPC appropriate for player level (returns all 3)
    static vector<NPCMissionData> getNPCsForLevel(int playerLevel) {
        // Always return all 3 NPCs - they're always available!
        return getAllNPCs();
    }
};