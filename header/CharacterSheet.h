#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <vector>
#include "Faction.h"
#include "Inventory.h"
#include "GameStateManager.h"
#include "CurseData.h"

using namespace std;
using namespace sf;

enum class EquipmentSlot {
    Helmet,
    Chestplate,
    Pants,
    Boots,
    Weapon,
    Talisman,
    Companion,
    Curse
};

struct RaceStats {
    double health;
    double attack;
    double intelligence;
    double dexterity;
    double armor;
    double luck;
};

struct RacialPassive {
    string name;
    string description;
    int raceIndex;
    Faction faction;
};

struct InventoryItemDisplay {
    RectangleShape slot;
    Text nameText;
    Text quantityText;
    string itemName;
    int quantity;

    InventoryItemDisplay(const Font& font)
        : nameText(font), quantityText(font), itemName(""), quantity(0) {
    }
};

class CharacterSheet {
public:
    CharacterSheet(float windowWidth, float windowHeight,
        int raceIndex, int classIndex, Faction faction);
    ~CharacterSheet();

    void show();
    void hide();
    bool isVisible() const { return visible; }
    void draw(RenderWindow& window);
    bool isCloseClicked(const RenderWindow& window);
    bool isClickedOutside(const RenderWindow& window);

    void equipItem(EquipmentSlot slot, const string& itemName, Inventory* inventory = nullptr);
    void unequipItem(EquipmentSlot slot, Inventory* inventory = nullptr);
    string getEquippedItem(EquipmentSlot slot) const;

    // Check if an equipped item slot was clicked (returns slot, or -1 if none)
    int checkEquippedItemClick(const RenderWindow& window);

    // Equipment save/load
    map<int, string> saveEquipment() const;
    void loadEquipment(const map<int, string>& savedEquipment);

    void setInventory(Inventory* inv) { inventory = inv; }
    void updateInventoryDisplay();
    void addInventoryItem(const string& itemName);

    double getStat(const string& statName) const;
    double getEquipmentStat(const string& statName) const;

    double getSilverBonus() const;
    double getDodgeCapBonus() const;
    double getArmorBonus() const;
    double getIntelligenceScaling() const;
    double getArmorEffectiveness() const;
    double getHealthRegenPerTurn() const;

    void addXP(double amount);
    int getLevel() const { return level; }
    double getCurrentXP() const { return currentXP; }
    double getXPToNextLevel() const { return xpToNextLevel; }
    int getClassIndex() const { return classIndex; }

    double getDodgeChance() const { return dodgeChance; }
    int getCritChance() const { return critChance; }
    int getCritDamage() const { return critDamage; }

    void setMountData(const MountData* mount) { currentMount = mount; }

    // Curse methods
    void applyCurse(const CurseData& curse);
    void removeCurse();
    CurseData getCurrentCurse() const { return currentCurse; }
    bool hasActiveCurse() const { return hasCurse; }
    bool hasFirstCurseApplied() const { return hasFirstCurse; }
    bool hasUnlockedClass() const { return unlockedClass; }
    void setHasFirstCurse(bool value) { hasFirstCurse = value; }
    void setUnlockedClass(bool value) { unlockedClass = value; }
    bool shouldTriggerCurseSelection() const { return needsCurseSelection; }
    void setNeedsCurseSelection(bool value) { needsCurseSelection = value; }
    Faction getFaction() const { return faction; }

    // Tooltip methods
    void checkEquipmentHover(const RenderWindow& window);
    string getItemDescription(const string& itemName) const;

    void updateEquipmentVisuals();
    void updateStatsDisplay();
    string checkInventoryItemClick(const RenderWindow& window);

    // Sell confirmation methods
    bool isShowingSellConfirmation() const { return showingSellConfirmation; }
    bool checkConfirmSellClick(const RenderWindow& window);
    bool checkCancelSellClick(const RenderWindow& window);
    bool checkClickOutsidePopup(const RenderWindow& window);
    int getSellPrice() const { return pendingSellPrice; }
    string getSelectedInventoryItem() const { return selectedInventoryItem; }
    void clearInventorySelection();
    int getItemShopPrice(const string& itemName) const;
private:
    Font font;
    bool visible;
    float windowWidth;
    float windowHeight;

    int raceIndex;
    int classIndex;
    Faction faction;

    int level;
    double currentXP;
    double xpToNextLevel;

    RaceStats baseStats;
    int critChance;
    int critDamage;
    double dodgeChance;

    map<EquipmentSlot, string> equipment;

    Inventory* inventory;
    vector<InventoryItemDisplay> inventoryDisplayItems;

    const MountData* currentMount;
    Text mountText{ font };

    RectangleShape sheetBackground;
    Text titleText{ font };

    Text levelText{ font };
    RectangleShape xpBarBackground;
    RectangleShape xpBarFill;
    Text xpText{ font };

    vector<RectangleShape> armorSlots;
    vector<Text> armorSlotLabels;
    vector<Text> armorEquippedNames;

    vector<RectangleShape> otherSlots;
    vector<Text> otherSlotLabels;
    vector<Text> otherEquippedNames;

    // Inventory section (separate from equipment slots)
    RectangleShape inventoryBackground;
    vector<RectangleShape> inventorySlots;

    vector<Text> statsTexts;

    void setupUI();
    void updateXPBar();
    void levelUp();
    double calculateXPForLevel(int lvl);
    void loadRaceStats();
    void applyClassBonuses();
    void applyStatConversions();
    void calculateDodgeChance();
    void calculateCritChance();
    RaceStats getEntenteRaceStats(int race);
    RaceStats getThrongRaceStats(int race);
    void setupInventoryDisplay();
    void updateMountDisplay();

    // Curse system members
    CurseData currentCurse;
    bool hasCurse;
    bool hasFirstCurse;
    bool unlockedClass;
    bool needsCurseSelection;

    // Tooltip for equipment hover
    RectangleShape tooltipBox;
    Text tooltipText{ font };
    int hoveredSlotIndex;
    int hoveredInventoryIndex;  // For inventory item hover tooltip

    // Unequip button (shown when equipment slot is clicked)
    RectangleShape unequipButton;
    Text unequipButtonText{ font };
    int selectedEquipmentSlot; // -1 = none selected
    Clock doubleClickClock;
    int lastClickedSlot;
    float doubleClickTime;

    // Inventory item selection and buttons
    int selectedInventoryIndex;
    string selectedInventoryItem;
    Clock inventoryDoubleClickClock;
    int lastClickedInventoryIndex;

    RectangleShape equipButton;
    Text equipButtonText{ font };
    RectangleShape sellButton;
    Text sellButtonText{ font };

    // Sell confirmation popup
    bool showingSellConfirmation;
    int pendingSellPrice;
    RectangleShape confirmPopupBackground;
    RectangleShape confirmPopupBox;
    Text confirmPopupText{ font };
    RectangleShape confirmSureButton;
    Text confirmSureText{ font };
    RectangleShape confirmNoButton;
    Text confirmNoText{ font };

};