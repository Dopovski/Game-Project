#include "Mission.h"
#include <cmath>
#include <random>
#include <iostream>

Mission::Mission(MissionType type, const string& npcName, const string& monsterName,
    const string& monsterTexture, int monsterLevel)
    : type(type), npcName(npcName), monsterName(monsterName),
    monsterTexture(monsterTexture), monsterLevel(monsterLevel),
    started(false), completed(false), playerRace(""), mountSpeedBonus(0.0f) {

    // Calculate duration based on level bracket (PDF specification)
    calculateDuration();
    calculateRewards();
}

void Mission::calculateDuration() {
    // Level 1-10: Seconds-based (BaseTime = Level × 6) - REDUCED for faster early gameplay
    if (monsterLevel <= 10) {
        double baseTime = monsterLevel * 6.0; // SHORT mission time in seconds (reduced from 12)

        switch (type) {
        case MissionType::Short:
            durationSeconds = baseTime;
            break;
        case MissionType::Medium:
            durationSeconds = baseTime * 3.0; // ×3 for medium
            break;
        case MissionType::Long:
            durationSeconds = baseTime * 6.0; // ×6 for long
            break;
        }
    }
    // Level 11-100: Minutes-based with distance brackets
    else {
        double shortKm, mediumKm, longKm;

        // Determine distance based on level bracket
        if (monsterLevel <= 20) {        // 11-20
            shortKm = 0.5;
            mediumKm = 1.5;
            longKm = 3.0;
        }
        else if (monsterLevel <= 40) {  // 21-40
            shortKm = 1.0;
            mediumKm = 3.0;
            longKm = 6.0;
        }
        else if (monsterLevel <= 60) {  // 41-60
            shortKm = 2.0;
            mediumKm = 5.0;
            longKm = 8.0;
        }
        else if (monsterLevel <= 80) {  // 61-80
            shortKm = 3.0;
            mediumKm = 7.0;
            longKm = 10.0;
        }
        else {                           // 81-100
            shortKm = 4.0;
            mediumKm = 9.0;
            longKm = 14.0;
        }

        // Convert km to seconds (1 km = 12 minutes = 720 seconds)
        double kmToSeconds = 720.0;

        switch (type) {
        case MissionType::Short:
            durationSeconds = shortKm * kmToSeconds;
            break;
        case MissionType::Medium:
            durationSeconds = mediumKm * kmToSeconds;
            break;
        case MissionType::Long:
            durationSeconds = longKm * kmToSeconds;
            break;
        }
    }

    // Apply mount speed bonus
    if (mountSpeedBonus > 0.0f) {
        durationSeconds = durationSeconds * (1.0 - mountSpeedBonus);
    }
}

void Mission::start() {
    if (!started) {
        startTime = steady_clock::now();
        started = true;
    }
}

bool Mission::isComplete() const {
    if (completed) return true;
    if (!started) return false;

    auto now = steady_clock::now();
    auto elapsed = duration_cast<chrono::seconds>(now - startTime).count();
    return elapsed >= durationSeconds;
}

double Mission::getRemainingSeconds() const {
    if (!started || completed) return 0.0;

    auto now = steady_clock::now();
    auto elapsed = duration_cast<chrono::seconds>(now - startTime).count();
    double remaining = durationSeconds - elapsed;
    return remaining > 0.0 ? remaining : 0.0;
}

double Mission::getTotalSeconds() const {
    return durationSeconds;
}

double Mission::getProgressPercent() const {
    if (!started) return 0.0;
    if (completed) return 100.0;

    auto now = steady_clock::now();
    auto elapsed = duration_cast<chrono::seconds>(now - startTime).count();
    double percent = (elapsed / durationSeconds) * 100.0;
    return percent > 100.0 ? 100.0 : percent;
}

void Mission::calculateRewards() {
    // XP rewards based on PDF table for levels 1-10
    // Then scales with level for 11-100

    // Random number generator for percentage-based drops
    // Each mission gets fresh random rolls
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 100);

    if (monsterLevel <= 10) {
        // PDF Table for levels 1-10
        int baseXP = monsterLevel * 10;

        switch (type) {
        case MissionType::Short:
            rewards.xp = baseXP;
            rewards.silver = monsterLevel * 4;
            // 17% chance for 1 gold, 42% chance for 1 torch
            rewards.gold = (dis(gen) <= 17) ? 1 : 0;
            rewards.torches = (dis(gen) <= 42) ? 1 : 0;
            break;
        case MissionType::Medium:
            rewards.xp = baseXP * 1.6;
            rewards.silver = static_cast<int>(monsterLevel * 4 * 1.8);
            // 17% chance for 1 gold, 42% chance for 1 torch
            rewards.gold = (dis(gen) <= 17) ? 1 : 0;
            rewards.torches = (dis(gen) <= 42) ? 1 : 0;
            break;
        case MissionType::Long:
            rewards.xp = baseXP * 2.5;
            rewards.silver = static_cast<int>(monsterLevel * 4 * 3.0);
            // 17% chance for 1 gold, 42% chance for 1 torch
            rewards.gold = (dis(gen) <= 17) ? 1 : 0;
            rewards.torches = (dis(gen) <= 42) ? 1 : 0;
            break;
        }
    }
    else {
        // Levels 11-100: Scale rewards with level
        int baseXP = monsterLevel * 10;

        switch (type) {
        case MissionType::Short:
            rewards.xp = baseXP;
            rewards.silver = monsterLevel * 4;
            // 17% chance for 1 gold, 42% chance for 1 torch
            rewards.gold = (dis(gen) <= 17) ? 1 : 0;
            rewards.torches = (dis(gen) <= 42) ? 1 : 0;
            break;
        case MissionType::Medium:
            rewards.xp = baseXP * 1.6;
            rewards.silver = static_cast<int>(monsterLevel * 4 * 1.8);
            // 17% chance for 1 gold, 42% chance for 1 torch
            rewards.gold = (dis(gen) <= 17) ? 1 : 0;
            rewards.torches = (dis(gen) <= 42) ? 1 : 0;
            break;
        case MissionType::Long:
            rewards.xp = baseXP * 2.5;
            rewards.silver = static_cast<int>(monsterLevel * 4 * 3.0);
            // 17% chance for 1 gold, 42% chance for 1 torch
            rewards.gold = (dis(gen) <= 17) ? 1 : 0;
            rewards.torches = (dis(gen) <= 42) ? 1 : 0;
            break;
        }
    }
}
void Mission::applyRacialBonuses() {
    // Human Silver Tongue: +5% silver gain from all sources
    if (playerRace == "Human") {
        int originalSilver = rewards.silver;
        rewards.silver = static_cast<int>(rewards.silver * 1.05);
        int bonus = rewards.silver - originalSilver;

        cout << "\n[Silver Tongue] Human racial ability activated!" << endl;
        cout << "  Original silver: " << originalSilver << endl;
        cout << "  Bonus silver (+5%): " << bonus << endl;
        cout << "  Total silver: " << rewards.silver << endl;
    }
}

void Mission::setMountSpeedBonus(float bonus) {
    mountSpeedBonus = bonus;
    // Recalculate duration with the new mount speed bonus
    calculateDuration();
}