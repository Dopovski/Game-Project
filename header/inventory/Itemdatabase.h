#pragma once
#include <string>
#include <map>
#include "CharacterSheet.h"

using namespace std;

struct ItemData {
    string name;
    EquipmentSlot slot;
    double attack;
    double health;
    double armor;
    double dexterity;
    double intelligence;
    double luck;
    string effect;
    int price;  // Price in silver
};

class ItemDatabase {
public:
    static ItemDatabase& getInstance() {
        static ItemDatabase instance;
        return instance;
    }

    EquipmentSlot getItemSlot(const string& itemName) const {
        auto it = itemSlots.find(itemName);
        if (it != itemSlots.end()) {
            return it->second;
        }
        return EquipmentSlot::Weapon; // Default
    }

    ItemData getItemData(const string& itemName) const {
        auto it = itemDataMap.find(itemName);
        if (it != itemDataMap.end()) {
            return it->second;
        }
        return ItemData(); // Return empty data
    }

    int getItemPrice(const string& itemName) const {
        auto it = itemPrices.find(itemName);
        if (it != itemPrices.end()) {
            return it->second;
        }
        return 0; // Unknown item
    }

private:
    ItemDatabase() {
        initializeItemSlots();
        initializeItemData();
        initializeItemPrices();
    }

    void initializeItemSlots() {
        // ============================================
        // ASSASSIN ITEMS (Class 0)
        // ============================================

        // Assassin Weapons
        itemSlots["The Farmer's Scythe"] = EquipmentSlot::Weapon;
        itemSlots["Short Blade"] = EquipmentSlot::Weapon;
        itemSlots["Long Blade"] = EquipmentSlot::Weapon;
        itemSlots["Basic Sword"] = EquipmentSlot::Weapon;
        itemSlots["Broken Sword"] = EquipmentSlot::Weapon;
        itemSlots["Legion Sword"] = EquipmentSlot::Weapon;
        itemSlots["Dagger"] = EquipmentSlot::Weapon;
        itemSlots["Stun"] = EquipmentSlot::Weapon;

        // Assassin Helmets
        itemSlots["Shadow Hood"] = EquipmentSlot::Helmet;
        itemSlots["Silent Hood"] = EquipmentSlot::Helmet;
        itemSlots["Rogue's Cap"] = EquipmentSlot::Helmet;
        itemSlots["Nightblade Mask"] = EquipmentSlot::Helmet;
        itemSlots["Assassin's Helm"] = EquipmentSlot::Helmet;

        // Assassin Chestplates
        itemSlots["Vest"] = EquipmentSlot::Chestplate;
        itemSlots["Nightstalker Vest"] = EquipmentSlot::Chestplate;
        itemSlots["Thief's Cloak"] = EquipmentSlot::Chestplate;
        itemSlots["Silent Armor"] = EquipmentSlot::Chestplate;
        itemSlots["Rogue's Garb"] = EquipmentSlot::Chestplate;

        // Assassin Leggings (Pants)
        itemSlots["Shadow Leggings"] = EquipmentSlot::Pants;
        itemSlots["Silent Steps"] = EquipmentSlot::Pants;
        itemSlots["Poisoned Trousers"] = EquipmentSlot::Pants;
        itemSlots["Nightblade Pants"] = EquipmentSlot::Pants;
        itemSlots["Rogue's Greaves"] = EquipmentSlot::Pants;

        // Assassin Boots
        itemSlots["Leather Boots"] = EquipmentSlot::Boots;
        itemSlots["Long Boots"] = EquipmentSlot::Boots;
        itemSlots["Dynamites Boots"] = EquipmentSlot::Boots;
        itemSlots["Boots of Healing"] = EquipmentSlot::Boots;
        itemSlots["Shadowstep Boots"] = EquipmentSlot::Boots;

        // Assassin Talismans
        itemSlots["I DONT KNOW"] = EquipmentSlot::Talisman;
        itemSlots["Metal"] = EquipmentSlot::Talisman;
        itemSlots["Love"] = EquipmentSlot::Talisman;
        itemSlots["Bloodstone"] = EquipmentSlot::Talisman;
        itemSlots["Flask"] = EquipmentSlot::Talisman;
        itemSlots["Vial of Venom"] = EquipmentSlot::Talisman;

        // Assassin Companions
        itemSlots["Wolf"] = EquipmentSlot::Companion;
        itemSlots["Rogue Cat"] = EquipmentSlot::Companion;
        itemSlots["Shadow Rat"] = EquipmentSlot::Companion;
        itemSlots["Crows of Misfortune"] = EquipmentSlot::Companion;
        itemSlots["Merchant Rat"] = EquipmentSlot::Companion;

        // ============================================
        // MAGE ITEMS (Class 2)
        // ============================================

        // Mage Weapons
        itemSlots["Dark Ring"] = EquipmentSlot::Weapon;
        itemSlots["Book of Secrets"] = EquipmentSlot::Weapon;
        itemSlots["Lost Ring"] = EquipmentSlot::Weapon;
        itemSlots["Book of Healing"] = EquipmentSlot::Weapon;
        itemSlots["Stick"] = EquipmentSlot::Weapon;
        itemSlots["Rod"] = EquipmentSlot::Weapon;

        // Mage Helmets
        itemSlots["Wizard's Hat"] = EquipmentSlot::Helmet;
        itemSlots["Arcane Hood"] = EquipmentSlot::Helmet;
        itemSlots["Scholar's Cap"] = EquipmentSlot::Helmet;
        itemSlots["Mage's Circlet"] = EquipmentSlot::Helmet;
        itemSlots["Enchanted Hood"] = EquipmentSlot::Helmet;

        // Mage Chestplates
        itemSlots["Apprentice Robe"] = EquipmentSlot::Chestplate;
        itemSlots["Pyromancer's Cloak"] = EquipmentSlot::Chestplate;
        itemSlots["Mage's Mantle"] = EquipmentSlot::Chestplate;
        itemSlots["Robe of Shielding"] = EquipmentSlot::Chestplate;
        itemSlots["Arcane Vestments"] = EquipmentSlot::Chestplate;

        // Mage Leggings
        itemSlots["Scholar's Trousers"] = EquipmentSlot::Pants;
        itemSlots["Pyromancer's Leggings"] = EquipmentSlot::Pants;
        itemSlots["Mana Flow Pants"] = EquipmentSlot::Pants;
        itemSlots["Arcane Pants"] = EquipmentSlot::Pants;
        itemSlots["Mystic Legwraps"] = EquipmentSlot::Pants;

        // Mage Boots
        itemSlots["Magical Boots"] = EquipmentSlot::Boots;
        itemSlots["Enchanted Boots"] = EquipmentSlot::Boots;
        itemSlots["Boots of Witch"] = EquipmentSlot::Boots;
        itemSlots["Boots of the Pyromancer"] = EquipmentSlot::Boots;
        itemSlots["Boots of Pyromancer"] = EquipmentSlot::Boots;
        itemSlots["Arcane Slippers"] = EquipmentSlot::Boots;

        // Mage Talismans
        itemSlots["Some Other Crystal"] = EquipmentSlot::Talisman;
        itemSlots["Ancient Coin"] = EquipmentSlot::Talisman;
        itemSlots["Orb of the Fortune Teller"] = EquipmentSlot::Talisman;
        itemSlots["Orb of Fortune Teller"] = EquipmentSlot::Talisman;
        itemSlots["GOLDEN ARMOR"] = EquipmentSlot::Talisman;
        itemSlots["Big CRYSTAL"] = EquipmentSlot::Talisman;
        itemSlots["Meteor"] = EquipmentSlot::Talisman;
        itemSlots["STAR"] = EquipmentSlot::Talisman;

        // Mage Companions
        itemSlots["Mana Sprite"] = EquipmentSlot::Companion;
        itemSlots["Arcane Familiar"] = EquipmentSlot::Companion;
        itemSlots["Fire Imp"] = EquipmentSlot::Companion;
        itemSlots["Crystal Owl"] = EquipmentSlot::Companion;
        itemSlots["Fox"] = EquipmentSlot::Companion;

        // ============================================
        // BRUISER ITEMS (Class 1)
        // ============================================

        // Bruiser Weapons
        itemSlots["Battle Shield"] = EquipmentSlot::Weapon;
        itemSlots["Pickaxe"] = EquipmentSlot::Weapon;
        itemSlots["Mallet"] = EquipmentSlot::Weapon;
        itemSlots["Tomahawk"] = EquipmentSlot::Weapon;
        itemSlots["Hammer"] = EquipmentSlot::Weapon;
        itemSlots["Hammer of George"] = EquipmentSlot::Weapon;
        itemSlots["Pointy Stick"] = EquipmentSlot::Weapon;
        itemSlots["Enchanted Shield"] = EquipmentSlot::Weapon;

        // Bruiser Helmets
        itemSlots["Iron Helm"] = EquipmentSlot::Helmet;
        itemSlots["Battle Helm"] = EquipmentSlot::Helmet;
        itemSlots["Warrior's Cap"] = EquipmentSlot::Helmet;
        itemSlots["Guardian Helm"] = EquipmentSlot::Helmet;
        itemSlots["Heavy Plate Helm"] = EquipmentSlot::Helmet;

        // Bruiser Chestplates
        itemSlots["Light Vest"] = EquipmentSlot::Chestplate;
        itemSlots["Enchanted Armor"] = EquipmentSlot::Chestplate;
        itemSlots["Guardian Plate"] = EquipmentSlot::Chestplate;
        itemSlots["Juggernaut Armor"] = EquipmentSlot::Chestplate;
        itemSlots["Cloak"] = EquipmentSlot::Chestplate;

        // Bruiser Leggings
        itemSlots["Iron Legguards"] = EquipmentSlot::Pants;
        itemSlots["Stone Greaves"] = EquipmentSlot::Pants;
        itemSlots["Vanguard Pants"] = EquipmentSlot::Pants;
        itemSlots["Juggernaut Leggings"] = EquipmentSlot::Pants;
        itemSlots["Titan's Legguards"] = EquipmentSlot::Pants;

        // Bruiser Boots
        itemSlots["Boots of Steel"] = EquipmentSlot::Boots;
        itemSlots["Boots of Mage"] = EquipmentSlot::Boots;
        itemSlots["Boots of Health"] = EquipmentSlot::Boots;
        itemSlots["Iron Treads"] = EquipmentSlot::Boots;
        itemSlots["Boulder Boots"] = EquipmentSlot::Boots;

        // Bruiser Talismans
        itemSlots["Horn for Battle"] = EquipmentSlot::Talisman;
        itemSlots["Map"] = EquipmentSlot::Talisman;
        itemSlots["Chain"] = EquipmentSlot::Talisman;
        itemSlots["Crystal"] = EquipmentSlot::Talisman;
        itemSlots["Money Shield"] = EquipmentSlot::Talisman;
        itemSlots["Scycle"] = EquipmentSlot::Talisman;

        // Bruiser Companions
        itemSlots["Rabbit"] = EquipmentSlot::Companion;
        itemSlots["Turtle"] = EquipmentSlot::Companion;
        itemSlots["Hedgehog"] = EquipmentSlot::Companion;
        itemSlots["Iron Golem"] = EquipmentSlot::Companion;
        itemSlots["Bruiser Merchant Rat"] = EquipmentSlot::Companion;

        // ============================================
        // RARE ITEMS - ASSASSIN
        // ============================================

        // Assassin Rare Weapons
        itemSlots["Venomfang Blade"] = EquipmentSlot::Weapon;
        itemSlots["Phantom Edge"] = EquipmentSlot::Weapon;
        itemSlots["Shadowpiercer Crossbow"] = EquipmentSlot::Weapon;
        itemSlots["Deathwhisper Dagger"] = EquipmentSlot::Weapon;
        itemSlots["Rhythm Blade"] = EquipmentSlot::Weapon;
        itemSlots["Soulreaver Bow"] = EquipmentSlot::Weapon;
        itemSlots["Vampiric Fang"] = EquipmentSlot::Weapon;
        itemSlots["Shield Crusher"] = EquipmentSlot::Weapon;
        itemSlots["Executioner's Blade"] = EquipmentSlot::Weapon;
        itemSlots["Armor Shredder"] = EquipmentSlot::Weapon;

        // Assassin Rare Helmets
        itemSlots["Nightstalker's Cowl"] = EquipmentSlot::Helmet;
        itemSlots["Phantom Visage"] = EquipmentSlot::Helmet;
        itemSlots["Veil of Shadows"] = EquipmentSlot::Helmet;
        itemSlots["Death's Shroud"] = EquipmentSlot::Helmet;
        itemSlots["Whisper Mask"] = EquipmentSlot::Helmet;

        // Assassin Rare Chestplates
        itemSlots["Shadowweave Armor"] = EquipmentSlot::Chestplate;
        itemSlots["Nightfall Vest"] = EquipmentSlot::Chestplate;
        itemSlots["Cloak of Evasion"] = EquipmentSlot::Chestplate;
        itemSlots["Deathdealer's Garb"] = EquipmentSlot::Chestplate;
        itemSlots["Assassin's Shroud"] = EquipmentSlot::Chestplate;

        // Assassin Rare Leggings
        itemSlots["Swift Striders"] = EquipmentSlot::Pants;
        itemSlots["Phantom Greaves"] = EquipmentSlot::Pants;
        itemSlots["Venom-Laced Pants"] = EquipmentSlot::Pants;
        itemSlots["Shadow Pants"] = EquipmentSlot::Pants;
        itemSlots["Death's Step"] = EquipmentSlot::Pants;

        // Assassin Rare Boots
        itemSlots["Stalker's Treads"] = EquipmentSlot::Boots;
        itemSlots["Nightblade Boots"] = EquipmentSlot::Boots;
        itemSlots["Phantom Step"] = EquipmentSlot::Boots;
        itemSlots["Deathwalk Boots"] = EquipmentSlot::Boots;
        itemSlots["Silent Runners"] = EquipmentSlot::Boots;

        // Assassin Rare Talismans
        itemSlots["Heart of Shadows"] = EquipmentSlot::Talisman;
        itemSlots["Venom Vial"] = EquipmentSlot::Talisman;
        itemSlots["Dark Pendant"] = EquipmentSlot::Talisman;
        itemSlots["Assassin's Mark"] = EquipmentSlot::Talisman;
        itemSlots["Shadow Crystal"] = EquipmentSlot::Talisman;

        // Assassin Rare Companions
        itemSlots["Shadow Panther"] = EquipmentSlot::Companion;
        itemSlots["Venomous Spider"] = EquipmentSlot::Companion;
        itemSlots["Ghost Raven"] = EquipmentSlot::Companion;
        itemSlots["Blood Bat"] = EquipmentSlot::Companion;
        itemSlots["Phantom Serpent"] = EquipmentSlot::Companion;

        // ============================================
        // RARE ITEMS - MAGE
        // ============================================

        // Mage Rare Weapons
        itemSlots["Inferno Wand"] = EquipmentSlot::Weapon;
        itemSlots["Wand of Devastation"] = EquipmentSlot::Weapon;
        itemSlots["Arcane Staff"] = EquipmentSlot::Weapon;
        itemSlots["Grand Rod"] = EquipmentSlot::Weapon;
        itemSlots["Bleeding Orb"] = EquipmentSlot::Weapon;
        itemSlots["Mage Shield Piercer"] = EquipmentSlot::Weapon;

        // Mage Rare Helmets
        itemSlots["Mask of Embers"] = EquipmentSlot::Helmet;
        itemSlots["Crown of Flames"] = EquipmentSlot::Helmet;
        itemSlots["Archmage's Hood"] = EquipmentSlot::Helmet;
        itemSlots["Veil of Sorcery"] = EquipmentSlot::Helmet;
        itemSlots["Mindfire Circlet"] = EquipmentSlot::Helmet;

        // Mage Rare Chestplates
        itemSlots["Battlemage Cloak"] = EquipmentSlot::Chestplate;
        itemSlots["Robes of the Inferno"] = EquipmentSlot::Chestplate;
        itemSlots["Arcane Vestment"] = EquipmentSlot::Chestplate;
        itemSlots["Sorcerer's Mantle"] = EquipmentSlot::Chestplate;
        itemSlots["Spellweaver's Robe"] = EquipmentSlot::Chestplate;

        // Mage Rare Leggings
        itemSlots["Mana-Woven Pants"] = EquipmentSlot::Pants;
        itemSlots["Flamecaster Leggings"] = EquipmentSlot::Pants;
        itemSlots["Arcane Greaves"] = EquipmentSlot::Pants;
        itemSlots["Sorcerer's Trousers"] = EquipmentSlot::Pants;
        itemSlots["Runic Legwraps"] = EquipmentSlot::Pants;

        // Mage Rare Boots
        itemSlots["Boots of the Archmage"] = EquipmentSlot::Boots;
        itemSlots["Flamestep Boots"] = EquipmentSlot::Boots;
        itemSlots["Runic Treads"] = EquipmentSlot::Boots;
        itemSlots["Spellfire Slippers"] = EquipmentSlot::Boots;
        itemSlots["Mage's Striders"] = EquipmentSlot::Boots;

        // Mage Rare Talismans
        itemSlots["Flame Orb"] = EquipmentSlot::Talisman;
        itemSlots["Arcane Chains"] = EquipmentSlot::Talisman;
        itemSlots["Golden Aegis"] = EquipmentSlot::Talisman;
        itemSlots["Arcane Reliquary"] = EquipmentSlot::Talisman;
        itemSlots["Orb of Devastation"] = EquipmentSlot::Talisman;
        itemSlots["Crystal of Vitality"] = EquipmentSlot::Talisman;
        itemSlots["Runic Pendant"] = EquipmentSlot::Talisman;

        // Mage Rare Companions
        itemSlots["Phoenix Hatchling"] = EquipmentSlot::Companion;
        itemSlots["Arcane Golem"] = EquipmentSlot::Companion;
        itemSlots["Fire Elemental"] = EquipmentSlot::Companion;
        itemSlots["Mana Wyrm"] = EquipmentSlot::Companion;
        itemSlots["Crystal Guardian"] = EquipmentSlot::Companion;

        // ============================================
        // RARE ITEMS - BRUISER
        // ============================================

        // Bruiser Rare Weapons
        itemSlots["Colossus Hammer"] = EquipmentSlot::Weapon;
        itemSlots["Brutal Cleaver"] = EquipmentSlot::Weapon;
        itemSlots["Titan's Axe"] = EquipmentSlot::Weapon;
        itemSlots["Bruiser Shield Basher"] = EquipmentSlot::Weapon;
        itemSlots["Warlord's Mace"] = EquipmentSlot::Weapon;
        itemSlots["Bone Crusher"] = EquipmentSlot::Weapon;

        // Bruiser Rare Helmets
        itemSlots["Titan's Visage"] = EquipmentSlot::Helmet;
        itemSlots["Juggernaut Helm"] = EquipmentSlot::Helmet;
        itemSlots["Warlord's Crown"] = EquipmentSlot::Helmet;
        itemSlots["Berserker's Mask"] = EquipmentSlot::Helmet;
        itemSlots["Fortress Helm"] = EquipmentSlot::Helmet;

        // Bruiser Rare Chestplates
        itemSlots["Spiked Bulwark"] = EquipmentSlot::Chestplate;
        itemSlots["Ironclad Vest"] = EquipmentSlot::Chestplate;
        itemSlots["Titan's Plate"] = EquipmentSlot::Chestplate;
        itemSlots["Warlord's Mail"] = EquipmentSlot::Chestplate;
        itemSlots["Berserker's Harness"] = EquipmentSlot::Chestplate;
        itemSlots["Fortress Armor"] = EquipmentSlot::Chestplate;

        // Bruiser Rare Leggings
        itemSlots["Titan's Greaves"] = EquipmentSlot::Pants;
        itemSlots["Juggernaut Pants"] = EquipmentSlot::Pants;
        itemSlots["Warlord's Pants"] = EquipmentSlot::Pants;
        itemSlots["Berserker's Legguards"] = EquipmentSlot::Pants;
        itemSlots["Fortress Greaves"] = EquipmentSlot::Pants;

        // Bruiser Rare Boots
        itemSlots["Titan's Treads"] = EquipmentSlot::Boots;
        itemSlots["Juggernaut Boots"] = EquipmentSlot::Boots;
        itemSlots["Warlord's Stompers"] = EquipmentSlot::Boots;
        itemSlots["Berserker's Boots"] = EquipmentSlot::Boots;
        itemSlots["Fortress Steps"] = EquipmentSlot::Boots;

        // Bruiser Rare Talismans
        itemSlots["Heart of the Mountain"] = EquipmentSlot::Talisman;
        itemSlots["Warlord's Emblem"] = EquipmentSlot::Talisman;
        itemSlots["Titan's Heart"] = EquipmentSlot::Talisman;
        itemSlots["Berserker's Totem"] = EquipmentSlot::Talisman;
        itemSlots["Stone of Vitality"] = EquipmentSlot::Talisman;
        itemSlots["Iron Pendant"] = EquipmentSlot::Talisman;

        // Bruiser Rare Companions
        itemSlots["War Bear"] = EquipmentSlot::Companion;
        itemSlots["Iron Guardian"] = EquipmentSlot::Companion;
        itemSlots["Stone Tortoise"] = EquipmentSlot::Companion;
        itemSlots["Berserker Wolf"] = EquipmentSlot::Companion;
        itemSlots["Mountain Golem"] = EquipmentSlot::Companion;
    }

    void initializeItemData() {
        // ============================================
        // ASSASSIN ITEM DATA
        // ============================================

        // Assassin Weapons
        itemDataMap["The Farmer's Scythe"] = { "The Farmer's Scythe", EquipmentSlot::Weapon, 17, 0, 0, 0, 0, 0, "+3 Health on hit, +10 Silver on dungeon finish", 100 };
        itemDataMap["Short Blade"] = { "Short Blade", EquipmentSlot::Weapon, 10, 80, 0, 0, 0, 0, "Heal 2.5 HP after every attack", 30 };
        itemDataMap["Long Blade"] = { "Long Blade", EquipmentSlot::Weapon, 30, 150, 0, 0, 0, 0, "", 120 };
        itemDataMap["Basic Sword"] = { "Basic Sword", EquipmentSlot::Weapon, 40, 0, 0, 0, 0, 0, "", 150 };
        itemDataMap["Broken Sword"] = { "Broken Sword", EquipmentSlot::Weapon, 10, 0, 0, 0, 0, 0, "", 20 };
        itemDataMap["Legion Sword"] = { "Legion Sword", EquipmentSlot::Weapon, 60, 0, 0, 0, 0, 0, "", 200 };
        itemDataMap["Dagger"] = { "Dagger", EquipmentSlot::Weapon, 15, 0, 0, 15, 0, 0, "Each hit shreds 3 armor (max 15)", 35 };
        itemDataMap["Stun"] = { "Stun", EquipmentSlot::Weapon, 0, 0, 0, 0, 0, 0, "Stun enemy and attack again", 500 };

        // Assassin Helmets
        itemDataMap["Shadow Hood"] = { "Shadow Hood", EquipmentSlot::Helmet, 0, 0, 15, 5, 0, 0, "+5 Dex for 1 turn after dodge", 120 };
        itemDataMap["Silent Hood"] = { "Silent Hood", EquipmentSlot::Helmet, 0, 0, 10, 0, 0, 0, "First crit: +10 bonus Attack", 80 };
        itemDataMap["Rogue's Cap"] = { "Rogue's Cap", EquipmentSlot::Helmet, 5, 0, 12, 0, 0, 0, "Heal 2 HP after crit", 100 };
        itemDataMap["Nightblade Mask"] = { "Nightblade Mask", EquipmentSlot::Helmet, 0, 0, 10, 0, 0, 0, "Dodge second enemy attack", 90 };
        itemDataMap["Assassin's Helm"] = { "Assassin's Helm", EquipmentSlot::Helmet, 0, 0, 15, 0, 0, 0, "+5 Dex while below 50% Health", 110 };

        // Assassin Chestplates
        itemDataMap["Vest"] = { "Vest", EquipmentSlot::Chestplate, 0, 0, 25, 0, 0, 0, "", 110 };
        itemDataMap["Nightstalker Vest"] = { "Nightstalker Vest", EquipmentSlot::Chestplate, 0, 0, 25, 0, 0, 0, "+5 Attack per crit in combat", 200 };
        itemDataMap["Thief's Cloak"] = { "Thief's Cloak", EquipmentSlot::Chestplate, 0, 0, 20, 0, 0, 0, "Dodge first boss/elite attack", 150 };
        itemDataMap["Silent Armor"] = { "Silent Armor", EquipmentSlot::Chestplate, 0, 0, 15, 0, 0, 0, "+5 Dex when attacked", 100 };
        itemDataMap["Rogue's Garb"] = { "Rogue's Garb", EquipmentSlot::Chestplate, 0, 0, 20, 5, 0, 0, "<50% HP: +15 Attack for one turn", 180 };

        // Assassin Leggings
        itemDataMap["Shadow Leggings"] = { "Shadow Leggings", EquipmentSlot::Pants, 0, 0, 15, 0, 0, 0, "+5 Dex after each attack", 130 };
        itemDataMap["Silent Steps"] = { "Silent Steps", EquipmentSlot::Pants, 0, 0, 10, 10, 0, 0, "Dodge second enemy attack", 115 };
        itemDataMap["Poisoned Trousers"] = { "Poisoned Trousers", EquipmentSlot::Pants, 0, 0, 10, 0, 0, 0, "Poison: 3 dmg/turn for 2 turns", 90 };
        itemDataMap["Nightblade Pants"] = { "Nightblade Pants", EquipmentSlot::Pants, 5, 0, 12, 0, 0, 0, "+5 Attack if attacking below 50% Health", 100 };
        itemDataMap["Rogue's Greaves"] = { "Rogue's Greaves", EquipmentSlot::Pants, 0, 0, 15, 0, 0, 0, "After crit: +5 Dex for next attack", 110 };

        // Assassin Boots
        itemDataMap["Leather Boots"] = { "Leather Boots", EquipmentSlot::Boots, 0, 0, 20, 0, 0, 0, "", 25 };
        itemDataMap["Long Boots"] = { "Long Boots", EquipmentSlot::Boots, 0, 0, 25, 0, 0, 0, "", 100 };
        itemDataMap["Dynamites Boots"] = { "Dynamites Boots", EquipmentSlot::Boots, 30, 0, 0, 0, 0, 0, "+10 Armor Penetration", 120 };
        itemDataMap["Boots of Healing"] = { "Boots of Healing", EquipmentSlot::Boots, 20, 0, 0, 0, 0, 0, "+4 Health per attack", 160 };
        itemDataMap["Shadowstep Boots"] = { "Shadowstep Boots", EquipmentSlot::Boots, 25, 0, 0, 10, 0, 0, "Below 50% HP: +15 Attack", 150 };

        // Assassin Talismans
        itemDataMap["I DONT KNOW"] = { "I DONT KNOW", EquipmentSlot::Talisman, 0, 150, 0, 0, 0, 0, "", 150 };
        itemDataMap["Metal"] = { "Metal", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "Gain 20 Shield at start of combat", 350 };
        itemDataMap["Bloodstone"] = { "Bloodstone", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "+10 Attack below 30% HP", 200 };
        itemDataMap["Flask"] = { "Flask", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "Second enemy attack deals 0 damage", 250 };
        itemDataMap["Vial of Venom"] = { "Vial of Venom", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "Poison: 5 dmg/turn for 3 turns", 450 };

        // Assassin Companions
        itemDataMap["Wolf"] = { "Wolf", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Dungeon win: +80 XP, heal 3 HP per attack", 600 };
        itemDataMap["Rogue Cat"] = { "Rogue Cat", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "First attack guaranteed crit", 550 };
        itemDataMap["Shadow Rat"] = { "Shadow Rat", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+10 Dex per dodge in combat", 400 };
        itemDataMap["Crows of Misfortune"] = { "Crows of Misfortune", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "First enemy attack: 20% miss chance", 500 };
        itemDataMap["Merchant Rat"] = { "Merchant Rat", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Dungeon wins grant +25 Silver", 350 };

        // ============================================
        // MAGE ITEM DATA
        // ============================================

        // Mage Weapons
        itemDataMap["Dark Ring"] = { "Dark Ring", EquipmentSlot::Weapon, 50, 0, 0, 0, 20, 0, "Arena: +5 dmg/spell (max +25)", 400 };
        itemDataMap["Book of Secrets"] = { "Book of Secrets", EquipmentSlot::Weapon, 30, 0, 0, 0, 15, 0, "+5 dmg to burning enemies", 200 };
        itemDataMap["Lost Ring"] = { "Lost Ring", EquipmentSlot::Weapon, 25, 0, 0, 0, 30, 0, "+10 Crit Damage", 180 };
        itemDataMap["Book of Healing"] = { "Book of Healing", EquipmentSlot::Weapon, 20, 0, 0, 0, 10, 0, "Heal 10 HP on hit", 150 };
        itemDataMap["Stick"] = { "Stick", EquipmentSlot::Weapon, 10, 0, 0, 0, 5, 0, "", 30 };
        itemDataMap["Rod"] = { "Rod", EquipmentSlot::Weapon, 35, 0, 0, 0, 0, 0, "", 80 };

        // Mage Helmets
        itemDataMap["Wizard's Hat"] = { "Wizard's Hat", EquipmentSlot::Helmet, 0, 0, 8, 0, 15, 0, "First spell: +5 damage", 100 };
        itemDataMap["Arcane Hood"] = { "Arcane Hood", EquipmentSlot::Helmet, 0, 0, 10, 0, 12, 0, "+5 Intelligence per hit in combat", 150 };
        itemDataMap["Scholar's Cap"] = { "Scholar's Cap", EquipmentSlot::Helmet, 0, 0, 12, 0, 10, 0, "+5 Attack per crit", 120 };
        itemDataMap["Mage's Circlet"] = { "Mage's Circlet", EquipmentSlot::Helmet, 0, 0, 15, 0, 20, 0, "Heal 5 HP on crit", 180 };
        itemDataMap["Enchanted Hood"] = { "Enchanted Hood", EquipmentSlot::Helmet, 0, 0, 10, 0, 8, 0, "Block second enemy attack", 140 };

        // Mage Chestplates
        itemDataMap["Apprentice Robe"] = { "Apprentice Robe", EquipmentSlot::Chestplate, 0, 0, 10, 0, 10, 0, "", 60 };
        itemDataMap["Pyromancer's Cloak"] = { "Pyromancer's Cloak", EquipmentSlot::Chestplate, 0, 0, 15, 0, 15, 0, "Burn: 3 dmg/turn for 2 turns", 180 };
        itemDataMap["Mage's Mantle"] = { "Mage's Mantle", EquipmentSlot::Chestplate, 0, 0, 20, 0, 20, 0, "", 150 };
        itemDataMap["Robe of Shielding"] = { "Robe of Shielding", EquipmentSlot::Chestplate, 0, 0, 20, 0, 0, 0, "Gain 15 Shield when below 50% HP", 200 };
        itemDataMap["Arcane Vestments"] = { "Arcane Vestments", EquipmentSlot::Chestplate, 0, 0, 18, 0, 25, 0, "+5 Attack per spell cast", 220 };

        // Mage Leggings
        itemDataMap["Scholar's Trousers"] = { "Scholar's Trousers", EquipmentSlot::Pants, 0, 0, 12, 0, 0, 0, "Heal 2 HP per spell", 100 };
        itemDataMap["Pyromancer's Leggings"] = { "Pyromancer's Leggings", EquipmentSlot::Pants, 0, 0, 10, 0, 0, 0, "Burn: 2 dmg/turn for 2 turns", 120 };
        itemDataMap["Mana Flow Pants"] = { "Mana Flow Pants", EquipmentSlot::Pants, 0, 0, 8, 0, 10, 0, "+5 Intelligence below 50% HP", 140 };
        itemDataMap["Arcane Pants"] = { "Arcane Pants", EquipmentSlot::Pants, 0, 0, 10, 0, 12, 0, "Gain 20 Shield at combat start", 180 };
        itemDataMap["Mystic Legwraps"] = { "Mystic Legwraps", EquipmentSlot::Pants, 0, 0, 8, 0, 15, 0, "+3 Attack per spell", 160 };

        // Mage Boots
        itemDataMap["Magical Boots"] = { "Magical Boots", EquipmentSlot::Boots, 0, 0, 0, 0, 12, 0, "", 50 };
        itemDataMap["Enchanted Boots"] = { "Enchanted Boots", EquipmentSlot::Boots, 0, 0, 0, 0, 18, 0, "", 80 };
        itemDataMap["Boots of Witch"] = { "Boots of Witch", EquipmentSlot::Boots, 40, 0, 0, 0, 0, 0, "+8 Armor Penetration", 150 };
        itemDataMap["Boots of the Pyromancer"] = { "Boots of the Pyromancer", EquipmentSlot::Boots, 30, 0, 0, 0, 0, 0, "+5 Armor Pen, Deal 5 damage at start", 180 };
        itemDataMap["Boots of Pyromancer"] = { "Boots of Pyromancer", EquipmentSlot::Boots, 30, 0, 0, 0, 0, 0, "+5 Armor Pen, Deal 5 damage at start", 180 };
        itemDataMap["Arcane Slippers"] = { "Arcane Slippers", EquipmentSlot::Boots, 25, 0, 0, 0, 12, 0, "First spell +10 damage", 200 };

        // Mage Talismans
        itemDataMap["Some Other Crystal"] = { "Some Other Crystal", EquipmentSlot::Talisman, 15, 0, 0, 0, 15, 0, "", 100 };
        itemDataMap["Ancient Coin"] = { "Ancient Coin", EquipmentSlot::Talisman, 0, 80, 0, 0, 0, 0, "Gain 30 Silver every 3rd dungeon", 250 };
        itemDataMap["Orb of the Fortune Teller"] = { "Orb of the Fortune Teller", EquipmentSlot::Talisman, 30, 80, 0, 0, 0, 0, "", 180 };
        itemDataMap["Orb of Fortune Teller"] = { "Orb of Fortune Teller", EquipmentSlot::Talisman, 30, 80, 0, 0, 0, 0, "", 180 };
        itemDataMap["GOLDEN ARMOR"] = { "GOLDEN ARMOR", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "First enemy attack deals 0 damage", 400 };
        itemDataMap["Big CRYSTAL"] = { "Big CRYSTAL", EquipmentSlot::Talisman, 30, 0, 10, 0, 0, 0, "Gain 20 Shield at start", 350 };
        itemDataMap["Meteor"] = { "Meteor", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "Deal 25 damage at combat start", 500 };
        itemDataMap["STAR"] = { "STAR", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "Deal 50 damage at start (Arena only)", 600 };

        // Mage Companions
        itemDataMap["Mana Sprite"] = { "Mana Sprite", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+10 Intelligence in combat", 300 };
        itemDataMap["Arcane Familiar"] = { "Arcane Familiar", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "5% chance any enemy attack deals 0", 450 };
        itemDataMap["Fire Imp"] = { "Fire Imp", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+5 spell damage per spell (max +25)", 400 };
        itemDataMap["Crystal Owl"] = { "Crystal Owl", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Restore 5 HP per turn", 550 };
        itemDataMap["Fox"] = { "Fox", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Dungeon win: +80 XP, Heal 3 HP per attack", 600 };

        // ============================================
        // BRUISER ITEM DATA
        // ============================================

        // Bruiser Weapons
        itemDataMap["Battle Shield"] = { "Battle Shield", EquipmentSlot::Weapon, 0, 110, 0, 0, 0, 0, "Heal 4 HP after attack", 80 };
        itemDataMap["Pickaxe"] = { "Pickaxe", EquipmentSlot::Weapon, 15, 0, 0, 0, 0, 0, "+5 Attack per hit (max +25)", 60 };
        itemDataMap["Mallet"] = { "Mallet", EquipmentSlot::Weapon, 8, 0, 0, 0, 0, 0, "+8 Health on hit", 70 };
        itemDataMap["Tomahawk"] = { "Tomahawk", EquipmentSlot::Weapon, 25, 130, 0, 0, 0, 0, "", 100 };
        itemDataMap["Hammer"] = { "Hammer", EquipmentSlot::Weapon, 35, 0, 0, 0, 0, 0, "", 90 };
        itemDataMap["Hammer of George"] = { "Hammer of George", EquipmentSlot::Weapon, 25, 150, 0, 0, 0, 0, "", 120 };
        itemDataMap["Pointy Stick"] = { "Pointy Stick", EquipmentSlot::Weapon, 40, 0, 20, 0, 0, 0, "", 140 };
        itemDataMap["Enchanted Shield"] = { "Enchanted Shield", EquipmentSlot::Weapon, 0, 0, 0, 0, 0, 0, "+35 Shield at combat start", 200 };

        // Bruiser Helmets
        itemDataMap["Iron Helm"] = { "Iron Helm", EquipmentSlot::Helmet, 0, 0, 30, 0, 0, 0, "Reduce first enemy attack by 5", 80 };
        itemDataMap["Battle Helm"] = { "Battle Helm", EquipmentSlot::Helmet, 0, 10, 25, 0, 0, 0, "Heal 3 HP when hit", 100 };
        itemDataMap["Warrior's Cap"] = { "Warrior's Cap", EquipmentSlot::Helmet, 0, 0, 20, 0, 0, 0, "Gain 10 Shield at combat start", 90 };
        itemDataMap["Guardian Helm"] = { "Guardian Helm", EquipmentSlot::Helmet, 0, 0, 25, 0, 0, 0, "First enemy attack deals 0 damage", 150 };
        itemDataMap["Heavy Plate Helm"] = { "Heavy Plate Helm", EquipmentSlot::Helmet, 0, 50, 0, 0, 0, 0, "", 70 };

        // Bruiser Chestplates
        itemDataMap["Light Vest"] = { "Light Vest", EquipmentSlot::Chestplate, 0, 0, 15, 0, 0, 0, "+3 HP per attack", 60 };
        itemDataMap["Enchanted Armor"] = { "Enchanted Armor", EquipmentSlot::Chestplate, 0, 0, 0, 0, 0, 0, "+25 Shield at combat start", 120 };
        itemDataMap["Guardian Plate"] = { "Guardian Plate", EquipmentSlot::Chestplate, 0, 0, 30, 0, 0, 0, "Heal 3 HP after attack", 140 };
        itemDataMap["Juggernaut Armor"] = { "Juggernaut Armor", EquipmentSlot::Chestplate, 0, 35, 0, 0, 0, 0, "Reduce first enemy attack by 5", 130 };
        itemDataMap["Cloak"] = { "Cloak", EquipmentSlot::Chestplate, 10, 0, 20, 0, 0, 0, "", 100 };

        // Bruiser Leggings
        itemDataMap["Iron Legguards"] = { "Iron Legguards", EquipmentSlot::Pants, 0, 0, 25, 0, 0, 0, "Reduce damage taken by 3%", 90 };
        itemDataMap["Stone Greaves"] = { "Stone Greaves", EquipmentSlot::Pants, 0, 10, 20, 0, 0, 0, "Gain 5 Shield at combat start", 100 };
        itemDataMap["Vanguard Pants"] = { "Vanguard Pants", EquipmentSlot::Pants, 0, 5, 15, 0, 0, 0, "Heal 2 HP per attack", 110 };
        itemDataMap["Juggernaut Leggings"] = { "Juggernaut Leggings", EquipmentSlot::Pants, 0, 0, 20, 0, 0, 0, "Enemy attacks deal -2 damage", 120 };
        itemDataMap["Titan's Legguards"] = { "Titan's Legguards", EquipmentSlot::Pants, 0, 25, 0, 0, 0, 0, "Gain 5 HP per turn", 150 };

        // Bruiser Boots
        itemDataMap["Boots of Steel"] = { "Boots of Steel", EquipmentSlot::Boots, 0, 0, 30, 0, 0, 0, "Reduce damage taken by 10%", 160 };
        itemDataMap["Boots of Mage"] = { "Boots of Mage", EquipmentSlot::Boots, 0, 0, 35, 0, 0, 0, "Gain Shield vs Mage enemies", 100 };
        itemDataMap["Boots of Health"] = { "Boots of Health", EquipmentSlot::Boots, 0, 150, 0, 0, 0, 0, "", 120 };
        itemDataMap["Iron Treads"] = { "Iron Treads", EquipmentSlot::Boots, 0, 0, 30, 0, 0, 0, "Reduce damage taken by 5%", 110 };
        itemDataMap["Boulder Boots"] = { "Boulder Boots", EquipmentSlot::Boots, 0, 25, 10, 0, 0, 0, "Gain 10 Shield at combat start", 130 };

        // Bruiser Talismans
        itemDataMap["Horn for Battle"] = { "Horn for Battle", EquipmentSlot::Talisman, 0, 150, 0, 0, 0, 0, "Arena: Enemy has -15 Attack", 180 };
        itemDataMap["Map"] = { "Map", EquipmentSlot::Talisman, 0, 30, 0, 0, 0, 0, "Heal 2.5 HP per attack; Execute <25% HP", 200 };
        itemDataMap["Chain"] = { "Chain", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "+10 Health on hit", 150 };
        itemDataMap["Crystal"] = { "Crystal", EquipmentSlot::Talisman, 0, 150, 0, 0, 0, 0, "", 100 };
        itemDataMap["Money Shield"] = { "Money Shield", EquipmentSlot::Talisman, 0, 125, 0, 0, 0, 0, "Gain 65 Silver every 5th dungeon", 250 };
        itemDataMap["Scycle"] = { "Scycle", EquipmentSlot::Talisman, 20, 0, 0, 0, 0, 0, "+20 Silver on dungeon win", 180 };

        // Bruiser Companions
        itemDataMap["Rabbit"] = { "Rabbit", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "First attack is a guaranteed crit", 400 };
        itemDataMap["Turtle"] = { "Turtle", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+50 XP on dungeon win, Heal 3 HP/attack, gain Shield", 500 };
        itemDataMap["Hedgehog"] = { "Hedgehog", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Below 30% HP: Gain 40 Shield", 450 };
        itemDataMap["Iron Golem"] = { "Iron Golem", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Gain 20 Shield at combat start", 350 };
        itemDataMap["Bruiser Merchant Rat"] = { "Bruiser Merchant Rat", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Dungeon win: +25 Silver", 300 };

        // ============================================
        // RARE ITEM DATA - ASSASSIN
        // ============================================

        // Assassin Rare Weapons
        itemDataMap["Venomfang Blade"] = { "Venomfang Blade", EquipmentSlot::Weapon, 15, 0, 0, 0, 0, 0, "Apply Open Wound (enemy healing -40%)", 280 };
        itemDataMap["Phantom Edge"] = { "Phantom Edge", EquipmentSlot::Weapon, 25, 0, 25, 0, 0, 0, "Gain 30 Shield after first attack", 350 };
        itemDataMap["Shadowpiercer Crossbow"] = { "Shadowpiercer Crossbow", EquipmentSlot::Weapon, 20, 0, 0, 0, 0, 0, "+18 Armor Penetration", 320 };
        itemDataMap["Deathwhisper Dagger"] = { "Deathwhisper Dagger", EquipmentSlot::Weapon, 20, 0, 0, 0, 0, 0, "+10 Armor Penetration", 260 };
        itemDataMap["Rhythm Blade"] = { "Rhythm Blade", EquipmentSlot::Weapon, 10, 0, 0, 0, 0, 0, "Every 3rd attack guaranteed crit", 400 };
        itemDataMap["Soulreaver Bow"] = { "Soulreaver Bow", EquipmentSlot::Weapon, 30, 0, 0, 0, 0, 0, "Each attack deals +15 bonus damage", 450 };
        itemDataMap["Vampiric Fang"] = { "Vampiric Fang", EquipmentSlot::Weapon, 20, 0, 0, 0, 0, 0, "+10 HP on hit", 380 };
        itemDataMap["Shield Crusher"] = { "Shield Crusher", EquipmentSlot::Weapon, 20, 0, 0, 0, 0, 0, "+10 Shield, Attacks remove enemy shield", 340 };
        itemDataMap["Executioner's Blade"] = { "Executioner's Blade", EquipmentSlot::Weapon, 15, 250, 0, 0, 0, 0, "", 300 };
        itemDataMap["Armor Shredder"] = { "Armor Shredder", EquipmentSlot::Weapon, 20, 0, 0, 0, 0, 0, "+20 Armor Penetration", 380 };

        // Assassin Rare Helmets
        itemDataMap["Nightstalker's Cowl"] = { "Nightstalker's Cowl", EquipmentSlot::Helmet, 0, 100, 20, 0, 0, 0, "+10 Dex when below 50% HP", 280 };
        itemDataMap["Phantom Visage"] = { "Phantom Visage", EquipmentSlot::Helmet, 0, 0, 15, 0, 0, 0, "Dodge the third enemy attack", 320 };
        itemDataMap["Veil of Shadows"] = { "Veil of Shadows", EquipmentSlot::Helmet, 0, 0, 25, 0, 0, 0, "First crit deals +20 bonus damage", 300 };
        itemDataMap["Death's Shroud"] = { "Death's Shroud", EquipmentSlot::Helmet, 0, 150, 10, 0, 0, 0, "+5 Attack after each dodge", 340 };
        itemDataMap["Whisper Mask"] = { "Whisper Mask", EquipmentSlot::Helmet, 0, 0, 30, 0, 0, 0, "Gain 15 Shield when you crit", 360 };

        // Assassin Rare Chestplates
        itemDataMap["Shadowweave Armor"] = { "Shadowweave Armor", EquipmentSlot::Chestplate, 0, 0, 30, 0, 0, 0, "Gain 30 Shield at combat start", 380 };
        itemDataMap["Nightfall Vest"] = { "Nightfall Vest", EquipmentSlot::Chestplate, 0, 0, 40, 0, 0, 0, "+10 Attack when below 50% HP", 320 };
        itemDataMap["Cloak of Evasion"] = { "Cloak of Evasion", EquipmentSlot::Chestplate, 0, 0, 25, 0, 0, 0, "10% chance to dodge any attack", 400 };
        itemDataMap["Deathdealer's Garb"] = { "Deathdealer's Garb", EquipmentSlot::Chestplate, 0, 0, 35, 0, 0, 0, "+3 HP per attack", 350 };
        itemDataMap["Assassin's Shroud"] = { "Assassin's Shroud", EquipmentSlot::Chestplate, 0, 100, 20, 0, 0, 0, "First attack deals +25 damage", 380 };

        // Assassin Rare Leggings
        itemDataMap["Swift Striders"] = { "Swift Striders", EquipmentSlot::Pants, 0, 0, 20, 10, 0, 0, "+5 Dex after each attack", 300 };
        itemDataMap["Phantom Greaves"] = { "Phantom Greaves", EquipmentSlot::Pants, 0, 0, 25, 0, 0, 0, "Dodge second and fourth enemy attacks", 420 };
        itemDataMap["Venom-Laced Pants"] = { "Venom-Laced Pants", EquipmentSlot::Pants, 0, 0, 15, 0, 0, 0, "Poison: 5 dmg/turn for 3 turns", 340 };
        itemDataMap["Shadow Pants"] = { "Shadow Pants", EquipmentSlot::Pants, 0, 0, 30, 0, 0, 0, "+10 Attack after dodging", 360 };
        itemDataMap["Death's Step"] = { "Death's Step", EquipmentSlot::Pants, 0, 100, 20, 0, 0, 0, "+5% crit chance", 380 };

        // Assassin Rare Boots
        itemDataMap["Stalker's Treads"] = { "Stalker's Treads", EquipmentSlot::Boots, 15, 175, 0, 0, 0, 0, "", 280 };
        itemDataMap["Nightblade Boots"] = { "Nightblade Boots", EquipmentSlot::Boots, 0, 0, 20, 0, 0, 0, "+15 Attack on first attack", 320 };
        itemDataMap["Phantom Step"] = { "Phantom Step", EquipmentSlot::Boots, 0, 0, 25, 5, 0, 0, "Dodge first enemy attack", 380 };
        itemDataMap["Deathwalk Boots"] = { "Deathwalk Boots", EquipmentSlot::Boots, 15, 0, 15, 0, 0, 0, "+5 HP per crit", 340 };
        itemDataMap["Silent Runners"] = { "Silent Runners", EquipmentSlot::Boots, 0, 0, 30, 0, 0, 0, "+10 Armor Penetration", 300 };

        // Assassin Rare Talismans
        itemDataMap["Heart of Shadows"] = { "Heart of Shadows", EquipmentSlot::Talisman, 0, 250, 0, 0, 0, 0, "+5 Attack per kill (dungeon only)", 400 };
        itemDataMap["Venom Vial"] = { "Venom Vial", EquipmentSlot::Talisman, 0, 150, 0, 0, 0, 0, "Poison: 8 dmg/turn for 2 turns", 380 };
        itemDataMap["Dark Pendant"] = { "Dark Pendant", EquipmentSlot::Talisman, 0, 200, 0, 0, 0, 0, "+20 Attack when below 30% HP", 350 };
        itemDataMap["Assassin's Mark"] = { "Assassin's Mark", EquipmentSlot::Talisman, 0, 100, 0, 0, 0, 0, "+25% crit damage", 420 };
        itemDataMap["Shadow Crystal"] = { "Shadow Crystal", EquipmentSlot::Talisman, 0, 175, 0, 0, 0, 0, "Gain 20 Shield at combat start", 360 };

        // Assassin Rare Companions
        itemDataMap["Shadow Panther"] = { "Shadow Panther", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+15 Attack on first attack, +5 Dex per dodge", 550 };
        itemDataMap["Venomous Spider"] = { "Venomous Spider", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Apply poison on hit (4 dmg/turn, 3 turns)", 500 };
        itemDataMap["Ghost Raven"] = { "Ghost Raven", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "25% chance enemy misses first attack", 480 };
        itemDataMap["Blood Bat"] = { "Blood Bat", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+8 HP on hit, +100 XP on dungeon win", 520 };
        itemDataMap["Phantom Serpent"] = { "Phantom Serpent", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+10 Armor Pen, Crits apply Open Wound", 580 };

        // ============================================
        // RARE ITEM DATA - MAGE
        // ============================================

        // Mage Rare Weapons
        itemDataMap["Inferno Wand"] = { "Inferno Wand", EquipmentSlot::Weapon, 45, 0, 0, 0, 0, 0, "Deal 45 damage at combat start", 480 };
        itemDataMap["Wand of Devastation"] = { "Wand of Devastation", EquipmentSlot::Weapon, 25, 150, 0, 0, 0, 0, "Deal 25 damage at combat start", 400 };
        itemDataMap["Arcane Staff"] = { "Arcane Staff", EquipmentSlot::Weapon, 25, 70, 0, 0, 0, 0, "+7 Armor Pen, +5 Attack per spell", 450 };
        itemDataMap["Grand Rod"] = { "Grand Rod", EquipmentSlot::Weapon, 60, 0, 0, 0, 0, 0, "", 350 };
        itemDataMap["Bleeding Orb"] = { "Bleeding Orb", EquipmentSlot::Weapon, 30, 0, 0, 0, 0, 0, "Apply Open Wound on attack", 380 };
        itemDataMap["Mage Shield Piercer"] = { "Mage Shield Piercer", EquipmentSlot::Weapon, 15, 0, 0, 0, 0, 0, "+10 Shield, Attacks remove enemy shield", 340 };

        // Mage Rare Helmets
        itemDataMap["Mask of Embers"] = { "Mask of Embers", EquipmentSlot::Helmet, 30, 100, 0, 0, 0, 0, "Deal 2 damage to enemy at turn start", 380 };
        itemDataMap["Crown of Flames"] = { "Crown of Flames", EquipmentSlot::Helmet, 20, 0, 15, 0, 0, 0, "+10 spell damage when below 50% HP", 360 };
        itemDataMap["Archmage's Hood"] = { "Archmage's Hood", EquipmentSlot::Helmet, 0, 0, 25, 0, 20, 0, "+5 Intelligence per spell cast", 400 };
        itemDataMap["Veil of Sorcery"] = { "Veil of Sorcery", EquipmentSlot::Helmet, 0, 0, 30, 0, 0, 0, "Block third enemy attack", 350 };
        itemDataMap["Mindfire Circlet"] = { "Mindfire Circlet", EquipmentSlot::Helmet, 15, 100, 0, 0, 0, 0, "Crits deal +15 bonus damage", 340 };

        // Mage Rare Chestplates
        itemDataMap["Battlemage Cloak"] = { "Battlemage Cloak", EquipmentSlot::Chestplate, 0, 0, 45, 0, 10, 0, "", 320 };
        itemDataMap["Robes of the Inferno"] = { "Robes of the Inferno", EquipmentSlot::Chestplate, 0, 0, 35, 0, 0, 0, "Burn: 5 dmg/turn for 2 turns", 380 };
        itemDataMap["Arcane Vestment"] = { "Arcane Vestment", EquipmentSlot::Chestplate, 0, 0, 30, 0, 0, 0, "Gain 25 Shield at combat start", 400 };
        itemDataMap["Sorcerer's Mantle"] = { "Sorcerer's Mantle", EquipmentSlot::Chestplate, 0, 100, 25, 0, 0, 0, "+5 HP per spell cast", 360 };
        itemDataMap["Spellweaver's Robe"] = { "Spellweaver's Robe", EquipmentSlot::Chestplate, 0, 0, 40, 0, 0, 0, "+8 Attack per spell (max +40)", 420 };

        // Mage Rare Leggings
        itemDataMap["Mana-Woven Pants"] = { "Mana-Woven Pants", EquipmentSlot::Pants, 0, 0, 25, 0, 15, 0, "+5 Intelligence below 50% HP", 300 };
        itemDataMap["Flamecaster Leggings"] = { "Flamecaster Leggings", EquipmentSlot::Pants, 0, 0, 20, 0, 0, 0, "Burn: 4 dmg/turn for 2 turns", 320 };
        itemDataMap["Arcane Greaves"] = { "Arcane Greaves", EquipmentSlot::Pants, 0, 0, 30, 0, 0, 0, "Gain 15 Shield at combat start", 340 };
        itemDataMap["Sorcerer's Trousers"] = { "Sorcerer's Trousers", EquipmentSlot::Pants, 0, 150, 20, 0, 0, 0, "+3 HP per turn", 360 };
        itemDataMap["Runic Legwraps"] = { "Runic Legwraps", EquipmentSlot::Pants, 0, 0, 25, 0, 0, 0, "+5 Attack per crit", 300 };

        // Mage Rare Boots
        itemDataMap["Boots of the Archmage"] = { "Boots of the Archmage", EquipmentSlot::Boots, 20, 0, 20, 0, 0, 0, "First spell +15 damage", 380 };
        itemDataMap["Flamestep Boots"] = { "Flamestep Boots", EquipmentSlot::Boots, 25, 0, 0, 0, 0, 0, "Deal 10 damage at combat start", 340 };
        itemDataMap["Runic Treads"] = { "Runic Treads", EquipmentSlot::Boots, 0, 0, 30, 0, 15, 0, "", 300 };
        itemDataMap["Spellfire Slippers"] = { "Spellfire Slippers", EquipmentSlot::Boots, 15, 100, 0, 0, 0, 0, "+5 spell damage per turn", 360 };
        itemDataMap["Mage's Striders"] = { "Mage's Striders", EquipmentSlot::Boots, 0, 0, 35, 0, 0, 0, "+10 Armor Penetration", 320 };

        // Mage Rare Talismans
        itemDataMap["Flame Orb"] = { "Flame Orb", EquipmentSlot::Talisman, 30, 0, 0, 0, 0, 0, "Deal 8 damage at combat start", 300 };
        itemDataMap["Arcane Chains"] = { "Arcane Chains", EquipmentSlot::Talisman, 40, 0, 25, 0, 0, 0, "", 350 };
        itemDataMap["Golden Aegis"] = { "Golden Aegis", EquipmentSlot::Talisman, 0, 0, 20, 0, 0, 0, "First enemy attack deals 0 damage", 450 };
        itemDataMap["Arcane Reliquary"] = { "Arcane Reliquary", EquipmentSlot::Talisman, 0, 150, 25, 0, 0, 0, "+10 Shield at combat start", 380 };
        itemDataMap["Orb of Devastation"] = { "Orb of Devastation", EquipmentSlot::Talisman, 0, 0, 0, 0, 0, 0, "Every 3rd attack deals +50 bonus damage", 500 };
        itemDataMap["Crystal of Vitality"] = { "Crystal of Vitality", EquipmentSlot::Talisman, 0, 250, 0, 0, 0, 0, "After attack, heal 2.5% of max HP", 420 };
        itemDataMap["Runic Pendant"] = { "Runic Pendant", EquipmentSlot::Talisman, 30, 0, 0, 0, 0, 0, "+12 Armor Penetration", 340 };

        // Mage Rare Companions
        itemDataMap["Phoenix Hatchling"] = { "Phoenix Hatchling", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "Deal 20 damage at start, +5 HP per turn", 580 };
        itemDataMap["Arcane Golem"] = { "Arcane Golem", EquipmentSlot::Companion, 0, 0, 0, 0, 10, 0, "+25 Shield at combat start", 520 };
        itemDataMap["Fire Elemental"] = { "Fire Elemental", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "All attacks apply Burn (3 dmg/turn, 2 turns)", 500 };
        itemDataMap["Mana Wyrm"] = { "Mana Wyrm", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+8 Attack per spell cast (max +40)", 480 };
        itemDataMap["Crystal Guardian"] = { "Crystal Guardian", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+30 Shield at start, block one attack below 25% HP", 550 };

        // ============================================
        // RARE ITEM DATA - BRUISER
        // ============================================

        // Bruiser Rare Weapons
        itemDataMap["Colossus Hammer"] = { "Colossus Hammer", EquipmentSlot::Weapon, 15, 200, 0, 0, 0, 0, "+5 HP on hit", 380 };
        itemDataMap["Brutal Cleaver"] = { "Brutal Cleaver", EquipmentSlot::Weapon, 25, 0, 0, 0, 0, 0, "+5 Armor Penetration", 300 };
        itemDataMap["Titan's Axe"] = { "Titan's Axe", EquipmentSlot::Weapon, 20, 0, 0, 0, 0, 0, "Every 5th attack guaranteed crit", 400 };
        itemDataMap["Bruiser Shield Basher"] = { "Bruiser Shield Basher", EquipmentSlot::Weapon, 20, 0, 0, 0, 0, 0, "+10 Shield, Attacks remove enemy shield", 340 };
        itemDataMap["Warlord's Mace"] = { "Warlord's Mace", EquipmentSlot::Weapon, 30, 100, 0, 0, 0, 0, "+3 HP per attack", 420 };
        itemDataMap["Bone Crusher"] = { "Bone Crusher", EquipmentSlot::Weapon, 35, 0, 0, 0, 0, 0, "Shred 5 armor per hit (max 25)", 380 };

        // Bruiser Rare Helmets
        itemDataMap["Titan's Visage"] = { "Titan's Visage", EquipmentSlot::Helmet, 0, 150, 30, 0, 0, 0, "+5 HP when hit", 380 };
        itemDataMap["Juggernaut Helm"] = { "Juggernaut Helm", EquipmentSlot::Helmet, 0, 100, 40, 0, 0, 0, "First enemy attack deals -10 damage", 400 };
        itemDataMap["Warlord's Crown"] = { "Warlord's Crown", EquipmentSlot::Helmet, 0, 200, 20, 0, 0, 0, "+5 Attack when below 50% HP", 360 };
        itemDataMap["Berserker's Mask"] = { "Berserker's Mask", EquipmentSlot::Helmet, 0, 0, 35, 0, 0, 0, "+10 Attack when below 30% HP", 340 };
        itemDataMap["Fortress Helm"] = { "Fortress Helm", EquipmentSlot::Helmet, 0, 0, 50, 0, 0, 0, "Gain 20 Shield at combat start", 420 };

        // Bruiser Rare Chestplates
        itemDataMap["Spiked Bulwark"] = { "Spiked Bulwark", EquipmentSlot::Chestplate, 0, 0, 30, 0, 0, 0, "Reflect 15 damage when hit, apply Open Wound", 450 };
        itemDataMap["Ironclad Vest"] = { "Ironclad Vest", EquipmentSlot::Chestplate, 0, 0, 50, 0, 0, 0, "+5 HP per attack", 400 };
        itemDataMap["Titan's Plate"] = { "Titan's Plate", EquipmentSlot::Chestplate, 0, 100, 40, 0, 0, 0, "", 350 };
        itemDataMap["Warlord's Mail"] = { "Warlord's Mail", EquipmentSlot::Chestplate, 0, 0, 40, 0, 0, 0, "First attack deals -20% damage", 380 };
        itemDataMap["Berserker's Harness"] = { "Berserker's Harness", EquipmentSlot::Chestplate, 0, 0, 30, 0, 0, 0, "+5 Attack per hit taken (max +25)", 420 };
        itemDataMap["Fortress Armor"] = { "Fortress Armor", EquipmentSlot::Chestplate, 0, 0, 55, 0, 0, 0, "", 300 };

        // Bruiser Rare Leggings
        itemDataMap["Titan's Greaves"] = { "Titan's Greaves", EquipmentSlot::Pants, 0, 200, 0, 0, 0, 0, "+5 HP per turn", 380 };
        itemDataMap["Juggernaut Pants"] = { "Juggernaut Pants", EquipmentSlot::Pants, 0, 0, 35, 0, 0, 0, "Reduce all damage by 5%", 400 };
        itemDataMap["Warlord's Pants"] = { "Warlord's Pants", EquipmentSlot::Pants, 0, 150, 20, 0, 0, 0, "+3 HP per attack", 360 };
        itemDataMap["Berserker's Legguards"] = { "Berserker's Legguards", EquipmentSlot::Pants, 0, 100, 25, 0, 0, 0, "+5 Attack when below 50% HP", 340 };
        itemDataMap["Fortress Greaves"] = { "Fortress Greaves", EquipmentSlot::Pants, 0, 250, 0, 0, 0, 0, "+10 Shield at combat start", 380 };

        // Bruiser Rare Boots
        itemDataMap["Titan's Treads"] = { "Titan's Treads", EquipmentSlot::Boots, 0, 175, 20, 0, 0, 0, "+3 HP per turn", 380 };
        itemDataMap["Juggernaut Boots"] = { "Juggernaut Boots", EquipmentSlot::Boots, 0, 0, 40, 0, 0, 0, "Reduce damage by 8%", 400 };
        itemDataMap["Warlord's Stompers"] = { "Warlord's Stompers", EquipmentSlot::Boots, 0, 150, 0, 0, 0, 0, "+15 Shield at combat start", 340 };
        itemDataMap["Berserker's Boots"] = { "Berserker's Boots", EquipmentSlot::Boots, 0, 0, 30, 0, 0, 0, "+10 Attack when below 40% HP", 320 };
        itemDataMap["Fortress Steps"] = { "Fortress Steps", EquipmentSlot::Boots, 0, 100, 35, 0, 0, 0, "", 300 };

        // Bruiser Rare Talismans
        itemDataMap["Heart of the Mountain"] = { "Heart of the Mountain", EquipmentSlot::Talisman, 0, 300, 0, 0, 0, 0, "+10 HP on hit", 450 };
        itemDataMap["Warlord's Emblem"] = { "Warlord's Emblem", EquipmentSlot::Talisman, 0, 250, 0, 0, 0, 0, "Apply Open Wound on attack", 400 };
        itemDataMap["Titan's Heart"] = { "Titan's Heart", EquipmentSlot::Talisman, 0, 350, 0, 0, 0, 0, "+5 Shield per turn", 480 };
        itemDataMap["Berserker's Totem"] = { "Berserker's Totem", EquipmentSlot::Talisman, 0, 200, 0, 0, 0, 0, "+15 Attack when below 30% HP", 380 };
        itemDataMap["Stone of Vitality"] = { "Stone of Vitality", EquipmentSlot::Talisman, 0, 250, 0, 0, 0, 0, "+3% max HP heal per attack", 420 };
        itemDataMap["Iron Pendant"] = { "Iron Pendant", EquipmentSlot::Talisman, 0, 200, 30, 0, 0, 0, "", 350 };

        // Bruiser Rare Companions
        itemDataMap["War Bear"] = { "War Bear", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+40 Shield at combat start, +5 HP per attack", 580 };
        itemDataMap["Iron Guardian"] = { "Iron Guardian", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+55 Shield at combat start", 500 };
        itemDataMap["Stone Tortoise"] = { "Stone Tortoise", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+30 Shield at start, +5 HP per turn, +75 XP on win", 550 };
        itemDataMap["Berserker Wolf"] = { "Berserker Wolf", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+10 Attack per hit taken (max +30), First attack crits", 520 };
        itemDataMap["Mountain Golem"] = { "Mountain Golem", EquipmentSlot::Companion, 0, 0, 0, 0, 0, 0, "+25 Shield at start, Reduce damage by 5%", 480 };
    }

    map<string, EquipmentSlot> itemSlots;
    map<string, ItemData> itemDataMap;
    map<string, int> itemPrices;

    void initializeItemPrices() {
        // Prices are now in itemDataMap, but keeping this for backwards compatibility
        for (const auto& pair : itemDataMap) {
            itemPrices[pair.first] = pair.second.price;
        }
    }
};