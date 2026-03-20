#include "MageEquipmentEffectsSystem.h"
#include "CharacterSheet.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;
using namespace sf;

MageEquipmentEffectsSystem::MageEquipmentEffectsSystem()
    : playerSheet(nullptr), attackCount(0), playerAttackCount(0), enemyAttackCount(0),
    playerCritCount(0), spellCount(0), firstPlayerAttackUsed(false), firstEnemyAttackProcessed(false),
    secondEnemyAttackDodged(false), thirdEnemyAttackDodged(false), lowHPShieldTriggered(false),
    lowHPShieldTriggered25(false), permanentAttackBonus(0.0), permanentIntelBonus(0.0),
    burnDamage(0), burnTurnsRemaining(0), currentShield(0.0), bonusArmorPen(0.0),
    dungeonWinCount(0), orbDevastationCounter(0), fireImpStacks(0), manaWyrmStacks(0),
    spellweaverStacks(0), arcaneStaffStacks(0), enemyHasOpenWound(false),
    floatingTextCallback(nullptr), playerPosition(0.f, 0.f), enemyPosition(0.f, 0.f) {
}

MageEquipmentEffectsSystem::~MageEquipmentEffectsSystem() {}

void MageEquipmentEffectsSystem::initialize(CharacterSheet* sheet) {
    playerSheet = sheet;
    if (!playerSheet) return;

    equippedWeapon = playerSheet->getEquippedItem(EquipmentSlot::Weapon);
    equippedHelmet = playerSheet->getEquippedItem(EquipmentSlot::Helmet);
    equippedChestplate = playerSheet->getEquippedItem(EquipmentSlot::Chestplate);
    equippedPants = playerSheet->getEquippedItem(EquipmentSlot::Pants);
    equippedBoots = playerSheet->getEquippedItem(EquipmentSlot::Boots);
    equippedTalisman = playerSheet->getEquippedItem(EquipmentSlot::Talisman);
    equippedCompanion = playerSheet->getEquippedItem(EquipmentSlot::Companion);

    bonusArmorPen = 0.0;
    if (hasItem("Boots of Witch")) bonusArmorPen += 8.0;
    if (hasItem("Boots of the Pyromancer")) bonusArmorPen += 5.0;
    if (hasItem("Arcane Staff")) bonusArmorPen += 7.0;
    if (hasItem("Mage's Striders")) bonusArmorPen += 10.0;
    if (hasItem("Runic Pendant")) bonusArmorPen += 12.0;
    reset();
}

void MageEquipmentEffectsSystem::setEquippedItem(const string& slotName, const string& itemName) {
    if (slotName == "Weapon") equippedWeapon = itemName;
    else if (slotName == "Helmet") equippedHelmet = itemName;
    else if (slotName == "Chestplate") equippedChestplate = itemName;
    else if (slotName == "Pants") equippedPants = itemName;
    else if (slotName == "Boots") equippedBoots = itemName;
    else if (slotName == "Talisman") equippedTalisman = itemName;
    else if (slotName == "Companion") equippedCompanion = itemName;
}

void MageEquipmentEffectsSystem::reset() {
    attackCount = playerAttackCount = enemyAttackCount = playerCritCount = spellCount = 0;
    firstPlayerAttackUsed = firstEnemyAttackProcessed = secondEnemyAttackDodged = false;
    thirdEnemyAttackDodged = lowHPShieldTriggered = lowHPShieldTriggered25 = false;
    permanentAttackBonus = permanentIntelBonus = currentShield = 0.0;
    burnDamage = burnTurnsRemaining = 0;
    orbDevastationCounter = fireImpStacks = manaWyrmStacks = spellweaverStacks = arcaneStaffStacks = 0;
    enemyHasOpenWound = false;
}

bool MageEquipmentEffectsSystem::hasItem(const string& itemName) const {
    return equippedWeapon == itemName || equippedHelmet == itemName || equippedChestplate == itemName ||
        equippedPants == itemName || equippedBoots == itemName || equippedTalisman == itemName || equippedCompanion == itemName;
}

void MageEquipmentEffectsSystem::addPermanentAttackBonus(double bonus) { permanentAttackBonus += bonus; }
void MageEquipmentEffectsSystem::addPermanentIntelBonus(double bonus) { permanentIntelBonus += bonus; }

void MageEquipmentEffectsSystem::onCombatStart(double& playerHealth, double& shield, double& enemyHealth, bool isArena) {
    // Shields - Common
    if (hasItem("Big CRYSTAL")) { currentShield += 20.0; spawnFloatingText("+20 Shield", Color(150, 150, 200), true); }
    if (hasItem("Arcane Pants")) { currentShield += 20.0; spawnFloatingText("+20 Shield", Color(150, 150, 200), true); }
    // Shields - Rare
    if (hasItem("Arcane Vestment")) { currentShield += 25.0; spawnFloatingText("+25 Shield", Color(150, 150, 200), true); }
    if (hasItem("Arcane Greaves")) { currentShield += 15.0; spawnFloatingText("+15 Shield", Color(150, 150, 200), true); }
    if (hasItem("Arcane Reliquary")) { currentShield += 10.0; spawnFloatingText("+10 Shield", Color(150, 150, 200), true); }
    if (hasItem("Arcane Golem")) { currentShield += 25.0; permanentIntelBonus += 10.0; spawnFloatingText("+25 Shield", Color(150, 150, 200), true); }
    if (hasItem("Crystal Guardian")) { currentShield += 30.0; spawnFloatingText("+30 Shield", Color(150, 150, 200), true); }

    // Start damage - Common
    if (hasItem("Meteor")) { enemyHealth -= 25.0; spawnFloatingText("-25 HP", Color(255, 100, 50), false); }
    if (hasItem("STAR") && isArena) { enemyHealth -= 50.0; spawnFloatingText("-50 HP", Color(255, 200, 50), false); }
    if (hasItem("Boots of the Pyromancer")) { enemyHealth -= 5.0; spawnFloatingText("-5 HP", Color(255, 100, 50), false); }
    // Start damage - Rare
    if (hasItem("Inferno Wand")) { enemyHealth -= 45.0; spawnFloatingText("-45 HP", Color(255, 100, 50), false); }
    if (hasItem("Wand of Devastation")) { enemyHealth -= 25.0; spawnFloatingText("-25 HP", Color(255, 100, 50), false); }
    if (hasItem("Flame Orb")) { enemyHealth -= 8.0; spawnFloatingText("-8 HP", Color(255, 100, 50), false); }
    if (hasItem("Flamestep Boots")) { enemyHealth -= 10.0; spawnFloatingText("-10 HP", Color(255, 100, 50), false); }
    if (hasItem("Phoenix Hatchling")) { enemyHealth -= 20.0; spawnFloatingText("-20 HP", Color(255, 100, 50), false); }

    if (hasItem("Mana Sprite")) permanentIntelBonus += 10.0;
    shield = currentShield;
}

void MageEquipmentEffectsSystem::onTurnStart(double& playerHealth, double maxHealth) {
    attackCount++;
    if (hasItem("Crystal Owl")) { healPlayer(playerHealth, 5.0, maxHealth, "Crystal Owl"); spawnFloatingText("+5 HP", Color(50, 255, 50), true); }
    if (hasItem("Phoenix Hatchling")) { healPlayer(playerHealth, 5.0, maxHealth, "Phoenix"); spawnFloatingText("+5 HP", Color(50, 255, 50), true); }
    if (hasItem("Sorcerer's Trousers")) { healPlayer(playerHealth, 3.0, maxHealth, "Sorcerer"); spawnFloatingText("+3 HP", Color(50, 255, 50), true); }
    if (hasItem("Spellfire Slippers")) addPermanentAttackBonus(5.0);
}

void MageEquipmentEffectsSystem::onPlayerAttackStart(double& attackBonus, bool isArena) {
    playerAttackCount++;
    spellCount++;

    // First spell bonuses
    if (spellCount == 1) {
        if (hasItem("Wizard's Hat")) attackBonus += 5.0;
        if (hasItem("Arcane Slippers")) attackBonus += 10.0;
        if (hasItem("Boots of the Archmage")) attackBonus += 15.0;
    }

    // Per-spell stacking
    if (hasItem("Dark Ring") && isArena) attackBonus += min(spellCount * 5.0, 25.0);
    if (hasItem("Mystic Legwraps")) addPermanentAttackBonus(3.0);
    if (hasItem("Fire Imp") && fireImpStacks < 5) { fireImpStacks++; addPermanentAttackBonus(5.0); }
    if (hasItem("Arcane Staff") && arcaneStaffStacks < 10) { arcaneStaffStacks++; addPermanentAttackBonus(5.0); }
    if (hasItem("Spellweaver's Robe") && spellweaverStacks < 5) { spellweaverStacks++; addPermanentAttackBonus(8.0); }
    if (hasItem("Mana Wyrm") && manaWyrmStacks < 5) { manaWyrmStacks++; addPermanentAttackBonus(8.0); }
    if (hasItem("Archmage's Hood")) addPermanentIntelBonus(5.0);

    // Orb of Devastation - every 3rd attack +50 dmg
    if (hasItem("Orb of Devastation")) {
        orbDevastationCounter++;
        if (orbDevastationCounter >= 3) { attackBonus += 50.0; orbDevastationCounter = 0; spawnFloatingText("+50 DMG!", Color(255, 50, 50), true); }
    }

    attackBonus += permanentAttackBonus;
}

void MageEquipmentEffectsSystem::onPlayerAttackHit(double& playerHealth, int& bonusSilver, double maxHealth, double& enemyHealth) {
    // Healing
    if (hasItem("Book of Healing")) { healPlayer(playerHealth, 10.0, maxHealth, "Book"); spawnFloatingText("+10 HP", Color(50, 255, 50), true); }
    if (hasItem("Fox")) { healPlayer(playerHealth, 3.0, maxHealth, "Fox"); spawnFloatingText("+3 HP", Color(50, 255, 50), true); }
    if (hasItem("Scholar's Trousers")) healPlayer(playerHealth, 2.0, maxHealth, "Scholar");
    if (hasItem("Sorcerer's Mantle")) healPlayer(playerHealth, 5.0, maxHealth, "Sorcerer");
    if (hasItem("Crystal of Vitality")) healPlayer(playerHealth, maxHealth * 0.025, maxHealth, "Vitality");

    // Stacking
    if (hasItem("Arcane Hood")) addPermanentIntelBonus(5.0);
    if (hasItem("Arcane Vestments")) addPermanentAttackBonus(5.0);

    // Burn bonus
    if (hasItem("Book of Secrets") && burnTurnsRemaining > 0) enemyHealth -= 5.0;

    // Open Wound
    if (hasItem("Bleeding Orb") && !enemyHasOpenWound) { enemyHasOpenWound = true; spawnFloatingText("Open Wound!", Color(255, 50, 50), false); }

    // Fire Elemental burn
    if (hasItem("Fire Elemental") && burnTurnsRemaining == 0) { burnDamage = 3; burnTurnsRemaining = 2; }
}

void MageEquipmentEffectsSystem::onPlayerCrit(double& attackBonus, double& playerHealth, double maxHealth) {
    playerCritCount++;
    if (hasItem("Scholar's Cap")) addPermanentAttackBonus(5.0);
    if (hasItem("Mage's Circlet")) healPlayer(playerHealth, 5.0, maxHealth, "Circlet");
    if (hasItem("Lost Ring")) attackBonus += 10.0;
    if (hasItem("Mindfire Circlet")) attackBonus += 15.0;
    if (hasItem("Runic Legwraps")) addPermanentAttackBonus(5.0);
}

void MageEquipmentEffectsSystem::onPlayerTakeDamage(double& intelBonus, bool& shouldBlock) {
    intelBonus += permanentIntelBonus;
}

void MageEquipmentEffectsSystem::onEnemyAttackStart(bool& enemyShouldMiss, double& enemyArmorReduction) {
    enemyAttackCount++;

    // First attack blocks
    if (enemyAttackCount == 1 && !firstEnemyAttackProcessed) {
        if (hasItem("GOLDEN ARMOR") || hasItem("Golden Aegis")) {
            enemyShouldMiss = true;
            firstEnemyAttackProcessed = true;
            spawnFloatingText("Blocked!", Color(255, 215, 0), true);
        }
    }

    // Second attack block
    if (hasItem("Enchanted Hood") && enemyAttackCount == 2 && !secondEnemyAttackDodged) {
        enemyShouldMiss = true;
        secondEnemyAttackDodged = true;
    }

    // Third attack block
    if (hasItem("Veil of Sorcery") && enemyAttackCount == 3 && !thirdEnemyAttackDodged) {
        enemyShouldMiss = true;
        thirdEnemyAttackDodged = true;
    }

    // Arcane Familiar - 5% block
    if (hasItem("Arcane Familiar") && (rand() % 100) < 5) enemyShouldMiss = true;

    enemyArmorReduction += bonusArmorPen;
}

void MageEquipmentEffectsSystem::applyBurnDamage(double& enemyHealth) {
    if (burnTurnsRemaining > 0) {
        enemyHealth -= burnDamage;
        burnTurnsRemaining--;
        spawnFloatingText("Burn: -" + to_string(burnDamage), Color(255, 100, 50), false);
    }

    if (burnTurnsRemaining == 0) {
        if (hasItem("Pyromancer's Cloak")) { burnDamage = 3; burnTurnsRemaining = 2; }
        else if (hasItem("Pyromancer's Leggings")) { burnDamage = 2; burnTurnsRemaining = 2; }
        else if (hasItem("Robes of the Inferno")) { burnDamage = 5; burnTurnsRemaining = 2; }
        else if (hasItem("Flamecaster Leggings")) { burnDamage = 4; burnTurnsRemaining = 2; }
    }
}

void MageEquipmentEffectsSystem::onCombatEnd(bool playerWon, int& bonusXP, int& bonusSilver, bool isArena, int dungeonCount) {
    if (!playerWon) return;
    if (!isArena) dungeonWinCount++;

    if (hasItem("Fox") && !isArena) bonusXP += 80;
    if (hasItem("Ancient Coin") && !isArena && dungeonWinCount % 3 == 0) bonusSilver += 30;
}

double MageEquipmentEffectsSystem::getConditionalAttackBonus(double currentHP, double maxHP, bool isArena) {
    double bonus = 0.0;
    double hpPercent = (currentHP / maxHP) * 100.0;
    if (hpPercent < 50.0) {
        if (hasItem("Mana Flow Pants")) bonus += 5.0;
        if (hasItem("Crown of Flames")) bonus += 10.0;
        if (hasItem("Mana-Woven Pants")) bonus += 5.0;
    }
    return bonus;
}

void MageEquipmentEffectsSystem::checkLowHPShield(double currentHP, double maxHP) {
    double hpPercent = (currentHP / maxHP) * 100.0;
    if (hasItem("Robe of Shielding") && hpPercent < 50.0 && !lowHPShieldTriggered) {
        lowHPShieldTriggered = true;
        currentShield += 15.0;
    }
}

void MageEquipmentEffectsSystem::damageShield(double& incomingDamage) {
    if (currentShield > 0.0) {
        if (incomingDamage <= currentShield) { currentShield -= incomingDamage; incomingDamage = 0.0; }
        else { incomingDamage -= currentShield; currentShield = 0.0; }
    }
}

void MageEquipmentEffectsSystem::addShield(double amount) { currentShield += amount; }

void MageEquipmentEffectsSystem::setFloatingTextCallback(FloatingTextCallback callback) { floatingTextCallback = callback; }
void MageEquipmentEffectsSystem::setCharacterPositions(Vector2f playerPos, Vector2f enemyPos) { playerPosition = playerPos; enemyPosition = enemyPos; }

void MageEquipmentEffectsSystem::spawnFloatingText(const string& text, Color color, bool aroundPlayer) {
    if (floatingTextCallback) floatingTextCallback(text, aroundPlayer ? playerPosition : enemyPosition, color, aroundPlayer);
}

void MageEquipmentEffectsSystem::healPlayer(double& currentHealth, double healAmount, double maxHealth, const string& source) {
    currentHealth = min(currentHealth + healAmount, maxHealth);
}