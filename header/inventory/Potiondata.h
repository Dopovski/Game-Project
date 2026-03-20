#pragma once
#include <string>
#include <ctime>
#include <SFML/Graphics.hpp>  // ADD THIS LINE

using namespace sf;

enum class PotionType {
    Health,
    Attack,
    Intelligence,
    Dexterity,
    Luck,
    Speed
};

enum class PotionRarity {
    Common,      // 50% chance, +10% stat
    Rare,        // 30% chance, +15% stat
    Mythical,    // 15% chance, +20% stat
    Legendary    // 5% chance, Speed potion only
};

struct Potion {
    PotionType type;
    PotionRarity rarity;
    std::string name;
    std::string description;
    int cost;           // Gold cost
    double statBonus;   // Percentage bonus (0.10 = 10%)
    int durationDays;   // How long potion lasts
    time_t purchaseTime; // When it was bought
    bool isActive;      // Whether effects are currently applied

    Potion() : type(PotionType::Health), rarity(PotionRarity::Common),
        cost(1), statBonus(0.10), durationDays(3),
        purchaseTime(0), isActive(false) {
    }

    Potion(PotionType t, PotionRarity r)
        : type(t), rarity(r), purchaseTime(0), isActive(false) {

        // Set cost and duration
        if (type == PotionType::Speed) {
            cost = 5;
            durationDays = 5;
        }
        else {
            cost = 1;
            durationDays = 3;
        }

        // Set stat bonus based on rarity
        switch (rarity) {
        case PotionRarity::Common:
            statBonus = 0.10; // 10%
            break;
        case PotionRarity::Rare:
            statBonus = 0.15; // 15%
            break;
        case PotionRarity::Mythical:
            statBonus = 0.20; // 20%
            break;
        case PotionRarity::Legendary:
            statBonus = 0.15; // 15% fatigue reduction for Speed
            break;
        }

        // Set name and description
        updateNameAndDescription();
    }

    void updateNameAndDescription() {
        // Rarity prefix
        std::string rarityStr;
        switch (rarity) {
        case PotionRarity::Common: rarityStr = "Common"; break;
        case PotionRarity::Rare: rarityStr = "Rare"; break;
        case PotionRarity::Mythical:  rarityStr = "Mythical"; break;
        case PotionRarity::Legendary: rarityStr = "Legendary"; break;
        }

        // Type name
        std::string typeStr;
        switch (type) {
        case PotionType::Health: typeStr = "Health"; break;
        case PotionType::Attack: typeStr = "Attack"; break;
        case PotionType::Intelligence: typeStr = "Intelligence"; break;
        case PotionType::Dexterity: typeStr = "Dexterity"; break;
        case PotionType::Luck: typeStr = "Luck"; break;
        case PotionType::Speed: typeStr = "Speed"; break;
        }

        name = rarityStr + " " + typeStr + " Potion";

        // Description
        if (type == PotionType::Speed) {
            description = "Reduces fatigue gain by 15%";
        }
        else {
            int bonus = static_cast<int>(statBonus * 100);
            description = "+" + std::to_string(bonus) + "% " + typeStr;
        }
    }

    // Check if potion has expired
    bool hasExpired() const {
        if (purchaseTime == 0) return false;

        time_t now = time(0);
        double secondsElapsed = difftime(now, purchaseTime);
        double daysElapsed = secondsElapsed / (24.0 * 60.0 * 60.0);

        return daysElapsed >= durationDays;
    }

    // Get remaining days
    int getRemainingDays() const {
        if (purchaseTime == 0) return durationDays;

        time_t now = time(0);
        double secondsElapsed = difftime(now, purchaseTime);
        double daysElapsed = secondsElapsed / (24.0 * 60.0 * 60.0);

        int remaining = durationDays - static_cast<int>(daysElapsed);
        return remaining > 0 ? remaining : 0;
    }

    // Get rarity color
    Color getRarityColor() const {
        switch (rarity) {
        case PotionRarity::Common:
            return Color(200, 200, 200); // Light gray
        case PotionRarity::Rare:
            return Color(100, 150, 255); // Blue
        case PotionRarity::Mythical:
            return Color(200, 100, 255); // Purple
        case PotionRarity::Legendary:
            return Color(255, 200, 50);  // Gold
        default:
            return Color::White;
        }
    }
};