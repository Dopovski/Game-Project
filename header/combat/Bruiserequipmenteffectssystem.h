#pragma once
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class CharacterSheet;
enum class EquipmentSlot;

using FloatingTextCallback = function<void(const string&, Vector2f, Color, bool)>;

class BruiserEquipmentEffectsSystem {
public:
    BruiserEquipmentEffectsSystem();
    ~BruiserEquipmentEffectsSystem();

    void initialize(CharacterSheet* playerSheet);
    void setEquippedItem(const string& slotName, const string& itemName);
    void setFloatingTextCallback(FloatingTextCallback callback);
    void setCharacterPositions(Vector2f playerPos, Vector2f enemyPos);

    void onCombatStart(double& playerHealth, double& shield, double& enemyAttack, bool isArena);
    void onTurnStart(double& playerHealth, double maxHealth);
    void onPlayerAttackStart(double& attackBonus, bool& forceCrit);
    void onPlayerAttackHit(double& playerHealth, int& bonusSilver, double maxHealth, double& enemyHealth);
    void onPlayerCrit(double& attackBonus, double& playerHealth, double maxHealth);
    void onPlayerTakeDamage(double& damageReduction, bool& shouldBlock, double& incomingDamage, double currentHP, double maxHP);
    void onEnemyAttackStart(bool& enemyShouldMiss, double& flatDamageReduction);
    void onCombatEnd(bool playerWon, int& bonusXP, int& bonusSilver, bool isArena, int dungeonCount);

    double getConditionalAttackBonus(double currentHP, double maxHP);
    double getDamageReductionPercent() const;

    double getShield() const { return currentShield; }
    void damageShield(double& incomingDamage);
    void addShield(double amount);
    bool hasOpenWound() const { return enemyHasOpenWound; }

    void reset();
    bool hasItem(const string& itemName) const;

private:
    CharacterSheet* playerSheet;

    string equippedWeapon, equippedHelmet, equippedChestplate;
    string equippedPants, equippedBoots, equippedTalisman, equippedCompanion;

    int attackCount, playerAttackCount, enemyAttackCount, playerCritCount, hitsTaken;
    bool firstPlayerAttackUsed, firstEnemyAttackProcessed, lowHPShieldTriggered;

    double permanentAttackBonus, currentShield;
    int pickaxeStacks, armorShredStacks, berserkerHarnessStacks, berserkerWolfStacks;
    int dungeonWinCount, titanAxeCounter;
    bool enemyHasOpenWound;

    FloatingTextCallback floatingTextCallback;
    Vector2f playerPosition, enemyPosition;

    void addPermanentAttackBonus(double bonus);
    void spawnFloatingText(const string& text, Color color, bool aroundPlayer);
    void healPlayer(double& currentHealth, double healAmount, double maxHealth, const string& source);
};