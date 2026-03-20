#pragma once
#include <string>
#include <unordered_map>
#include "Mission.h"

using namespace std;

struct MonsterReward {
    string monsterName;
    int baseSilver;
    int baseXP;
    int shortSilver;
    int shortXP;
    int mediumSilver;
    int mediumXP;
    int longSilver;
    int longXP;
};

class MissionRewardDatabase {
public:
    // Get rewards for a specific level and mission type
    static MonsterReward getReward(int level, MissionType type) {
        // Clamp level between 1 and 100
        if (level < 1) level = 1;
        if (level > 100) level = 100;

        auto it = rewardTable.find(level);
        if (it != rewardTable.end()) {
            return it->second;
        }

        // If level not in table, calculate using formulas
        return calculateReward(level);
    }

    // Get silver reward for specific level and mission type
    static int getSilverReward(int level, MissionType type) {
        MonsterReward reward = getReward(level, type);
        switch (type) {
        case MissionType::Short:  return reward.shortSilver;
        case MissionType::Medium: return reward.mediumSilver;
        case MissionType::Long:   return reward.longSilver;
        default: return reward.shortSilver;
        }
    }

    // Get XP reward for specific level and mission type
    static int getXPReward(int level, MissionType type) {
        MonsterReward reward = getReward(level, type);
        switch (type) {
        case MissionType::Short:  return reward.shortXP;
        case MissionType::Medium: return reward.mediumXP;
        case MissionType::Long:   return reward.longXP;
        default: return reward.shortXP;
        }
    }

private:
    // Calculate reward using formulas from PDF
    static MonsterReward calculateReward(int level) {
        MonsterReward reward;
        reward.monsterName = "Level " + to_string(level) + " Monster";

        // Base formulas
        reward.baseSilver = level * 4;
        reward.baseXP = level * 10;

        // Short Mission: ?1.0
        reward.shortSilver = reward.baseSilver;
        reward.shortXP = reward.baseXP;

        // Medium Mission: ?1.8 Silver / ?1.6 XP
        reward.mediumSilver = static_cast<int>(reward.baseSilver * 1.8);
        reward.mediumXP = static_cast<int>(reward.baseXP * 1.6);

        // Long Mission: ?3.0 Silver / ?2.5 XP
        reward.longSilver = static_cast<int>(reward.baseSilver * 3.0);
        reward.longXP = static_cast<int>(reward.baseXP * 2.5);

        return reward;
    }

    // Complete reward table from PDF (levels 1-100)
    static const unordered_map<int, MonsterReward> rewardTable;
};