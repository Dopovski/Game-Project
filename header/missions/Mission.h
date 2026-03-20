#pragma once
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

enum class MissionType {
    Short,
    Medium,
    Long
};

struct MissionRewards {
    int silver;
    int gold;
    int torches;
    double xp;
};

class Mission {
public:
    Mission(MissionType type, const string& npcName, const string& monsterName,
        const string& monsterTexture, int monsterLevel);

    void start();
    bool isComplete() const;
    double getRemainingSeconds() const;
    double getTotalSeconds() const;
    double getProgressPercent() const;

    MissionRewards getRewards() const { return rewards; }
    string getMonsterName() const { return monsterName; }
    string getMonsterTexture() const { return monsterTexture; }
    int getMonsterLevel() const { return monsterLevel; }
    string getNPCName() const { return npcName; }
    MissionType getType() const { return type; }

    void skip() { completed = true; }

    void setPlayerRace(const string& race) { playerRace = race; }
    string getPlayerRace() const { return playerRace; }

    void setMountSpeedBonus(float bonus);

    void applyRacialBonuses();

private:
    MissionType type;
    string npcName;
    string monsterName;
    string monsterTexture;
    int monsterLevel;
    string playerRace;
    float mountSpeedBonus;

    time_point<steady_clock> startTime;
    double durationSeconds;
    bool started;
    bool completed;

    MissionRewards rewards;

    void calculateDuration();
    void calculateRewards();
};