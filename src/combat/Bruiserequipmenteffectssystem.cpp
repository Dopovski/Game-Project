#include "BruiserEquipmentEffectsSystem.h"
#include "CharacterSheet.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;
using namespace sf;

BruiserEquipmentEffectsSystem::BruiserEquipmentEffectsSystem()
    : playerSheet(nullptr), attackCount(0), playerAttackCount(0), enemyAttackCount(0),
    playerCritCount(0), hitsTaken(0), firstPlayerAttackUsed(false), firstEnemyAttackProcessed(false),
    lowHPShieldTriggered(false), permanentAttackBonus(0.0), currentShield(0.0),
    pickaxeStacks(0), armorShredStacks(0), berserkerHarnessStacks(0), berserkerWolfStacks(0),
    dungeonWinCount(0), titanAxeCounter(0), enemyHasOpenWound(false),
    floatingTextCallback(nullptr), playerPosition(0.f, 0.f), enemyPosition(0.f, 0.f) {
}

BruiserEquipmentEffectsSystem::~BruiserEquipmentEffectsSystem() {}

void BruiserEquipmentEffectsSystem::initialize(CharacterSheet* sheet) {
    playerSheet = sheet;
    if (!playerSheet) return;
    equippedWeapon = playerSheet->getEquippedItem(EquipmentSlot::Weapon);
    equippedHelmet = playerSheet->getEquippedItem(EquipmentSlot::Helmet);
    equippedChestplate = playerSheet->getEquippedItem(EquipmentSlot::Chestplate);
    equippedPants = playerSheet->getEquippedItem(EquipmentSlot::Pants);
    equippedBoots = playerSheet->getEquippedItem(EquipmentSlot::Boots);
    equippedTalisman = playerSheet->getEquippedItem(EquipmentSlot::Talisman);
    equippedCompanion = playerSheet->getEquippedItem(EquipmentSlot::Companion);
    reset();
}

void BruiserEquipmentEffectsSystem::setEquippedItem(const string& slotName, const string& itemName) {
    if (slotName == "Weapon") equippedWeapon = itemName;
    else if (slotName == "Helmet") equippedHelmet = itemName;
    else if (slotName == "Chestplate") equippedChestplate = itemName;
    else if (slotName == "Pants") equippedPants = itemName;
    else if (slotName == "Boots") equippedBoots = itemName;
    else if (slotName == "Talisman") equippedTalisman = itemName;
    else if (slotName == "Companion") equippedCompanion = itemName;
}

void BruiserEquipmentEffectsSystem::reset() {
    attackCount = playerAttackCount = enemyAttackCount = playerCritCount = hitsTaken = 0;
    firstPlayerAttackUsed = firstEnemyAttackProcessed = lowHPShieldTriggered = false;
    permanentAttackBonus = currentShield = 0.0;
    pickaxeStacks = armorShredStacks = berserkerHarnessStacks = berserkerWolfStacks = titanAxeCounter = 0;
    enemyHasOpenWound = false;
}

bool BruiserEquipmentEffectsSystem::hasItem(const string& itemName) const {
    return equippedWeapon == itemName || equippedHelmet == itemName || equippedChestplate == itemName ||
        equippedPants == itemName || equippedBoots == itemName || equippedTalisman == itemName || equippedCompanion == itemName;
}

void BruiserEquipmentEffectsSystem::addPermanentAttackBonus(double bonus) { permanentAttackBonus += bonus; }

void BruiserEquipmentEffectsSystem::onCombatStart(double& playerHealth, double& shield, double& enemyAttack, bool isArena) {
    if (hasItem("Enchanted Shield")) currentShield += 35.0;
    if (hasItem("Warrior's Cap")) currentShield += 10.0;
    if (hasItem("Enchanted Armor")) currentShield += 25.0;
    if (hasItem("Stone Greaves")) currentShield += 5.0;
    if (hasItem("Boulder Boots")) currentShield += 10.0;
    if (hasItem("Iron Golem")) currentShield += 20.0;
    if (hasItem("Fortress Helm")) currentShield += 20.0;
    if (hasItem("Fortress Greaves")) currentShield += 10.0;
    if (hasItem("Warlord's Stompers")) currentShield += 15.0;
    if (hasItem("War Bear")) currentShield += 40.0;
    if (hasItem("Iron Guardian")) currentShield += 55.0;
    if (hasItem("Stone Tortoise")) currentShield += 30.0;
    if (hasItem("Mountain Golem")) currentShield += 25.0;
    if (hasItem("Bruiser Shield Basher")) currentShield += 10.0;
    if (hasItem("Horn for Battle") && isArena) { enemyAttack -= 15.0; if (enemyAttack < 1.0) enemyAttack = 1.0; }
    shield = currentShield;
}

void BruiserEquipmentEffectsSystem::onTurnStart(double& playerHealth, double maxHealth) {
    attackCount++;
    if (hasItem("Titan's Legguards")) healPlayer(playerHealth, 5.0, maxHealth, "Titan");
    if (hasItem("Titan's Greaves")) healPlayer(playerHealth, 5.0, maxHealth, "Titan");
    if (hasItem("Titan's Treads")) healPlayer(playerHealth, 3.0, maxHealth, "Titan");
    if (hasItem("Stone Tortoise")) healPlayer(playerHealth, 5.0, maxHealth, "Stone");
    if (hasItem("Titan's Heart")) currentShield += 5.0;
}

void BruiserEquipmentEffectsSystem::onPlayerAttackStart(double& attackBonus, bool& forceCrit) {
    playerAttackCount++;
    if (!firstPlayerAttackUsed && (hasItem("Rabbit") || hasItem("Berserker Wolf"))) { forceCrit = true; firstPlayerAttackUsed = true; }
    if (hasItem("Pickaxe") && pickaxeStacks < 5) { pickaxeStacks++; addPermanentAttackBonus(5.0); }
    if (hasItem("Titan's Axe")) { titanAxeCounter++; if (titanAxeCounter >= 5) { forceCrit = true; titanAxeCounter = 0; } }
    attackBonus += permanentAttackBonus;
}

void BruiserEquipmentEffectsSystem::onPlayerAttackHit(double& playerHealth, int& bonusSilver, double maxHealth, double& enemyHealth) {
    if (hasItem("Battle Shield")) healPlayer(playerHealth, 4.0, maxHealth, "Battle");
    if (hasItem("Mallet")) healPlayer(playerHealth, 8.0, maxHealth, "Mallet");
    if (hasItem("Light Vest")) healPlayer(playerHealth, 3.0, maxHealth, "Light");
    if (hasItem("Guardian Plate")) healPlayer(playerHealth, 3.0, maxHealth, "Guardian");
    if (hasItem("Vanguard Pants")) healPlayer(playerHealth, 2.0, maxHealth, "Vanguard");
    if (hasItem("Map")) healPlayer(playerHealth, 2.5, maxHealth, "Map");
    if (hasItem("Chain")) healPlayer(playerHealth, 10.0, maxHealth, "Chain");
    if (hasItem("Turtle")) { healPlayer(playerHealth, 3.0, maxHealth, "Turtle"); currentShield += 2.0; }
    if (hasItem("Colossus Hammer")) healPlayer(playerHealth, 5.0, maxHealth, "Colossus");
    if (hasItem("Warlord's Mace")) healPlayer(playerHealth, 3.0, maxHealth, "Warlord");
    if (hasItem("Ironclad Vest")) healPlayer(playerHealth, 5.0, maxHealth, "Ironclad");
    if (hasItem("Warlord's Pants")) healPlayer(playerHealth, 3.0, maxHealth, "Warlord");
    if (hasItem("War Bear")) healPlayer(playerHealth, 5.0, maxHealth, "Bear");
    if (hasItem("Heart of the Mountain")) healPlayer(playerHealth, 10.0, maxHealth, "Heart");
    if (hasItem("Stone of Vitality")) healPlayer(playerHealth, maxHealth * 0.03, maxHealth, "Stone");
    if (hasItem("Bone Crusher") && armorShredStacks < 5) armorShredStacks++;
    if ((hasItem("Spiked Bulwark") || hasItem("Warlord's Emblem")) && !enemyHasOpenWound) enemyHasOpenWound = true;
}

void BruiserEquipmentEffectsSystem::onPlayerCrit(double& attackBonus, double& playerHealth, double maxHealth) { playerCritCount++; }

void BruiserEquipmentEffectsSystem::onPlayerTakeDamage(double& damageReduction, bool& shouldBlock, double& incomingDamage, double currentHP, double maxHP) {
    hitsTaken++;
    double hpPercent = (currentHP / maxHP) * 100.0;
    if (hasItem("Battle Helm")) healPlayer(currentHP, 3.0, maxHP, "Battle");
    if (hasItem("Titan's Visage")) healPlayer(currentHP, 5.0, maxHP, "Titan");
    if (hasItem("Berserker's Harness") && berserkerHarnessStacks < 5) { berserkerHarnessStacks++; addPermanentAttackBonus(5.0); }
    if (hasItem("Berserker Wolf") && berserkerWolfStacks < 3) { berserkerWolfStacks++; addPermanentAttackBonus(10.0); }
    if (hasItem("Hedgehog") && hpPercent < 30.0 && !lowHPShieldTriggered) { lowHPShieldTriggered = true; currentShield += 40.0; }
    if (hasItem("Juggernaut Leggings")) { incomingDamage -= 2.0; if (incomingDamage < 1.0) incomingDamage = 1.0; }
    damageReduction = getDamageReductionPercent();
}

void BruiserEquipmentEffectsSystem::onEnemyAttackStart(bool& enemyShouldMiss, double& flatDamageReduction) {
    enemyAttackCount++;
    if (enemyAttackCount == 1 && !firstEnemyAttackProcessed) {
        firstEnemyAttackProcessed = true;
        if (hasItem("Guardian Helm")) enemyShouldMiss = true;
        if (hasItem("Iron Helm")) flatDamageReduction += 5.0;
        if (hasItem("Juggernaut Armor")) flatDamageReduction += 5.0;
        if (hasItem("Juggernaut Helm")) flatDamageReduction += 10.0;
        if (hasItem("Warlord's Mail")) flatDamageReduction += 10.0;
    }
}

void BruiserEquipmentEffectsSystem::onCombatEnd(bool playerWon, int& bonusXP, int& bonusSilver, bool isArena, int dungeonCount) {
    if (!playerWon) return;
    if (!isArena) dungeonWinCount++;
    if (hasItem("Turtle") && !isArena) bonusXP += 50;
    if (hasItem("Scycle") && !isArena) bonusSilver += 20;
    if (hasItem("Bruiser Merchant Rat") && !isArena) bonusSilver += 25;
    if (hasItem("Money Shield") && !isArena && dungeonWinCount % 5 == 0) bonusSilver += 65;
    if (hasItem("Stone Tortoise") && !isArena) bonusXP += 75;
}

double BruiserEquipmentEffectsSystem::getConditionalAttackBonus(double currentHP, double maxHP) {
    double bonus = 0.0, hpPercent = (currentHP / maxHP) * 100.0;
    if (hpPercent < 50.0) { if (hasItem("Warlord's Crown")) bonus += 5.0; if (hasItem("Berserker's Legguards")) bonus += 5.0; }
    if (hpPercent < 40.0 && hasItem("Berserker's Boots")) bonus += 10.0;
    if (hpPercent < 30.0) { if (hasItem("Berserker's Mask")) bonus += 10.0; if (hasItem("Berserker's Totem")) bonus += 15.0; }
    return bonus;
}

double BruiserEquipmentEffectsSystem::getDamageReductionPercent() const {
    double r = 0.0;
    if (hasItem("Iron Legguards")) r += 0.03;
    if (hasItem("Boots of Steel")) r += 0.10;
    if (hasItem("Iron Treads")) r += 0.05;
    if (hasItem("Juggernaut Pants")) r += 0.05;
    if (hasItem("Juggernaut Boots")) r += 0.08;
    if (hasItem("Mountain Golem")) r += 0.05;
    return r;
}

void BruiserEquipmentEffectsSystem::damageShield(double& incomingDamage) {
    if (currentShield > 0.0) {
        if (incomingDamage <= currentShield) { currentShield -= incomingDamage; incomingDamage = 0.0; }
        else { incomingDamage -= currentShield; currentShield = 0.0; }
    }
}

void BruiserEquipmentEffectsSystem::addShield(double amount) { currentShield += amount; }
void BruiserEquipmentEffectsSystem::setFloatingTextCallback(FloatingTextCallback callback) { floatingTextCallback = callback; }
void BruiserEquipmentEffectsSystem::setCharacterPositions(Vector2f playerPos, Vector2f enemyPos) { playerPosition = playerPos; enemyPosition = enemyPos; }

void BruiserEquipmentEffectsSystem::spawnFloatingText(const string& text, Color color, bool aroundPlayer) {
    if (floatingTextCallback) floatingTextCallback(text, aroundPlayer ? playerPosition : enemyPosition, color, aroundPlayer);
}

void BruiserEquipmentEffectsSystem::healPlayer(double& currentHealth, double healAmount, double maxHealth, const string& source) {
    currentHealth = min(currentHealth + healAmount, maxHealth);
}