#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

// Forward declarations
class CharacterSheet;
enum class EquipmentSlot;

// Callback type for spawning floating text
using FloatingTextCallback = function<void(const string&, Vector2f, Color, bool)>;

class EquipmentEffectsSystem {
public:
    EquipmentEffectsSystem();
    ~EquipmentEffectsSystem();

    // Initialize with player's equipped items
    void initialize(CharacterSheet* playerSheet);

    // Manually set equipped items
    void setEquippedItem(const string& slotName, const string& itemName);

    // Set callback for spawning floating text
    void setFloatingTextCallback(FloatingTextCallback callback);

    // Set character positions for floating text
    void setCharacterPositions(Vector2f playerPos, Vector2f enemyPos);

    // Combat lifecycle triggers
    void onCombatStart(double& playerHealth, double& shield);
    void onTurnStart();
    void onPlayerAttackStart(double& attackBonus, bool& forceDoubleTap, bool& forceCrit);
    void onPlayerAttackHit(double& playerHealth, int& bonusSilver, double maxHealth);
    void onPlayerCrit(double& attackBonus, double& playerHealth, double maxHealth);
    void onPlayerDodge(double& dexterityBonus);
    void onPlayerTakeDamage(double& dexterityBonus, bool& shouldDodge);
    void onEnemyAttackStart(bool& enemyShouldMiss, double& enemyArmorReduction);
    void onCombatEnd(bool playerWon, int& bonusXP, int& bonusSilver, bool isArena);

    // Poison system
    void applyPoisonDamage(double& enemyHealth);

    // Conditional checks
    double getConditionalAttackBonus(double currentHP, double maxHP);
    double getConditionalDexterityBonus(double currentHP, double maxHP);

    // Shield system
    double getShield() const { return currentShield; }
    void damageShield(double& incomingDamage);

    // Open Wound system
    bool hasOpenWound() const { return enemyHasOpenWound; }

    // Reset for new combat
    void reset();

    // Public helper for checking equipped items
    bool hasItem(const string& itemName) const;

private:
    CharacterSheet* playerSheet;

    // Equipped items
    string equippedWeapon;
    string equippedHelmet;
    string equippedChestplate;
    string equippedPants;
    string equippedBoots;
    string equippedTalisman;
    string equippedCompanion;

    // Combat state tracking
    int attackCount;
    int playerAttackCount;
    int enemyAttackCount;
    int playerCritCount;
    int playerDodgeCount;
    bool firstCritUsed;
    bool firstPlayerAttackUsed;
    bool secondAttackDodged;
    bool secondEnemyAttackDodged;
    bool firstEnemyAttackProcessed;
    bool thirdEnemyAttackDodged;
    bool fourthEnemyAttackDodged;

    // Stacking buffs
    double permanentAttackBonus;
    double permanentDexterityBonus;

    // Poison tracking
    int poisonDamage;
    int poisonTurnsRemaining;

    // Armor shred tracking
    int armorShredStacks;
    double bonusArmorPen;

    // Shield
    double currentShield;

    // Rare item tracking
    int rhythmBladeCounter;
    bool openWoundApplied;
    bool enemyHasOpenWound;
    double critDamageBonus;
    int killCount;

    // Floating text system
    FloatingTextCallback floatingTextCallback;
    Vector2f playerPosition;
    Vector2f enemyPosition;

    // Helper functions
    void addPermanentAttackBonus(double bonus);
    void addPermanentDexterityBonus(double bonus);
    void spawnFloatingText(const string& text, Color color, bool aroundPlayer);
    void healPlayer(double& currentHealth, double healAmount, double maxHealth, const string& source);
};