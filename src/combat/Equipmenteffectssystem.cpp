#include "EquipmentEffectsSystem.h"
#include "CharacterSheet.h"
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace sf;

EquipmentEffectsSystem::EquipmentEffectsSystem()
    : playerSheet(nullptr),
    attackCount(0),
    playerAttackCount(0),
    enemyAttackCount(0),
    playerCritCount(0),
    playerDodgeCount(0),
    firstCritUsed(false),
    firstPlayerAttackUsed(false),
    secondAttackDodged(false),
    secondEnemyAttackDodged(false),
    firstEnemyAttackProcessed(false),
    thirdEnemyAttackDodged(false),
    fourthEnemyAttackDodged(false),
    permanentAttackBonus(0.0),
    permanentDexterityBonus(0.0),
    poisonDamage(0),
    poisonTurnsRemaining(0),
    armorShredStacks(0),
    bonusArmorPen(0.0),
    currentShield(0.0),
    rhythmBladeCounter(0),
    openWoundApplied(false),
    enemyHasOpenWound(false),
    critDamageBonus(0.0),
    killCount(0),
    floatingTextCallback(nullptr),
    playerPosition(0.0f, 0.0f),
    enemyPosition(0.0f, 0.0f) {
}

EquipmentEffectsSystem::~EquipmentEffectsSystem() {
}

void EquipmentEffectsSystem::initialize(CharacterSheet* sheet) {
    playerSheet = sheet;

    if (!playerSheet) {
        cout << "[Equipment System] ERROR: No CharacterSheet provided!" << endl;
        return;
    }

    // Get equipped items from CharacterSheet
    equippedWeapon = playerSheet->getEquippedItem(EquipmentSlot::Weapon);
    equippedHelmet = playerSheet->getEquippedItem(EquipmentSlot::Helmet);
    equippedChestplate = playerSheet->getEquippedItem(EquipmentSlot::Chestplate);
    equippedPants = playerSheet->getEquippedItem(EquipmentSlot::Pants);
    equippedBoots = playerSheet->getEquippedItem(EquipmentSlot::Boots);
    equippedTalisman = playerSheet->getEquippedItem(EquipmentSlot::Talisman);
    equippedCompanion = playerSheet->getEquippedItem(EquipmentSlot::Companion);

    // Debug output
    cout << "\n[Equipment System] Loaded equipment:" << endl;
    if (!equippedWeapon.empty()) cout << "  Weapon: " << equippedWeapon << endl;
    if (!equippedHelmet.empty()) cout << "  Helmet: " << equippedHelmet << endl;
    if (!equippedChestplate.empty()) cout << "  Chestplate: " << equippedChestplate << endl;
    if (!equippedPants.empty()) cout << "  Pants: " << equippedPants << endl;
    if (!equippedBoots.empty()) cout << "  Boots: " << equippedBoots << endl;
    if (!equippedTalisman.empty()) cout << "  Talisman: " << equippedTalisman << endl;
    if (!equippedCompanion.empty()) cout << "  Companion: " << equippedCompanion << endl;
    cout << endl;

    // Calculate bonus armor penetration from items
    bonusArmorPen = 0.0;
    if (hasItem("Shadowpiercer Crossbow")) bonusArmorPen += 18.0;
    if (hasItem("Deathwhisper Dagger")) bonusArmorPen += 10.0;
    if (hasItem("Armor Shredder")) bonusArmorPen += 20.0;
    if (hasItem("Dynamites Boots")) bonusArmorPen += 10.0;
    if (hasItem("Silent Runners")) bonusArmorPen += 10.0;
    if (hasItem("Phantom Serpent")) bonusArmorPen += 10.0;

    // Calculate crit damage bonus
    critDamageBonus = 0.0;
    if (hasItem("Assassin's Mark")) critDamageBonus += 0.25; // +25% crit damage

    reset();
}

void EquipmentEffectsSystem::setEquippedItem(const string& slotName, const string& itemName) {
    if (slotName == "Weapon") equippedWeapon = itemName;
    else if (slotName == "Helmet") equippedHelmet = itemName;
    else if (slotName == "Chestplate") equippedChestplate = itemName;
    else if (slotName == "Pants") equippedPants = itemName;
    else if (slotName == "Boots") equippedBoots = itemName;
    else if (slotName == "Talisman") equippedTalisman = itemName;
    else if (slotName == "Companion") equippedCompanion = itemName;
}

void EquipmentEffectsSystem::reset() {
    attackCount = 0;
    playerAttackCount = 0;
    enemyAttackCount = 0;
    playerCritCount = 0;
    playerDodgeCount = 0;
    firstCritUsed = false;
    firstPlayerAttackUsed = false;
    secondAttackDodged = false;
    secondEnemyAttackDodged = false;
    firstEnemyAttackProcessed = false;
    thirdEnemyAttackDodged = false;
    fourthEnemyAttackDodged = false;
    permanentAttackBonus = 0.0;
    permanentDexterityBonus = 0.0;
    poisonDamage = 0;
    poisonTurnsRemaining = 0;
    armorShredStacks = 0;
    currentShield = 0.0;
    rhythmBladeCounter = 0;
    openWoundApplied = false;
    enemyHasOpenWound = false;
    // Don't reset killCount - persists across combats in dungeon
}

bool EquipmentEffectsSystem::hasItem(const string& itemName) const {
    return equippedWeapon == itemName ||
        equippedHelmet == itemName ||
        equippedChestplate == itemName ||
        equippedPants == itemName ||
        equippedBoots == itemName ||
        equippedTalisman == itemName ||
        equippedCompanion == itemName;
}

void EquipmentEffectsSystem::addPermanentAttackBonus(double bonus) {
    permanentAttackBonus += bonus;
    cout << "[Equipment Buff] +Attack: +" << bonus << " (Total: +" << permanentAttackBonus << ")" << endl;
}

void EquipmentEffectsSystem::addPermanentDexterityBonus(double bonus) {
    permanentDexterityBonus += bonus;
    cout << "[Equipment Buff] +Dexterity: +" << bonus << " (Total: +" << permanentDexterityBonus << ")" << endl;
}

void EquipmentEffectsSystem::onCombatStart(double& playerHealth, double& shield) {
    cout << "\n=== Assassin Equipment Effects Activated ===" << endl;

    // COMMON ITEMS
    // Metal - Gain 20 Shield at start
    if (hasItem("Metal")) {
        currentShield += 20.0;
        shield = currentShield;
        cout << "[Metal] Gained 20 Shield!" << endl;
        spawnFloatingText("Metal: +20 Shield", Color(150, 150, 200), true);
    }

    // RARE ITEMS
    // Shadowweave Armor - Gain 30 Shield at start
    if (hasItem("Shadowweave Armor")) {
        currentShield += 30.0;
        shield = currentShield;
        cout << "[Shadowweave Armor] Gained 30 Shield!" << endl;
        spawnFloatingText("Shadowweave: +30 Shield", Color(150, 150, 200), true);
    }

    // Shadow Crystal - Gain 20 Shield at start
    if (hasItem("Shadow Crystal")) {
        currentShield += 20.0;
        shield = currentShield;
        cout << "[Shadow Crystal] Gained 20 Shield!" << endl;
        spawnFloatingText("Shadow Crystal: +20 Shield", Color(150, 150, 200), true);
    }

    // Shield Crusher - +10 Shield at start
    if (hasItem("Shield Crusher")) {
        currentShield += 10.0;
        shield = currentShield;
        cout << "[Shield Crusher] Gained 10 Shield!" << endl;
        spawnFloatingText("Shield Crusher: +10 Shield", Color(150, 150, 200), true);
    }

    // Heart of Shadows - Add kill bonus attack
    if (hasItem("Heart of Shadows") && killCount > 0) {
        double killBonus = killCount * 5.0;
        permanentAttackBonus += killBonus;
        cout << "[Heart of Shadows] +" << killBonus << " Attack from " << killCount << " kills!" << endl;
        spawnFloatingText("Heart: +" + to_string((int)killBonus) + " ATK", Color(255, 100, 100), true);
    }

    cout << "==========================================\n" << endl;
}

void EquipmentEffectsSystem::onTurnStart() {
    attackCount++;
}

void EquipmentEffectsSystem::onPlayerAttackStart(double& attackBonus, bool& forceDoubleTap, bool& forceCrit) {
    playerAttackCount++;

    // COMMON ITEMS
    // Rogue Cat - First attack guaranteed crit
    if (hasItem("Rogue Cat") && !firstPlayerAttackUsed) {
        firstPlayerAttackUsed = true;
        forceCrit = true;
        cout << "[Rogue Cat] First attack guaranteed critical!" << endl;
        spawnFloatingText("Rogue Cat: Crit!", Color(255, 50, 50), true);
    }

    // Shadowstep Boots - First attack: +15 Attack (below 50% HP)
    if (hasItem("Shadowstep Boots") && playerAttackCount == 1) {
        attackBonus += 15.0;
        cout << "[Shadowstep Boots] First attack: +15 Attack!" << endl;
        spawnFloatingText("Shadowstep: +15 ATK", Color(255, 100, 100), true);
    }

    // Stun - Attack twice
    if (hasItem("Stun")) {
        forceDoubleTap = true;
        cout << "[Stun] Double attack!" << endl;
        spawnFloatingText("Stun: Double!", Color(255, 255, 100), true);
    }

    // RARE ITEMS
    // Rhythm Blade - Every 3rd attack guaranteed crit
    if (hasItem("Rhythm Blade")) {
        rhythmBladeCounter++;
        if (rhythmBladeCounter >= 3) {
            forceCrit = true;
            rhythmBladeCounter = 0;
            cout << "[Rhythm Blade] Every 3rd attack - Guaranteed crit!" << endl;
            spawnFloatingText("Rhythm Blade: Crit!", Color(255, 50, 50), true);
        }
    }

    // Soulreaver Bow - Each attack deals +15 bonus damage
    if (hasItem("Soulreaver Bow")) {
        attackBonus += 15.0;
        cout << "[Soulreaver Bow] +15 bonus damage!" << endl;
        spawnFloatingText("Soulreaver: +15 DMG", Color(255, 100, 100), true);
    }

    // Nightblade Boots - First attack +15 Attack
    if (hasItem("Nightblade Boots") && playerAttackCount == 1) {
        attackBonus += 15.0;
        cout << "[Nightblade Boots] First attack: +15 Attack!" << endl;
        spawnFloatingText("Nightblade: +15 ATK", Color(255, 100, 100), true);
    }

    // Assassin's Shroud - First attack +25 damage
    if (hasItem("Assassin's Shroud") && playerAttackCount == 1) {
        attackBonus += 25.0;
        cout << "[Assassin's Shroud] First attack: +25 damage!" << endl;
        spawnFloatingText("Shroud: +25 DMG", Color(255, 100, 100), true);
    }

    // Shadow Panther - First attack +15 Attack
    if (hasItem("Shadow Panther") && playerAttackCount == 1) {
        attackBonus += 15.0;
        cout << "[Shadow Panther] First attack: +15 Attack!" << endl;
        spawnFloatingText("Panther: +15 ATK", Color(255, 100, 100), true);
    }

    // Add permanent bonuses
    attackBonus += permanentAttackBonus;
}

void EquipmentEffectsSystem::onPlayerAttackHit(double& playerHealth, int& bonusSilver, double maxHealth) {
    // COMMON ITEMS
    // Short Blade - Heal 2.5 HP after attack
    if (hasItem("Short Blade")) {
        healPlayer(playerHealth, 2.5, maxHealth, "Short Blade");
        spawnFloatingText("Short Blade: +2.5 HP", Color(50, 255, 50), true);
    }

    // The Farmer's Scythe - +3 HP on hit
    if (hasItem("The Farmer's Scythe")) {
        healPlayer(playerHealth, 3.0, maxHealth, "The Farmer's Scythe");
        spawnFloatingText("Scythe: +3 HP", Color(50, 255, 50), true);
    }

    // Boots of Healing - +4 HP per attack
    if (hasItem("Boots of Healing")) {
        healPlayer(playerHealth, 4.0, maxHealth, "Boots of Healing");
        spawnFloatingText("Healing Boots: +4 HP", Color(50, 255, 50), true);
    }

    // Wolf - Heal 3 HP per attack
    if (hasItem("Wolf")) {
        healPlayer(playerHealth, 3.0, maxHealth, "Wolf");
        spawnFloatingText("Wolf: +3 HP", Color(50, 255, 50), true);
    }

    // Dagger - Shred 3 armor (max 5 stacks = 15)
    if (hasItem("Dagger") && armorShredStacks < 5) {
        armorShredStacks++;
        cout << "[Dagger] Armor shred stack: " << armorShredStacks << "/5" << endl;
        spawnFloatingText("Dagger: Armor Shred", Color(255, 150, 50), false);
    }

    // RARE ITEMS
    // Vampiric Fang - +10 HP on hit
    if (hasItem("Vampiric Fang")) {
        healPlayer(playerHealth, 10.0, maxHealth, "Vampiric Fang");
        spawnFloatingText("Vampiric Fang: +10 HP", Color(50, 255, 50), true);
    }

    // Deathdealer's Garb - +3 HP per attack
    if (hasItem("Deathdealer's Garb")) {
        healPlayer(playerHealth, 3.0, maxHealth, "Deathdealer's Garb");
        spawnFloatingText("Deathdealer: +3 HP", Color(50, 255, 50), true);
    }

    // Blood Bat - +8 HP on hit
    if (hasItem("Blood Bat")) {
        healPlayer(playerHealth, 8.0, maxHealth, "Blood Bat");
        spawnFloatingText("Blood Bat: +8 HP", Color(50, 255, 50), true);
    }

    // Venomfang Blade - Apply Open Wound
    if (hasItem("Venomfang Blade") && !enemyHasOpenWound) {
        enemyHasOpenWound = true;
        cout << "[Venomfang Blade] Applied Open Wound - enemy healing reduced by 40%!" << endl;
        spawnFloatingText("Open Wound!", Color(255, 50, 50), false);
    }

    // Phantom Edge - Gain 30 Shield after first attack
    if (hasItem("Phantom Edge") && playerAttackCount == 1) {
        currentShield += 30.0;
        cout << "[Phantom Edge] Gained 30 Shield after first attack!" << endl;
        spawnFloatingText("Phantom Edge: +30 Shield", Color(150, 150, 200), true);
    }

    // Swift Striders - +5 Dex after each attack
    if (hasItem("Swift Striders")) {
        addPermanentDexterityBonus(5.0);
        spawnFloatingText("Swift Striders: +5 Dex", Color(200, 100, 255), true);
    }

    // Shadow Leggings - +5 Dex after each attack
    if (hasItem("Shadow Leggings")) {
        addPermanentDexterityBonus(5.0);
        spawnFloatingText("Shadow Leggings: +5 Dex", Color(200, 100, 255), true);
    }

    // Venomous Spider - Apply poison (4 dmg/turn, 3 turns)
    if (hasItem("Venomous Spider") && poisonTurnsRemaining == 0) {
        poisonDamage = 4;
        poisonTurnsRemaining = 3;
        cout << "[Venomous Spider] Enemy poisoned! 4 damage for 3 turns" << endl;
        spawnFloatingText("Spider Poison!", Color(100, 255, 100), false);
    }

    // Phantom Serpent - Crits apply Open Wound (handled in onPlayerCrit)
}

void EquipmentEffectsSystem::onPlayerCrit(double& attackBonus, double& playerHealth, double maxHealth) {
    playerCritCount++;

    // COMMON ITEMS
    // Silent Hood - First crit: +10 bonus Attack
    if (hasItem("Silent Hood") && !firstCritUsed) {
        firstCritUsed = true;
        attackBonus += 10.0;
        cout << "[Silent Hood] First crit: +10 Attack!" << endl;
        spawnFloatingText("Silent Hood: +10 ATK", Color(255, 100, 100), true);
    }

    // Rogue's Cap - Heal 2 HP after crit
    if (hasItem("Rogue's Cap")) {
        healPlayer(playerHealth, 2.0, maxHealth, "Rogue's Cap");
        spawnFloatingText("Rogue's Cap: +2 HP", Color(50, 255, 50), true);
    }

    // Nightstalker Vest - +5 Attack per crit
    if (hasItem("Nightstalker Vest")) {
        addPermanentAttackBonus(5.0);
        spawnFloatingText("Nightstalker: +5 ATK", Color(255, 100, 100), true);
    }

    // Rogue's Greaves - +5 Dex after crit
    if (hasItem("Rogue's Greaves")) {
        addPermanentDexterityBonus(5.0);
        spawnFloatingText("Rogue's Greaves: +5 Dex", Color(200, 100, 255), true);
    }

    // RARE ITEMS
    // Veil of Shadows - First crit +20 bonus damage
    if (hasItem("Veil of Shadows") && playerCritCount == 1) {
        attackBonus += 20.0;
        cout << "[Veil of Shadows] First crit: +20 bonus damage!" << endl;
        spawnFloatingText("Veil: +20 DMG", Color(255, 100, 100), true);
    }

    // Whisper Mask - Gain 15 Shield on crit
    if (hasItem("Whisper Mask")) {
        currentShield += 15.0;
        cout << "[Whisper Mask] Gained 15 Shield on crit!" << endl;
        spawnFloatingText("Whisper Mask: +15 Shield", Color(150, 150, 200), true);
    }

    // Deathwalk Boots - +5 HP per crit
    if (hasItem("Deathwalk Boots")) {
        healPlayer(playerHealth, 5.0, maxHealth, "Deathwalk Boots");
        spawnFloatingText("Deathwalk: +5 HP", Color(50, 255, 50), true);
    }

    // Phantom Serpent - Crits apply Open Wound
    if (hasItem("Phantom Serpent") && !enemyHasOpenWound) {
        enemyHasOpenWound = true;
        cout << "[Phantom Serpent] Crit applied Open Wound!" << endl;
        spawnFloatingText("Serpent: Open Wound!", Color(255, 50, 50), false);
    }

    // Apply crit damage bonus from Assassin's Mark
    if (critDamageBonus > 0.0) {
        double bonusDmg = attackBonus * critDamageBonus;
        attackBonus += bonusDmg;
        cout << "[Assassin's Mark] +25% crit damage bonus: +" << bonusDmg << endl;
    }
}

void EquipmentEffectsSystem::onPlayerDodge(double& dexterityBonus) {
    playerDodgeCount++;

    // COMMON ITEMS
    // Shadow Hood - +5 Dex after dodge
    if (hasItem("Shadow Hood")) {
        addPermanentDexterityBonus(5.0);
        spawnFloatingText("Shadow Hood: +5 Dex", Color(200, 100, 255), true);
    }

    // Shadow Rat - +10 Dex per dodge
    if (hasItem("Shadow Rat")) {
        addPermanentDexterityBonus(10.0);
        spawnFloatingText("Shadow Rat: +10 Dex", Color(200, 100, 255), true);
    }

    // RARE ITEMS
    // Death's Shroud - +5 Attack after dodge
    if (hasItem("Death's Shroud")) {
        addPermanentAttackBonus(5.0);
        spawnFloatingText("Death's Shroud: +5 ATK", Color(255, 100, 100), true);
    }

    // Shadow Pants - +10 Attack after dodging
    if (hasItem("Shadow Pants")) {
        addPermanentAttackBonus(10.0);
        spawnFloatingText("Shadow Pants: +10 ATK", Color(255, 100, 100), true);
    }

    // Shadow Panther - +5 Dex per dodge
    if (hasItem("Shadow Panther")) {
        addPermanentDexterityBonus(5.0);
        spawnFloatingText("Panther: +5 Dex", Color(200, 100, 255), true);
    }

    dexterityBonus += permanentDexterityBonus;
}

void EquipmentEffectsSystem::onPlayerTakeDamage(double& dexterityBonus, bool& shouldDodge) {
    // COMMON ITEMS
    // Silent Armor - +5 Dex when attacked
    if (hasItem("Silent Armor")) {
        addPermanentDexterityBonus(5.0);
        spawnFloatingText("Silent Armor: +5 Dex", Color(200, 100, 255), true);
    }

    // Nightblade Mask - Dodge second attack
    if (hasItem("Nightblade Mask") && enemyAttackCount == 2 && !secondAttackDodged) {
        shouldDodge = true;
        secondAttackDodged = true;
        cout << "[Nightblade Mask] Dodged second attack!" << endl;
        spawnFloatingText("Nightblade: Dodge!", Color(100, 200, 255), true);
    }

    // Silent Steps - Dodge second attack
    if (hasItem("Silent Steps") && enemyAttackCount == 2 && !secondEnemyAttackDodged) {
        shouldDodge = true;
        secondEnemyAttackDodged = true;
        cout << "[Silent Steps] Dodged second attack!" << endl;
        spawnFloatingText("Silent Steps: Dodge!", Color(100, 200, 255), true);
    }

    // RARE ITEMS
    // Phantom Visage - Dodge third enemy attack
    if (hasItem("Phantom Visage") && enemyAttackCount == 3 && !thirdEnemyAttackDodged) {
        shouldDodge = true;
        thirdEnemyAttackDodged = true;
        cout << "[Phantom Visage] Dodged third attack!" << endl;
        spawnFloatingText("Phantom Visage: Dodge!", Color(100, 200, 255), true);
    }

    // Phantom Greaves - Dodge 2nd and 4th enemy attacks
    if (hasItem("Phantom Greaves")) {
        if (enemyAttackCount == 2 && !secondEnemyAttackDodged) {
            shouldDodge = true;
            secondEnemyAttackDodged = true;
            cout << "[Phantom Greaves] Dodged second attack!" << endl;
            spawnFloatingText("Phantom Greaves: Dodge!", Color(100, 200, 255), true);
        }
        if (enemyAttackCount == 4 && !fourthEnemyAttackDodged) {
            shouldDodge = true;
            fourthEnemyAttackDodged = true;
            cout << "[Phantom Greaves] Dodged fourth attack!" << endl;
            spawnFloatingText("Phantom Greaves: Dodge!", Color(100, 200, 255), true);
        }
    }

    // Phantom Step - Dodge first enemy attack
    if (hasItem("Phantom Step") && enemyAttackCount == 1 && !firstEnemyAttackProcessed) {
        shouldDodge = true;
        cout << "[Phantom Step] Dodged first attack!" << endl;
        spawnFloatingText("Phantom Step: Dodge!", Color(100, 200, 255), true);
    }

    // Cloak of Evasion - 10% chance to dodge any attack
    if (hasItem("Cloak of Evasion")) {
        int roll = rand() % 100;
        if (roll < 10) {
            shouldDodge = true;
            cout << "[Cloak of Evasion] 10% dodge triggered!" << endl;
            spawnFloatingText("Cloak: Evade!", Color(100, 200, 255), true);
        }
    }

    dexterityBonus += permanentDexterityBonus;
}

void EquipmentEffectsSystem::onEnemyAttackStart(bool& enemyShouldMiss, double& enemyArmorReduction) {
    enemyAttackCount++;

    // COMMON ITEMS
    // Crows of Misfortune - 20% chance enemy misses first attack
    if (hasItem("Crows of Misfortune") && enemyAttackCount == 1) {
        int roll = rand() % 100;
        if (roll < 20) {
            enemyShouldMiss = true;
            cout << "[Crows of Misfortune] Enemy missed first attack!" << endl;
            spawnFloatingText("Crows: Miss!", Color(255, 255, 100), true);
        }
        firstEnemyAttackProcessed = true;
    }

    // Flask - Second enemy attack deals 0
    if (hasItem("Flask") && enemyAttackCount == 2 && !secondEnemyAttackDodged) {
        enemyShouldMiss = true;
        secondEnemyAttackDodged = true;
        cout << "[Flask] Second enemy attack blocked!" << endl;
        spawnFloatingText("Flask: Blocked!", Color(100, 200, 255), true);
    }

    // Dynamites Boots - Apply armor pen
    if (hasItem("Dynamites Boots")) {
        enemyArmorReduction += 10.0;
    }

    // Dagger armor shred
    if (hasItem("Dagger") && armorShredStacks > 0) {
        enemyArmorReduction += armorShredStacks * 3.0;
    }

    // RARE ITEMS
    // Ghost Raven - 25% chance enemy misses first attack
    if (hasItem("Ghost Raven") && enemyAttackCount == 1 && !firstEnemyAttackProcessed) {
        int roll = rand() % 100;
        if (roll < 25) {
            enemyShouldMiss = true;
            cout << "[Ghost Raven] Enemy missed first attack (25%)!" << endl;
            spawnFloatingText("Raven: Miss!", Color(255, 255, 100), true);
        }
        firstEnemyAttackProcessed = true;
    }

    // Apply all bonus armor pen
    enemyArmorReduction += bonusArmorPen;
}

void EquipmentEffectsSystem::onCombatEnd(bool playerWon, int& bonusXP, int& bonusSilver, bool isArena) {
    if (!playerWon) return;

    // Track kill for Heart of Shadows
    if (!isArena) {
        killCount++;
    }

    // COMMON ITEMS
    // The Farmer's Scythe - +10 Silver
    if (hasItem("The Farmer's Scythe")) {
        bonusSilver += 10;
        cout << "[Farmer's Scythe] +10 Silver" << endl;
        spawnFloatingText("Scythe: +10 Silver", Color(192, 192, 192), true);
    }

    // Wolf - +80 XP (dungeon only)
    if (hasItem("Wolf") && !isArena) {
        bonusXP += 80;
        cout << "[Wolf] +80 XP" << endl;
        spawnFloatingText("Wolf: +80 XP", Color(100, 255, 255), true);
    }

    // Merchant Rat - +25 Silver (dungeon only)
    if (hasItem("Merchant Rat") && !isArena) {
        bonusSilver += 25;
        cout << "[Merchant Rat] +25 Silver" << endl;
        spawnFloatingText("Merchant Rat: +25 Silver", Color(192, 192, 192), true);
    }

    // RARE ITEMS
    // Blood Bat - +100 XP (dungeon only)
    if (hasItem("Blood Bat") && !isArena) {
        bonusXP += 100;
        cout << "[Blood Bat] +100 XP" << endl;
        spawnFloatingText("Blood Bat: +100 XP", Color(100, 255, 255), true);
    }
}

void EquipmentEffectsSystem::applyPoisonDamage(double& enemyHealth) {
    // Apply existing poison
    if (poisonTurnsRemaining > 0) {
        enemyHealth -= poisonDamage;
        poisonTurnsRemaining--;
        cout << "[Poison] " << poisonDamage << " damage! (" << poisonTurnsRemaining << " turns left)" << endl;
        spawnFloatingText("Poison: -" + to_string(poisonDamage), Color(100, 255, 100), false);
    }

    // Apply new poison from items
    if (poisonTurnsRemaining == 0) {
        // Poisoned Trousers - 3 dmg for 2 turns
        if (hasItem("Poisoned Trousers")) {
            poisonDamage = 3;
            poisonTurnsRemaining = 2;
            cout << "[Poisoned Trousers] Poison applied!" << endl;
            spawnFloatingText("Poison!", Color(100, 255, 100), false);
        }

        // Vial of Venom - 5 dmg for 3 turns
        if (hasItem("Vial of Venom")) {
            poisonDamage = 5;
            poisonTurnsRemaining = 3;
            cout << "[Vial of Venom] Poison applied!" << endl;
            spawnFloatingText("Venom!", Color(100, 255, 100), false);
        }

        // Venom Vial (Rare) - 8 dmg for 2 turns
        if (hasItem("Venom Vial")) {
            poisonDamage = 8;
            poisonTurnsRemaining = 2;
            cout << "[Venom Vial] Strong poison applied!" << endl;
            spawnFloatingText("Strong Poison!", Color(100, 255, 100), false);
        }

        // Venom-Laced Pants - 5 dmg for 3 turns
        if (hasItem("Venom-Laced Pants")) {
            poisonDamage = 5;
            poisonTurnsRemaining = 3;
            cout << "[Venom-Laced Pants] Poison applied!" << endl;
            spawnFloatingText("Venom Pants!", Color(100, 255, 100), false);
        }
    }
}

double EquipmentEffectsSystem::getConditionalAttackBonus(double currentHP, double maxHP) {
    double bonus = 0.0;
    double hpPercent = (currentHP / maxHP) * 100.0;

    // COMMON ITEMS
    if (hpPercent < 50.0) {
        // Rogue's Garb - +15 Attack below 50% HP
        if (hasItem("Rogue's Garb")) {
            bonus += 15.0;
        }
        // Nightblade Pants - +5 Attack below 50% HP
        if (hasItem("Nightblade Pants")) {
            bonus += 5.0;
        }
        // Shadowstep Boots - +15 Attack below 50% HP
        if (hasItem("Shadowstep Boots")) {
            bonus += 15.0;
        }
    }

    // RARE ITEMS
    if (hpPercent < 50.0) {
        // Nightfall Vest - +10 Attack below 50% HP
        if (hasItem("Nightfall Vest")) {
            bonus += 10.0;
        }
    }

    if (hpPercent < 30.0) {
        // Dark Pendant - +20 Attack below 30% HP
        if (hasItem("Dark Pendant")) {
            bonus += 20.0;
        }
        // Bloodstone - +10 Attack below 30% HP
        if (hasItem("Bloodstone")) {
            bonus += 10.0;
        }
    }

    return bonus;
}

double EquipmentEffectsSystem::getConditionalDexterityBonus(double currentHP, double maxHP) {
    double bonus = permanentDexterityBonus;
    double hpPercent = (currentHP / maxHP) * 100.0;

    // COMMON ITEMS
    if (hpPercent < 50.0) {
        // Assassin's Helm - +5 Dex below 50% HP
        if (hasItem("Assassin's Helm")) {
            bonus += 5.0;
        }
    }

    // RARE ITEMS
    if (hpPercent < 50.0) {
        // Nightstalker's Cowl - +10 Dex below 50% HP
        if (hasItem("Nightstalker's Cowl")) {
            bonus += 10.0;
        }
    }

    // Death's Step - +5% crit chance (handled separately)

    return bonus;
}

void EquipmentEffectsSystem::damageShield(double& incomingDamage) {
    if (currentShield > 0.0) {
        if (incomingDamage <= currentShield) {
            currentShield -= incomingDamage;
            cout << "[Shield] Absorbed " << incomingDamage << " (remaining: " << currentShield << ")" << endl;
            spawnFloatingText("Shield!", Color(150, 150, 200), true);
            incomingDamage = 0.0;
        }
        else {
            double overflow = incomingDamage - currentShield;
            cout << "[Shield] Broken! Absorbed " << currentShield << endl;
            spawnFloatingText("Shield Broken!", Color(255, 150, 150), true);
            currentShield = 0.0;
            incomingDamage = overflow;
        }
    }
}

void EquipmentEffectsSystem::setFloatingTextCallback(FloatingTextCallback callback) {
    floatingTextCallback = callback;
}

void EquipmentEffectsSystem::setCharacterPositions(Vector2f playerPos, Vector2f enemyPos) {
    playerPosition = playerPos;
    enemyPosition = enemyPos;
}

void EquipmentEffectsSystem::spawnFloatingText(const string& text, Color color, bool aroundPlayer) {
    if (floatingTextCallback) {
        Vector2f position = aroundPlayer ? playerPosition : enemyPosition;
        floatingTextCallback(text, position, color, aroundPlayer);
    }
}

void EquipmentEffectsSystem::healPlayer(double& currentHealth, double healAmount, double maxHealth, const string& source) {
    double oldHealth = currentHealth;
    currentHealth += healAmount;

    if (currentHealth > maxHealth) {
        currentHealth = maxHealth;
    }

    cout << "[" << source << "] Healed " << (currentHealth - oldHealth) << " HP" << endl;
}