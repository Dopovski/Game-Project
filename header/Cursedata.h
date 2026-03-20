#pragma once
#include <string>

struct CurseModifiers {
    double healthModifier;
    double attackModifier;
    double intelligenceModifier;
    double dexterityModifier;
    double armorModifier;
    double luckModifier;

    CurseModifiers()
        : healthModifier(0.0), attackModifier(0.0), intelligenceModifier(0.0),
        dexterityModifier(0.0), armorModifier(0.0), luckModifier(0.0) {
    }
};

struct CurseData {
    int curseId;
    std::string curseName;
    std::string description;
    CurseModifiers modifiers;
    bool isFirstCurse;

    CurseData()
        : curseId(-1), curseName(""), description(""), isFirstCurse(false) {
    }

    CurseData(int id, const std::string& name, const std::string& desc,
        double health, double attack, double intel, double dex, double armor, double luck, bool first = false)
        : curseId(id), curseName(name), description(desc), isFirstCurse(first) {
        modifiers.healthModifier = health;
        modifiers.attackModifier = attack;
        modifiers.intelligenceModifier = intel;
        modifiers.dexterityModifier = dex;
        modifiers.armorModifier = armor;
        modifiers.luckModifier = luck;
    }
};

struct CharacterCurseState {
    CurseData currentCurse;
    bool hasFirstCurse;
    bool unlockedClass;

    CharacterCurseState()
        : hasFirstCurse(false), unlockedClass(false) {
    }
};