#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class CharacterSheet;
enum class EquipmentSlot;

using FloatingTextCallback = function<void(const string&, Vector2f, Color, bool)>;

class MageEquipmentEffectsSystem {
public:
    MageEquipmentEffectsSystem();
    ~MageEquipmentEffectsSystem();

    void initialize(CharacterSheet* playerSheet);
    void setEquippedItem(const string& slotName, const string& itemName);
    void setFloatingTextCallback(FloatingTextCallback callback);
    void setCharacterPositions(Vector2f playerPos, Vector2f enemyPos);

    // Combat triggers
    void onCombatStart(double& playerHealth, double& shield, double& enemyHealth, bool isArena);
    void onTurnStart(double& playerHealth, double maxHealth);
    void onPlayerAttackStart(double& attackBonus, bool isArena);
    void onPlayerAttackHit(double& playerHealth, int& bonusSilver, double maxHealth, double& enemyHealth);
    void onPlayerCrit(double& attackBonus, double& playerHealth, double maxHealth);
    void onPlayerTakeDamage(double& intelBonus, bool& shouldBlock);
    void onEnemyAttackStart(bool& enemyShouldMiss, double& enemyArmorReduction);
    void onCombatEnd(bool playerWon, int& bonusXP, int& bonusSilver, bool isArena, int dungeonCount);

    void applyBurnDamage(double& enemyHealth);
    double getConditionalAttackBonus(double currentHP, double maxHP, bool isArena);
    void checkLowHPShield(double currentHP, double maxHP);

    double getShield() const { return currentShield; }
    void damageShield(double& incomingDamage);
    void addShield(double amount);
    bool hasOpenWound() const { return enemyHasOpenWound; }

    void reset();
    bool hasItem(const string& itemName) const;

private:
    CharacterSheet* playerSheet;

    string equippedWeapon;
    string equippedHelmet;
    string equippedChestplate;
    string equippedPants;
    string equippedBoots;
    string equippedTalisman;
    string equippedCompanion;

    int attackCount;
    int playerAttackCount;
    int enemyAttackCount;
    int playerCritCount;
    int spellCount;
    bool firstPlayerAttackUsed;
    bool firstEnemyAttackProcessed;
    bool secondEnemyAttackDodged;
    bool thirdEnemyAttackDodged;
    bool lowHPShieldTriggered;
    bool lowHPShieldTriggered25;

    double permanentAttackBonus;
    double permanentIntelBonus;

    int burnDamage;
    int burnTurnsRemaining;

    double currentShield;
    double bonusArmorPen;
    int dungeonWinCount;

    int orbDevastationCounter;
    int fireImpStacks;
    int manaWyrmStacks;
    int spellweaverStacks;
    int arcaneStaffStacks;
    bool enemyHasOpenWound;

    FloatingTextCallback floatingTextCallback;
    Vector2f playerPosition;
    Vector2f enemyPosition;

    void addPermanentAttackBonus(double bonus);
    void addPermanentIntelBonus(double bonus);
    void spawnFloatingText(const string& text, Color color, bool aroundPlayer);
    void healPlayer(double& currentHealth, double healAmount, double maxHealth, const string& source);
};