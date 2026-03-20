#include "Shop.h"
#include "Inventory.h"
#include "CurrencyManager.h"
#include <iostream>
#include <random>
#include <algorithm>

Shop::Shop(float width, float height)
    : visible(false), windowWidth(width), windowHeight(height), playerClassIndex(-1), hoveredItemIndex(-1), tooltipText(font) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for Shop!" << endl;
    }

    // Initialize shop background - full height, aligned to right
    float shopWidth = 1600.0f;
    float shopHeight = 1080.0f;
    shopBackground.setSize({ shopWidth, shopHeight });
    shopBackground.setFillColor(Color(20, 20, 40, 255));  // Solid, not transparent
    shopBackground.setOutlineThickness(0.0f);
    shopBackground.setPosition({
        width - shopWidth,  // Align to right
        0.0f
        });

    // Title
    titleText = new Text(font);
    titleText->setFont(font);
    titleText->setString("SHOP");
    titleText->setCharacterSize(60);
    titleText->setFillColor(Color(255, 215, 0));
    titleText->setStyle(Text::Bold);
    FloatRect titleBounds = titleText->getLocalBounds();
    titleText->setOrigin({ titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f });
    titleText->setPosition({ width - shopWidth / 2.0f, 50.0f });

    // Refresh button (higher up, above items, centered)
    // Calculate where items will be
    float itemHeight = 180.0f;
    float itemSpacing = 40.0f;
    float bottomMargin = 80.0f;
    float itemsStartY = shopHeight - bottomMargin - (2 * itemHeight) - itemSpacing;

    refreshButton.setSize({ 200.0f, 60.0f });
    refreshButton.setFillColor(Color(0, 100, 200));
    refreshButton.setOutlineThickness(3.0f);
    refreshButton.setOutlineColor(Color::White);
    refreshButton.setPosition({
        width - shopWidth / 2.0f - 100.0f,  // Center horizontally in shop
        itemsStartY - 180.0f  // Higher - more space above items
        });

    refreshButtonText = new Text(font);
    refreshButtonText->setFont(font);
    refreshButtonText->setString("Refresh Shop");
    refreshButtonText->setCharacterSize(28);
    refreshButtonText->setFillColor(Color::White);
    refreshButtonText->setStyle(Text::Bold);
    FloatRect refreshBounds = refreshButtonText->getLocalBounds();
    refreshButtonText->setOrigin({ refreshBounds.size.x / 2.0f, refreshBounds.size.y / 2.0f });
    refreshButtonText->setPosition({
        refreshButton.getPosition().x + 100.0f,
        refreshButton.getPosition().y + 30.0f
        });

    // Refresh cost text (below refresh button)
    refreshCostText = new Text(font);
    refreshCostText->setFont(font);
    refreshCostText->setString("Cost: 1 Gold");
    refreshCostText->setCharacterSize(22);
    refreshCostText->setFillColor(Color(255, 215, 0));
    FloatRect costBounds = refreshCostText->getLocalBounds();
    refreshCostText->setOrigin({ costBounds.size.x / 2.0f, costBounds.size.y / 2.0f });
    refreshCostText->setPosition({
        refreshButton.getPosition().x + 100.0f,
        refreshButton.getPosition().y + 75.0f
        });

    // Time until refresh text (below cost)
    timeUntilRefreshText = new Text(font);
    timeUntilRefreshText->setFont(font);
    timeUntilRefreshText->setCharacterSize(20);
    timeUntilRefreshText->setFillColor(Color(200, 200, 200));
    timeUntilRefreshText->setPosition({
        width - shopWidth / 2.0f - 150.0f,
        refreshButton.getPosition().y + 110.0f
        });


    // Cheat button (top right corner of shop)
    cheatButton.setSize({ 150.0f, 50.0f });
    cheatButton.setFillColor(Color(50, 200, 50));
    cheatButton.setOutlineThickness(3.0f);
    cheatButton.setOutlineColor(Color::White);
    cheatButton.setPosition({
        width - 170.0f,  // Top right corner
        20.0f
        });

    cheatButtonText.setString("+1000 Silver");
    cheatButtonText.setCharacterSize(22);
    cheatButtonText.setFillColor(Color::White);
    cheatButtonText.setStyle(Text::Bold);
    FloatRect cheatBounds = cheatButtonText.getLocalBounds();
    cheatButtonText.setOrigin({ cheatBounds.size.x / 2.0f, cheatBounds.size.y / 2.0f });
    cheatButtonText.setPosition({
        cheatButton.getPosition().x + 75.0f,
        cheatButton.getPosition().y + 25.0f
        });

    // Setup tooltip
    tooltipBox.setFillColor(Color(20, 20, 20, 240));
    tooltipBox.setOutlineThickness(2.0f);
    tooltipBox.setOutlineColor(Color(255, 215, 0));
    tooltipText.setCharacterSize(20);
    tooltipText.setFillColor(Color::White);
    // Initialize item pool and generate first set of items
    initializeItemPool();
    lastRefreshTime = time(nullptr);
    generateShopItems();
}

Shop::~Shop() {
    delete titleText;
    delete refreshButtonText;
    delete refreshCostText;
    delete timeUntilRefreshText;
}

void Shop::initializeItemPool() {
    ShopItem item;

    // ASSASSIN WEAPONS (Class 0 only)
    item.name = "Broken Sword";
    item.description = "+10 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 20;
    item.iconPath = "";
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Short Blade";
    item.description = "+10 Attack, +80 Health\nHeal 2.5 HP after every attack";
    item.rarity = ItemRarity::Common;
    item.price = 30;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Dagger";
    item.description = "+15 Attack, +15 Dexterity\nEach hit shreds 3 armor (max 15)";
    item.rarity = ItemRarity::Common;
    item.price = 35;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "The Farmer's Scythe";
    item.description = "+17 Attack, +3 Health on hit\nGain 10 Silver on dungeon finish";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Long Blade";
    item.description = "+30 Attack, +150 Health";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Basic Sword";
    item.description = "+40 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Stun";
    item.description = "Stun enemy and attack again";
    item.rarity = ItemRarity::Common;
    item.price = 500;
    item.classIndex = 0;
    itemPool.push_back(item);

    // ASSASSIN HELMETS (Class 0 only)
    item.name = "Silent Hood";
    item.description = "+10 Armor\nEvery crit deals +8 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 80;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Nightblade Mask";
    item.description = "+10 Armor\nDodge second enemy attack";
    item.rarity = ItemRarity::Common;
    item.price = 90;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Rogue's Cap";
    item.description = "+12 Armor, +5 Attack\nHeal 2 HP after crit";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shadow Hood";
    item.description = "+15 Armor, +5 Dexterity\n+5 Dex for 1 turn after dodge";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Assassin's Helm";
    item.description = "+15 Armor\n+5 Dex below 50% HP";
    item.rarity = ItemRarity::Common;
    item.price = 110;
    item.classIndex = 0;
    itemPool.push_back(item);


    // ASSASSIN CHESTPLATES (Class 0 only)
    item.name = "Vest";
    item.description = "+25 Armor";
    item.rarity = ItemRarity::Common;
    item.price = 110;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Silent Armor";
    item.description = "+15 Armor\nGain +5 Dex when attacked";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Thief's Cloak";
    item.description = "+20 Armor\nDodge first boss/elite attack";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Rogue's Garb";
    item.description = "+20 Armor, +5 Dexterity\n+15 Attack below 50% HP";
    item.rarity = ItemRarity::Common;
    item.price = 180;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Nightstalker Vest";
    item.description = "+25 Armor\n+5 Attack per crit in combat";
    item.rarity = ItemRarity::Common;
    item.price = 200;
    item.classIndex = 0;
    itemPool.push_back(item);


    // ASSASSIN LEGGINGS (Class 0 only)
    item.name = "Poisoned Trousers";
    item.description = "+10 Armor\nPoison: 5 dmg/turn for 3 turns";
    item.rarity = ItemRarity::Common;
    item.price = 90;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Nightblade Pants";
    item.description = "+12 Armor, +5 Attack\n+5 Attack below 50% HP";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shadow Leggings";
    item.description = "+15 Armor\n+5 Dex after each attack";
    item.rarity = ItemRarity::Common;
    item.price = 130;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Rogue's Greaves";
    item.description = "+15 Armor\n+5 Dex after crit";
    item.rarity = ItemRarity::Common;
    item.price = 110;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Silent Steps";
    item.description = "+10 Armor, +10 Dexterity\nDodge second enemy attack";
    item.rarity = ItemRarity::Common;
    item.price = 115;
    item.classIndex = 0;
    itemPool.push_back(item);


    // ASSASSIN BOOTS (Class 0 only)
    item.name = "Leather Boots";
    item.description = "+20 Armor";
    item.rarity = ItemRarity::Common;
    item.price = 25;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Long Boots";
    item.description = "+25 Armor";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Dynamites Boots";
    item.description = "+30 Attack\n+10 Armor Penetration";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Boots of Healing";
    item.description = "+20 Attack\n+4 Health per attack";
    item.rarity = ItemRarity::Common;
    item.price = 160;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shadowstep Boots";
    item.description = "+25 Attack, +10 Dexterity\nBelow 50% HP: +15 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 0;
    itemPool.push_back(item);


    // ASSASSIN TALISMANS (Class 0 only)
    item.name = "Heart of Steel";
    item.description = "+100 Health, +10 Armor";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Bloodstone";
    item.description = "+10 Attack while below 30% HP";
    item.rarity = ItemRarity::Common;
    item.price = 200;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Metal";
    item.description = "+100 Health\nGain 20 Shield at start of combat";
    item.rarity = ItemRarity::Common;
    item.price = 350;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Flask";
    item.description = "Second enemy attack deals 0 damage";
    item.rarity = ItemRarity::Common;
    item.price = 250;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Vial of Venom";
    item.description = "Poison: 5 dmg/turn for 3 turns";
    item.rarity = ItemRarity::Common;
    item.price = 450;
    item.classIndex = 0;
    itemPool.push_back(item);

    // ASSASSIN COMPANIONS (Class 0 only)
    item.name = "Merchant Rat";
    item.description = "Dungeon wins grant +40 Silver";
    item.rarity = ItemRarity::Common;
    item.price = 350;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shadow Rat";
    item.description = "+10 Dex per dodge in combat";
    item.rarity = ItemRarity::Common;
    item.price = 400;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Crows of Misfortune";
    item.description = "First enemy attack: 20% miss chance";
    item.rarity = ItemRarity::Common;
    item.price = 500;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Rogue Cat";
    item.description = "First attack guaranteed crit";
    item.rarity = ItemRarity::Common;
    item.price = 550;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Wolf";
    item.description = "Dungeon win: +80 XP\nHeal 3 HP per attack";
    item.rarity = ItemRarity::Common;
    item.price = 600;
    item.classIndex = 0;
    itemPool.push_back(item);

    // ============================================
    // MAGE ITEMS (Class 2)
    // ============================================

    // MAGE WEAPONS (Class 2)
    item.name = "Dark Ring";
    item.description = "+15 Attack, +50 Health\nArena: +12 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 40;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Book of Secrets";
    item.description = "+20 Attack\n+5 damage to burning enemies";
    item.rarity = ItemRarity::Common;
    item.price = 60;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Lost Ring";
    item.description = "+18 Attack, +90 Health\nNormal attacks +5 damage";
    item.rarity = ItemRarity::Common;
    item.price = 80;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Book of Healing";
    item.description = "+5 Attack\n+10 Health on hit";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Stick";
    item.description = "+45 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Rod";
    item.description = "+50 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 2;
    itemPool.push_back(item);

    // MAGE HELMETS (Class 2)
    item.name = "Wizard's Hat";
    item.description = "+10 Armor, +10 Intelligence\nFirst spell +5 damage";
    item.rarity = ItemRarity::Common;
    item.price = 80;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Hood";
    item.description = "+12 Intelligence\n+5 Intelligence after hit";
    item.rarity = ItemRarity::Common;
    item.price = 90;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Scholar's Cap";
    item.description = "+10 Intelligence\n+5 Attack after spell crit";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mage's Circlet";
    item.description = "+8 Armor, +8 Intelligence\nHeal 5 HP on spell crit";
    item.rarity = ItemRarity::Common;
    item.price = 110;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Enchanted Hood";
    item.description = "+10 Armor, +5 Intelligence\nSecond enemy attack deals 0 damage";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 2;
    itemPool.push_back(item);

    // MAGE CHESTPLATES (Class 2)
    item.name = "Apprentice Robe";
    item.description = "+20 Armor";
    item.rarity = ItemRarity::Common;
    item.price = 80;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Pyromancer's Cloak";
    item.description = "+15 Armor\nBurn: 3 dmg/turn for 2 turns";
    item.rarity = ItemRarity::Common;
    item.price = 140;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mage's Mantle";
    item.description = "+18 Armor, +8 Intelligence\nHeal 3 HP when hit";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Robe of Shielding";
    item.description = "+20 Armor\nGain 15 Shield when below 50% HP";
    item.rarity = ItemRarity::Common;
    item.price = 170;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Vestments";
    item.description = "+25 Armor\n+5 Attack per spell";
    item.rarity = ItemRarity::Common;
    item.price = 180;
    item.classIndex = 2;
    itemPool.push_back(item);

    // MAGE LEGGINGS (Class 2)
    item.name = "Scholar's Trousers";
    item.description = "+12 Armor\nHeal 2 HP per spell";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Pyromancer's Leggings";
    item.description = "+10 Armor\nBurn: 2 dmg/turn for 2 turns";
    item.rarity = ItemRarity::Common;
    item.price = 110;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mana Flow Pants";
    item.description = "+8 Armor, +10 Intelligence\n+5 Intelligence below 50% HP";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Pants";
    item.description = "+10 Armor, +12 Intelligence\nGain 20 Shield at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 130;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mystic Legwraps";
    item.description = "+8 Armor, +15 Intelligence\n+3 Attack per spell";
    item.rarity = ItemRarity::Common;
    item.price = 140;
    item.classIndex = 2;
    itemPool.push_back(item);

    // MAGE BOOTS (Class 2)
    item.name = "Magical Boots";
    item.description = "+12 Intelligence";
    item.rarity = ItemRarity::Common;
    item.price = 50;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Enchanted Boots";
    item.description = "+18 Intelligence";
    item.rarity = ItemRarity::Common;
    item.price = 90;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Boots of Witch";
    item.description = "+40 Attack\n+8 Armor Penetration";
    item.rarity = ItemRarity::Common;
    item.price = 130;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Boots of the Pyromancer";
    item.description = "+30 Attack\n+5 Armor Pen, Deal 5 damage at start";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Slippers";
    item.description = "+25 Attack, +12 Intelligence\nFirst spell +10 damage";
    item.rarity = ItemRarity::Common;
    item.price = 160;
    item.classIndex = 2;
    itemPool.push_back(item);

    // MAGE TALISMANS (Class 2)
    item.name = "Some Other Crystal";
    item.description = "+15 Attack, +15 Intelligence";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Ancient Coin";
    item.description = "+80 Health\nGain 30 Silver every 3rd dungeon";
    item.rarity = ItemRarity::Common;
    item.price = 180;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Orb of the Fortune Teller";
    item.description = "+30 Attack, +80 Health";
    item.rarity = ItemRarity::Common;
    item.price = 200;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "GOLDEN ARMOR";
    item.description = "First enemy attack deals 0 damage";
    item.rarity = ItemRarity::Common;
    item.price = 280;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Big CRYSTAL";
    item.description = "+30 Attack, +10 Armor\nGain 20 Shield at start";
    item.rarity = ItemRarity::Common;
    item.price = 300;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Meteor";
    item.description = "Deal 25 damage at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 350;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "STAR";
    item.description = "Deal 50 damage at start (Arena only)";
    item.rarity = ItemRarity::Common;
    item.price = 400;
    item.classIndex = 2;
    itemPool.push_back(item);

    // MAGE COMPANIONS (Class 2)
    item.name = "Mana Sprite";
    item.description = "+10 Intelligence in combat";
    item.rarity = ItemRarity::Common;
    item.price = 400;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Familiar";
    item.description = "5% chance any enemy attack deals 0";
    item.rarity = ItemRarity::Common;
    item.price = 480;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Fire Imp";
    item.description = "+5 spell damage per spell (max +25)";
    item.rarity = ItemRarity::Common;
    item.price = 500;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Crystal Owl";
    item.description = "Restore 5 HP per turn";
    item.rarity = ItemRarity::Common;
    item.price = 550;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Fox";
    item.description = "Dungeon win: +80 XP\nHeal 3 HP per attack";
    item.rarity = ItemRarity::Common;
    item.price = 600;
    item.classIndex = 2;
    itemPool.push_back(item);

    // ============================================
    // BRUISER ITEMS (Class 1)
    // ============================================

    // BRUISER WEAPONS (Class 1)
    item.name = "Battle Shield";
    item.description = "+110 Health\nHeal 4 HP after attack";
    item.rarity = ItemRarity::Common;
    item.price = 80;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Pickaxe";
    item.description = "+15 Attack\n+5 Attack per hit (max +25)";
    item.rarity = ItemRarity::Common;
    item.price = 60;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Mallet";
    item.description = "+8 Attack\n+8 Health on hit";
    item.rarity = ItemRarity::Common;
    item.price = 70;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Tomahawk";
    item.description = "+25 Attack, +130 Health";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Hammer";
    item.description = "+35 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 90;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Hammer of George";
    item.description = "+25 Attack, +150 Health";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Pointy Stick";
    item.description = "+40 Attack, +20 Armor";
    item.rarity = ItemRarity::Common;
    item.price = 140;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Enchanted Shield";
    item.description = "+35 Shield at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 200;
    item.classIndex = 1;
    itemPool.push_back(item);

    // BRUISER HELMETS (Class 1)
    item.name = "Iron Helm";
    item.description = "+30 Armor\nReduce first enemy attack by 5";
    item.rarity = ItemRarity::Common;
    item.price = 80;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Battle Helm";
    item.description = "+25 Armor, +10 Health\nHeal 3 HP when hit";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Warrior's Cap";
    item.description = "+20 Armor\nGain 10 Shield at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 90;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Guardian Helm";
    item.description = "+25 Armor\nFirst enemy attack deals 0 damage";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Heavy Plate Helm";
    item.description = "+50 Health";
    item.rarity = ItemRarity::Common;
    item.price = 70;
    item.classIndex = 1;
    itemPool.push_back(item);

    // BRUISER CHESTPLATES (Class 1)
    item.name = "Light Vest";
    item.description = "+15 Armor\n+3 HP per attack";
    item.rarity = ItemRarity::Common;
    item.price = 60;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Enchanted Armor";
    item.description = "+25 Shield at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Guardian Plate";
    item.description = "+30 Armor\nHeal 3 HP after attack";
    item.rarity = ItemRarity::Common;
    item.price = 140;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Juggernaut Armor";
    item.description = "+35 Health\nReduce first enemy attack by 5";
    item.rarity = ItemRarity::Common;
    item.price = 130;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Cloak";
    item.description = "+20 Armor, +10 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 1;
    itemPool.push_back(item);

    // BRUISER LEGGINGS (Class 1)
    item.name = "Iron Legguards";
    item.description = "+25 Armor\nReduce damage taken by 3%";
    item.rarity = ItemRarity::Common;
    item.price = 90;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Stone Greaves";
    item.description = "+20 Armor, +10 Health\nGain 5 Shield at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Vanguard Pants";
    item.description = "+15 Armor, +5 Health on hit\nHeal 2 HP per attack";
    item.rarity = ItemRarity::Common;
    item.price = 110;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Juggernaut Leggings";
    item.description = "+20 Armor\nEnemy attacks deal -2 damage";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Titan's Legguards";
    item.description = "+25 Health\nGain 5 HP per turn";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 1;
    itemPool.push_back(item);

    // BRUISER BOOTS (Class 1)
    item.name = "Boots of Steel";
    item.description = "+30 Armor\nReduce damage taken by 10%";
    item.rarity = ItemRarity::Common;
    item.price = 160;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Boots of Mage";
    item.description = "+35 Armor\nGain Shield vs Mage enemies";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Boots of Health";
    item.description = "+150 Health";
    item.rarity = ItemRarity::Common;
    item.price = 120;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Iron Treads";
    item.description = "+30 Armor\nReduce damage taken by 5%";
    item.rarity = ItemRarity::Common;
    item.price = 110;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Boulder Boots";
    item.description = "+25 Health, +10 Armor\nGain 10 Shield at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 130;
    item.classIndex = 1;
    itemPool.push_back(item);

    // BRUISER TALISMANS (Class 1)
    item.name = "Horn for Battle";
    item.description = "+150 Health\nArena: Enemy has -15 Attack";
    item.rarity = ItemRarity::Common;
    item.price = 180;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Map";
    item.description = "+30 Health\nHeal 2.5 HP per attack; Execute enemies below 25% HP";
    item.rarity = ItemRarity::Common;
    item.price = 200;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Chain";
    item.description = "+10 Health on hit";
    item.rarity = ItemRarity::Common;
    item.price = 150;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Crystal";
    item.description = "+150 Health";
    item.rarity = ItemRarity::Common;
    item.price = 100;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Money Shield";
    item.description = "+125 Health\nGain 65 Silver every 5th dungeon";
    item.rarity = ItemRarity::Common;
    item.price = 250;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Scycle";
    item.description = "+20 Attack\n+20 Silver on dungeon win";
    item.rarity = ItemRarity::Common;
    item.price = 180;
    item.classIndex = 1;
    itemPool.push_back(item);

    // BRUISER COMPANIONS (Class 1)
    item.name = "Rabbit";
    item.description = "First attack is a guaranteed crit";
    item.rarity = ItemRarity::Common;
    item.price = 400;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Turtle";
    item.description = "+50 XP on dungeon win\nHeal 3 HP per attack, gain Shield";
    item.rarity = ItemRarity::Common;
    item.price = 500;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Hedgehog";
    item.description = "Below 30% HP: Gain 40 Shield";
    item.rarity = ItemRarity::Common;
    item.price = 450;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Iron Golem";
    item.description = "Gain 20 Shield at combat start";
    item.rarity = ItemRarity::Common;
    item.price = 350;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Bruiser Merchant Rat";
    item.description = "Dungeon win: +25 Silver";
    item.rarity = ItemRarity::Common;
    item.price = 300;
    item.classIndex = 1;
    itemPool.push_back(item);

    // ============================================
    // RARE ITEMS - ASSASSIN (Class 0)
    // ============================================

    // Assassin Rare Weapons
    item.name = "Venomfang Blade";
    item.description = "+15 Attack\nApply Open Wound (enemy healing -40%)";
    item.rarity = ItemRarity::Rare;
    item.price = 280;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Phantom Edge";
    item.description = "+25 Attack, +25 Armor\nGain 30 Shield after first attack";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shadowpiercer Crossbow";
    item.description = "+20 Attack\n+18 Armor Penetration";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Deathwhisper Dagger";
    item.description = "+20 Attack\n+10 Armor Penetration";
    item.rarity = ItemRarity::Rare;
    item.price = 260;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Rhythm Blade";
    item.description = "+10 Attack\nEvery 3rd attack is a guaranteed crit";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Soulreaver Bow";
    item.description = "+30 Attack\nEach attack deals +15 bonus damage";
    item.rarity = ItemRarity::Rare;
    item.price = 450;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Vampiric Fang";
    item.description = "+20 Attack\n+10 HP on hit";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shield Crusher";
    item.description = "+20 Attack, +10 Shield\nAttacks remove enemy shield";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Executioner's Blade";
    item.description = "+15 Attack, +250 Health";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Armor Shredder";
    item.description = "+20 Attack\n+20 Armor Penetration";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 0;
    itemPool.push_back(item);

    // Assassin Rare Helmets
    item.name = "Nightstalker's Cowl";
    item.description = "+20 Armor, +100 Health\n+10 Dexterity when below 50% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 280;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Phantom Visage";
    item.description = "+15 Armor\nDodge the third enemy attack";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Veil of Shadows";
    item.description = "+25 Armor\nFirst crit deals +20 bonus damage";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Death's Shroud";
    item.description = "+10 Armor, +150 Health\n+5 Attack after each dodge";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Whisper Mask";
    item.description = "+30 Armor\nGain 15 Shield when you crit";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 0;
    itemPool.push_back(item);

    // Assassin Rare Chestplates
    item.name = "Shadowweave Armor";
    item.description = "+30 Armor\nGain 30 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Nightfall Vest";
    item.description = "+40 Armor\n+10 Attack when below 50% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Cloak of Evasion";
    item.description = "+25 Armor\n10% chance to dodge any attack";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Deathdealer's Garb";
    item.description = "+35 Armor\n+3 HP per attack";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Assassin's Shroud";
    item.description = "+20 Armor, +100 Health\nFirst attack deals +25 damage";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 0;
    itemPool.push_back(item);

    // Assassin Rare Leggings
    item.name = "Swift Striders";
    item.description = "+20 Armor, +10 Dex\n+5 Dex after each attack";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Phantom Greaves";
    item.description = "+25 Armor\nDodge second and fourth enemy attacks";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Venom-Laced Pants";
    item.description = "+15 Armor\nPoison: 5 dmg/turn for 3 turns";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shadow Pants";
    item.description = "+30 Armor\n+10 Attack after dodging";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Death's Step";
    item.description = "+20 Armor, +100 Health\n+5% crit chance";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 0;
    itemPool.push_back(item);

    // Assassin Rare Boots
    item.name = "Stalker's Treads";
    item.description = "+15 Attack, +175 Health";
    item.rarity = ItemRarity::Rare;
    item.price = 280;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Nightblade Boots";
    item.description = "+20 Armor\n+15 Attack on first attack";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Phantom Step";
    item.description = "+25 Armor\nDodge first enemy attack, +5 Dex";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Deathwalk Boots";
    item.description = "+15 Attack, +15 Armor\n+5 HP per crit";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Silent Runners";
    item.description = "+30 Armor\n+10 Armor Penetration";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 0;
    itemPool.push_back(item);

    // Assassin Rare Talismans
    item.name = "Heart of Shadows";
    item.description = "+250 Health\n+5 Attack per kill (dungeon only)";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Venom Vial";
    item.description = "+150 Health\nPoison: 8 dmg/turn for 2 turns";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Dark Pendant";
    item.description = "+200 Health\n+20 Attack when below 30% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Assassin's Mark";
    item.description = "+100 Health\n+25% crit damage";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Shadow Crystal";
    item.description = "+175 Health\nGain 20 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 0;
    itemPool.push_back(item);

    // Assassin Rare Companions
    item.name = "Shadow Panther";
    item.description = "+15 Attack on first attack\n+5 Dex per dodge";
    item.rarity = ItemRarity::Rare;
    item.price = 550;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Venomous Spider";
    item.description = "Apply poison on hit\n(4 dmg/turn, 3 turns)";
    item.rarity = ItemRarity::Rare;
    item.price = 500;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Ghost Raven";
    item.description = "25% chance enemy misses first attack";
    item.rarity = ItemRarity::Rare;
    item.price = 480;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Blood Bat";
    item.description = "+8 HP on hit\n+100 XP on dungeon win";
    item.rarity = ItemRarity::Rare;
    item.price = 520;
    item.classIndex = 0;
    itemPool.push_back(item);

    item.name = "Phantom Serpent";
    item.description = "+10 Armor Pen\nCrits apply Open Wound";
    item.rarity = ItemRarity::Rare;
    item.price = 580;
    item.classIndex = 0;
    itemPool.push_back(item);

    // ============================================
    // RARE ITEMS - MAGE (Class 2)
    // ============================================

    // Mage Rare Weapons
    item.name = "Inferno Wand";
    item.description = "+45 Attack\nDeal 45 damage at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 480;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Wand of Devastation";
    item.description = "+25 Attack, +150 Health\nDeal 25 damage at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Staff";
    item.description = "+25 Attack, +70 Health\n+7 Armor Pen, +5 Attack per spell";
    item.rarity = ItemRarity::Rare;
    item.price = 450;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Grand Rod";
    item.description = "+60 Attack";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Bleeding Orb";
    item.description = "+30 Attack\nApply Open Wound on attack";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mage Shield Piercer";
    item.description = "+15 Attack, +10 Shield\nAttacks remove enemy shield";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 2;
    itemPool.push_back(item);

    // Mage Rare Helmets
    item.name = "Mask of Embers";
    item.description = "+30 Attack, +100 Health\nDeal 2 damage to enemy at turn start";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Crown of Flames";
    item.description = "+20 Attack, +15 Armor\n+10 spell damage when below 50% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Archmage's Hood";
    item.description = "+25 Armor, +20 Intel\n+5 Intelligence per spell cast";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Veil of Sorcery";
    item.description = "+30 Armor\nBlock third enemy attack";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mindfire Circlet";
    item.description = "+15 Attack, +100 Health\nCrits deal +15 bonus damage";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 2;
    itemPool.push_back(item);

    // Mage Rare Chestplates
    item.name = "Battlemage Cloak";
    item.description = "+45 Armor\n+10 Intelligence";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Robes of the Inferno";
    item.description = "+35 Armor\nBurn: 5 dmg/turn for 2 turns";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Vestment";
    item.description = "+30 Armor\nGain 25 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Sorcerer's Mantle";
    item.description = "+25 Armor, +100 Health\n+5 HP per spell cast";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Spellweaver's Robe";
    item.description = "+40 Armor\n+8 Attack per spell (max +40)";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 2;
    itemPool.push_back(item);

    // Mage Rare Leggings
    item.name = "Mana-Woven Pants";
    item.description = "+25 Armor, +15 Intel\n+5 Intelligence below 50% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Flamecaster Leggings";
    item.description = "+20 Armor\nBurn: 4 dmg/turn for 2 turns";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Greaves";
    item.description = "+30 Armor\nGain 15 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Sorcerer's Trousers";
    item.description = "+20 Armor, +150 Health\n+3 HP per turn";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Runic Legwraps";
    item.description = "+25 Armor\n+5 Attack per crit";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 2;
    itemPool.push_back(item);

    // Mage Rare Boots
    item.name = "Boots of the Archmage";
    item.description = "+20 Attack, +20 Armor\nFirst spell +15 damage";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Flamestep Boots";
    item.description = "+25 Attack\nDeal 10 damage at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Runic Treads";
    item.description = "+30 Armor\n+15 Intelligence";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Spellfire Slippers";
    item.description = "+15 Attack, +100 Health\n+5 spell damage per turn";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mage's Striders";
    item.description = "+35 Armor\n+10 Armor Penetration";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 2;
    itemPool.push_back(item);

    // Mage Rare Talismans
    item.name = "Flame Orb";
    item.description = "+30 Attack\nDeal 8 damage at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Chains";
    item.description = "+40 Attack, +25 Armor";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Golden Aegis";
    item.description = "+20 Armor\nFirst enemy attack deals 0 damage";
    item.rarity = ItemRarity::Rare;
    item.price = 450;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Reliquary";
    item.description = "+150 Health, +25 Armor\n+10 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Orb of Devastation";
    item.description = "Every 3rd attack deals +50 bonus damage";
    item.rarity = ItemRarity::Rare;
    item.price = 500;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Crystal of Vitality";
    item.description = "+250 Health\nAfter attack, heal 2.5% of max HP";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Runic Pendant";
    item.description = "+30 Attack\n+12 Armor Penetration";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 2;
    itemPool.push_back(item);

    // Mage Rare Companions
    item.name = "Phoenix Hatchling";
    item.description = "Deal 20 damage at start\n+5 HP per turn";
    item.rarity = ItemRarity::Rare;
    item.price = 580;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Arcane Golem";
    item.description = "+25 Shield at combat start\n+10 Intelligence";
    item.rarity = ItemRarity::Rare;
    item.price = 520;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Fire Elemental";
    item.description = "All attacks apply Burn\n(3 dmg/turn, 2 turns)";
    item.rarity = ItemRarity::Rare;
    item.price = 500;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Mana Wyrm";
    item.description = "+8 Attack per spell cast\n(max +40)";
    item.rarity = ItemRarity::Rare;
    item.price = 480;
    item.classIndex = 2;
    itemPool.push_back(item);

    item.name = "Crystal Guardian";
    item.description = "+30 Shield at start\nBlock one attack below 25% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 550;
    item.classIndex = 2;
    itemPool.push_back(item);

    // ============================================
    // RARE ITEMS - BRUISER (Class 1)
    // ============================================

    // Bruiser Rare Weapons
    item.name = "Colossus Hammer";
    item.description = "+15 Attack, +200 Health\n+5 HP on hit";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Brutal Cleaver";
    item.description = "+25 Attack\n+5 Armor Penetration";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Titan's Axe";
    item.description = "+20 Attack\nEvery 5th attack is a guaranteed crit";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Bruiser Shield Basher";
    item.description = "+20 Attack, +10 Shield\nAttacks remove enemy shield";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Warlord's Mace";
    item.description = "+30 Attack, +100 Health\n+3 HP per attack";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Bone Crusher";
    item.description = "+35 Attack\nShred 5 armor per hit (max 25)";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    // Bruiser Rare Helmets
    item.name = "Titan's Visage";
    item.description = "+150 Health, +30 Armor\n+5 HP when hit";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Juggernaut Helm";
    item.description = "+100 Health, +40 Armor\nFirst enemy attack deals -10 damage";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Warlord's Crown";
    item.description = "+200 Health, +20 Armor\n+5 Attack when below 50% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Berserker's Mask";
    item.description = "+35 Armor\n+10 Attack when below 30% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Fortress Helm";
    item.description = "+50 Armor\nGain 20 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 1;
    itemPool.push_back(item);

    // Bruiser Rare Chestplates
    item.name = "Spiked Bulwark";
    item.description = "+30 Armor\nReflect 15 damage when hit, apply Open Wound";
    item.rarity = ItemRarity::Rare;
    item.price = 450;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Ironclad Vest";
    item.description = "+50 Armor\n+5 HP per attack";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Titan's Plate";
    item.description = "+40 Armor, +100 Health";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Warlord's Mail";
    item.description = "+40 Armor\nFirst attack deals -20% damage";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Berserker's Harness";
    item.description = "+30 Armor\n+5 Attack per hit taken (max +25)";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Fortress Armor";
    item.description = "+55 Armor";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 1;
    itemPool.push_back(item);

    // Bruiser Rare Leggings
    item.name = "Titan's Greaves";
    item.description = "+200 Health\n+5 HP per turn";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Juggernaut Pants";
    item.description = "+35 Armor\nReduce all damage by 5%";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Warlord's Pants";
    item.description = "+150 Health, +20 Armor\n+3 HP per attack";
    item.rarity = ItemRarity::Rare;
    item.price = 360;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Berserker's Legguards";
    item.description = "+25 Armor, +100 Health\n+5 Attack when below 50% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Fortress Greaves";
    item.description = "+250 Health\n+10 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    // Bruiser Rare Boots
    item.name = "Titan's Treads";
    item.description = "+175 Health, +20 Armor\n+3 HP per turn";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Juggernaut Boots";
    item.description = "+40 Armor\nReduce damage by 8%";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Warlord's Stompers";
    item.description = "+150 Health\n+15 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 340;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Berserker's Boots";
    item.description = "+30 Armor\n+10 Attack when below 40% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 320;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Fortress Steps";
    item.description = "+35 Armor, +100 Health";
    item.rarity = ItemRarity::Rare;
    item.price = 300;
    item.classIndex = 1;
    itemPool.push_back(item);

    // Bruiser Rare Talismans
    item.name = "Heart of the Mountain";
    item.description = "+300 Health\n+10 HP on hit";
    item.rarity = ItemRarity::Rare;
    item.price = 450;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Warlord's Emblem";
    item.description = "+250 Health\nApply Open Wound on attack";
    item.rarity = ItemRarity::Rare;
    item.price = 400;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Titan's Heart";
    item.description = "+350 Health\n+5 Shield per turn";
    item.rarity = ItemRarity::Rare;
    item.price = 480;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Berserker's Totem";
    item.description = "+200 Health\n+15 Attack when below 30% HP";
    item.rarity = ItemRarity::Rare;
    item.price = 380;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Stone of Vitality";
    item.description = "+250 Health\n+3% max HP heal per attack";
    item.rarity = ItemRarity::Rare;
    item.price = 420;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Iron Pendant";
    item.description = "+200 Health, +30 Armor";
    item.rarity = ItemRarity::Rare;
    item.price = 350;
    item.classIndex = 1;
    itemPool.push_back(item);

    // Bruiser Rare Companions
    item.name = "War Bear";
    item.description = "+40 Shield at combat start\n+5 HP per attack";
    item.rarity = ItemRarity::Rare;
    item.price = 580;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Iron Guardian";
    item.description = "+55 Shield at combat start";
    item.rarity = ItemRarity::Rare;
    item.price = 500;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Stone Tortoise";
    item.description = "+30 Shield at start\n+5 HP per turn, +75 XP on win";
    item.rarity = ItemRarity::Rare;
    item.price = 550;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Berserker Wolf";
    item.description = "+10 Attack per hit taken (max +30)\nFirst attack crits";
    item.rarity = ItemRarity::Rare;
    item.price = 520;
    item.classIndex = 1;
    itemPool.push_back(item);

    item.name = "Mountain Golem";
    item.description = "+25 Shield at start\nReduce damage by 5%";
    item.rarity = ItemRarity::Rare;
    item.price = 480;
    item.classIndex = 1;
    itemPool.push_back(item);
}

ItemRarity Shop::rollRarity() {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<float> dis(0.0f, 1.0f);

    float roll = dis(gen);

    if (roll < LEGENDARY_CHANCE) {
        return ItemRarity::Legendary;
    }
    else if (roll < LEGENDARY_CHANCE + MYTHIC_CHANCE) {
        return ItemRarity::Mythic;
    }
    else if (roll < LEGENDARY_CHANCE + MYTHIC_CHANCE + RARE_CHANCE) {
        return ItemRarity::Rare;
    }
    else {
        return ItemRarity::Common;
    }
}

void Shop::generateShopItems() {
    currentItems.clear();

    // Generate 6 items based on rarity percentages
    for (int i = 0; i < MAX_SHOP_ITEMS; ++i) {
        ItemRarity rolledRarity = rollRarity();

        // Filter item pool by rarity AND class
        vector<ShopItem> possibleItems;
        for (const auto& item : itemPool) {
            // Check rarity match
            if (item.rarity == rolledRarity) {
                // Check class match: -1 = all classes, or matches player class
                if (item.classIndex == -1 || item.classIndex == playerClassIndex) {
                    possibleItems.push_back(item);
                }
            }
        }

        // If no items of that rarity exist for this class, fall back to common items
        if (possibleItems.empty()) {
            for (const auto& item : itemPool) {
                if (item.rarity == ItemRarity::Common) {
                    // Check class match: -1 = all classes, or matches player class
                    if (item.classIndex == -1 || item.classIndex == playerClassIndex) {
                        possibleItems.push_back(item);
                    }
                }
            }
        }

        // Pick a random item from the filtered list
        if (!possibleItems.empty()) {
            static random_device rd;
            static mt19937 gen(rd());
            uniform_int_distribution<> dis(0, static_cast<int>(possibleItems.size()) - 1);

            ShopItem selectedItem = possibleItems[dis(gen)];
            selectedItem.isPurchased = false;
            currentItems.push_back(selectedItem);
        }
    }

    createItemVisuals();
}

void Shop::createItemVisuals() {
    // Clear old visuals
    itemVisuals.clear();

    // Layout constants - smaller item boxes at bottom
    float itemWidth = 350.0f;
    float itemHeight = 180.0f;
    float itemSpacing = 40.0f;
    float shopX = windowWidth - 1600.0f;  // Shop starts here (aligned right)
    float bottomMargin = 80.0f;

    // Calculate starting X to center items in shop area
    float totalWidth = (ITEMS_PER_ROW * itemWidth) + ((ITEMS_PER_ROW - 1) * itemSpacing);
    float startX = shopX + (1600.0f - totalWidth) / 2.0f;

    // Items start from bottom and go up
    float startY = 1080.0f - bottomMargin - (2 * itemHeight) - itemSpacing;

    for (int i = 0; i < currentItems.size(); ++i) {
        int row = i / ITEMS_PER_ROW;
        int col = i % ITEMS_PER_ROW;

        float x = startX + col * (itemWidth + itemSpacing);
        float y = startY + row * (itemHeight + itemSpacing);

        ShopItemVisual visual;
        visual.x = x;
        visual.y = y;

        // Item box
        visual.itemBox.setSize({ itemWidth, itemHeight });
        visual.itemBox.setPosition({ x, y });
        visual.itemBox.setOutlineThickness(4.0f);

        // Set colors based on rarity (solid)
        Color boxColor, outlineColor;

        switch (currentItems[i].rarity) {
        case ItemRarity::Common:
            boxColor = Color(60, 60, 60, 255);
            outlineColor = Color(150, 150, 150);
            break;
        case ItemRarity::Rare:
            boxColor = Color(0, 50, 100, 255);
            outlineColor = Color(100, 150, 255);
            break;
        case ItemRarity::Mythic:
            boxColor = Color(80, 0, 80, 255);
            outlineColor = Color(200, 100, 255);
            break;
        case ItemRarity::Legendary:
            boxColor = Color(100, 60, 0, 255);
            outlineColor = Color(255, 215, 0);
            break;
        }

        visual.itemBox.setFillColor(boxColor);
        visual.itemBox.setOutlineColor(outlineColor);

        itemVisuals.push_back(visual);
    }
}

void Shop::checkAutoRefresh() {
    time_t currentTime = time(nullptr);
    double secondsSinceRefresh = difftime(currentTime, lastRefreshTime);
    double hoursSinceRefresh = secondsSinceRefresh / 3600.0;

    if (hoursSinceRefresh >= 24) {
        generateShopItems();
        lastRefreshTime = currentTime;
    }
}

void Shop::update() {
    if (!visible) return;

    checkAutoRefresh();

    // Update time until refresh text
    time_t currentTime = time(nullptr);
    double secondsSinceRefresh = difftime(currentTime, lastRefreshTime);
    double secondsUntilRefresh = (24 * 3600.0) - secondsSinceRefresh;

    if (secondsUntilRefresh > 0) {
        int hoursLeft = static_cast<int>(secondsUntilRefresh / 3600.0);
        int minutesLeft = static_cast<int>((secondsUntilRefresh - (hoursLeft * 3600)) / 60.0);

        timeUntilRefreshText->setString(
            "Auto-refresh in: " + to_string(hoursLeft) + "h " + to_string(minutesLeft) + "m"
        );
    }
    else {
        timeUntilRefreshText->setString("Shop will refresh soon!");
    }
}


string wrapText(const string& text, const Font& font, unsigned int charSize, float maxWidth) {
    string wrappedText;
    string currentLine;
    string word;

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];

        if (c == ' ' || c == '\n' || i == text.length() - 1) {
            if (i == text.length() - 1 && c != ' ' && c != '\n') {
                word += c;
            }

            string testLine = currentLine.empty() ? word : currentLine + " " + word;
            Text testText(font);
            testText.setString(testLine);
            testText.setCharacterSize(charSize);

            if (testText.getLocalBounds().size.x <= maxWidth) {
                currentLine = testLine;
            }
            else {
                if (!currentLine.empty()) {
                    wrappedText += currentLine + "\n";
                    currentLine = word;
                }
                else {
                    wrappedText += word + "\n";
                    currentLine = "";
                }
            }

            if (c == '\n') {
                wrappedText += currentLine + "\n";
                currentLine = "";
            }

            word = "";
        }
        else {
            word += c;
        }
    }

    if (!currentLine.empty()) {
        wrappedText += currentLine;
    }

    return wrappedText;
}
void Shop::draw(RenderWindow& window) {
    if (!visible) return;

    // Draw shop background
    window.draw(shopBackground);
    window.draw(*titleText);

    // Draw refresh button and text
    window.draw(refreshButton);
    window.draw(*refreshButtonText);
    window.draw(*refreshCostText);
    window.draw(*timeUntilRefreshText);

    // Draw cheat button
    window.draw(cheatButton);
    window.draw(cheatButtonText);

    // Draw all items
    for (int i = 0; i < itemVisuals.size(); ++i) {
        const auto& visual = itemVisuals[i];
        const auto& item = currentItems[i];

        // Draw the box
        window.draw(visual.itemBox);

        // Determine text color based on rarity
        Color textColor;
        string rarityName;

        switch (item.rarity) {
        case ItemRarity::Common:
            textColor = Color::White;
            rarityName = "Common";
            break;
        case ItemRarity::Rare:
            textColor = Color(100, 150, 255);
            rarityName = "Rare";
            break;
        case ItemRarity::Mythic:
            textColor = Color(200, 100, 255);
            rarityName = "Mythic";
            break;
        case ItemRarity::Legendary:
            textColor = Color(255, 215, 0);
            rarityName = "Legendary";
            break;
        }

        // Create and draw name text
        Text nameText(font);
        nameText.setString(item.name);
        nameText.setCharacterSize(26);
        nameText.setFillColor(textColor);
        nameText.setStyle(Text::Bold);
        nameText.setPosition({ visual.x + 10.0f, visual.y + 10.0f });
        window.draw(nameText);

        // Create and draw rarity text
        Text rarityText(font);
        rarityText.setString(rarityName);
        rarityText.setCharacterSize(18);
        rarityText.setFillColor(textColor);
        rarityText.setPosition({ visual.x + 10.0f, visual.y + 45.0f });
        window.draw(rarityText);

        // Create and draw description text (wrapped)
        Text descText(font);
        string wrappedDesc = wrapText(item.description, font, 18, 330.0f);
        descText.setString(wrappedDesc);
        descText.setCharacterSize(18);
        descText.setFillColor(Color::White);
        descText.setPosition({ visual.x + 10.0f, visual.y + 75.0f });
        window.draw(descText);

        // Create and draw price text
        Text priceText(font);
        priceText.setString(to_string(item.price) + " Silver");
        priceText.setCharacterSize(24);
        priceText.setFillColor(Color(200, 200, 200));
        priceText.setStyle(Text::Bold);
        priceText.setPosition({ visual.x + 10.0f, visual.y + 150.0f });
        window.draw(priceText);

        // If purchased, draw "SOLD" overlay
        if (item.isPurchased) {
            FloatRect itemBoxBounds = visual.itemBox.getGlobalBounds();

            RectangleShape soldOverlay;
            soldOverlay.setSize({ itemBoxBounds.size.x, itemBoxBounds.size.y });
            soldOverlay.setPosition(itemBoxBounds.position);
            soldOverlay.setFillColor(Color(0, 0, 0, 150));
            window.draw(soldOverlay);

            Text soldText(font);
            soldText.setString("SOLD");
            soldText.setCharacterSize(50);
            soldText.setFillColor(Color::Red);
            soldText.setStyle(Text::Bold);
            FloatRect soldBounds = soldText.getLocalBounds();
            soldText.setOrigin({ soldBounds.size.x / 2.0f, soldBounds.size.y / 2.0f });
            soldText.setPosition({
                itemBoxBounds.position.x + itemBoxBounds.size.x / 2.0f,
                itemBoxBounds.position.y + itemBoxBounds.size.y / 2.0f
                });
            window.draw(soldText);
        }
    }

    // Draw tooltip if hovering over an item
    if (hoveredItemIndex >= 0 && hoveredItemIndex < currentItems.size()) {
        const auto& item = currentItems[hoveredItemIndex];
        const auto& visual = itemVisuals[hoveredItemIndex];

        // Prepare tooltip text
        tooltipText.setString(item.description);
        FloatRect textBounds = tooltipText.getLocalBounds();

        // Calculate tooltip size with padding
        float padding = 15.0f;
        float tooltipWidth = textBounds.size.x + (padding * 2);
        float tooltipHeight = textBounds.size.y + (padding * 2);

        // Position tooltip above the item
        float tooltipX = visual.x;
        float tooltipY = visual.y - tooltipHeight - 10.0f;

        // Keep tooltip on screen
        if (tooltipY < 0) tooltipY = visual.y + 190.0f;
        if (tooltipX + tooltipWidth > windowWidth) tooltipX = windowWidth - tooltipWidth;

        tooltipBox.setSize({ tooltipWidth, tooltipHeight });
        tooltipBox.setPosition({ tooltipX, tooltipY });
        tooltipText.setPosition({ tooltipX + padding, tooltipY + padding });

        window.draw(tooltipBox);
        window.draw(tooltipText);
    }
}

bool Shop::checkItemClick(const RenderWindow& window, CurrencyManager& currency, Inventory* inventory) {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);

    for (int i = 0; i < currentItems.size(); ++i) {
        auto& item = currentItems[i];
        const auto& visual = itemVisuals[i];

        if (!item.isPurchased && visual.itemBox.getGlobalBounds().contains({
            static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {

            // FIXED: Use CurrencyManager methods to update both value AND display
            if (currency.getSilver() >= item.price) {
                currency.removeSilver(item.price);
                item.isPurchased = true;

                cout << "Purchased: " << item.name << " for " << item.price << " silver!" << endl;

                // Add item to inventory
                if (inventory) {
                    inventory->addItem(item.name);
                }

                return true;
            }
            else {
                cout << "Not enough silver! Need " << item.price << " but have " << currency.getSilver() << endl;
            }
        }
    }

    return false;
}

bool Shop::checkRefreshClick(const RenderWindow& window, CurrencyManager& currency) {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);

    if (refreshButton.getGlobalBounds().contains({
        static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {

        // FIXED: Use CurrencyManager methods to update both value AND display
        if (currency.getGold() >= 1) {
            currency.removeGold(1);
            forceRefresh();
            cout << "Shop refreshed! Cost 1 gold. Remaining gold: " << currency.getGold() << endl;
            return true;
        }
        else {
            cout << "Not enough gold! Need 1 gold to refresh shop." << endl;
        }
    }

    return false;
}

bool Shop::checkCloseClick(const RenderWindow& window) {
    // No close button anymore
    return false;
}

void Shop::checkHover(const RenderWindow& window) {
    if (!visible) return;

    Vector2i mousePos = Mouse::getPosition(window);

    // Reset hovered item
    hoveredItemIndex = -1;

    // Hover effect for refresh button
    if (refreshButton.getGlobalBounds().contains({
        static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {
        refreshButton.setFillColor(Color(0, 150, 255));
    }
    else {
        refreshButton.setFillColor(Color(0, 100, 200));
    }

    // Hover effect for cheat button
    if (cheatButton.getGlobalBounds().contains({
        static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {
        cheatButton.setFillColor(Color(70, 255, 70));
    }
    else {
        cheatButton.setFillColor(Color(50, 200, 50));
    }

    // Hover effect for items
    for (int i = 0; i < currentItems.size(); ++i) {
        const auto& item = currentItems[i];
        auto& visual = itemVisuals[i];

        if (!item.isPurchased && visual.itemBox.getGlobalBounds().contains({
            static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)
            })) {

            hoveredItemIndex = i;

            Color currentColor = visual.itemBox.getFillColor();
            // Brighten the color slightly on hover
            visual.itemBox.setFillColor(Color(
                min(255, currentColor.r + 30),
                min(255, currentColor.g + 30),
                min(255, currentColor.b + 30),
                currentColor.a
            ));
        }
        else {
            // Reset to original color based on rarity
            Color boxColor;
            switch (item.rarity) {
            case ItemRarity::Common:
                boxColor = Color(60, 60, 60, 220);
                break;
            case ItemRarity::Rare:
                boxColor = Color(0, 50, 100, 220);
                break;
            case ItemRarity::Mythic:
                boxColor = Color(80, 0, 80, 220);
                break;
            case ItemRarity::Legendary:
                boxColor = Color(100, 60, 0, 220);
                break;
            }
            visual.itemBox.setFillColor(boxColor);
        }
    }
}

void Shop::forceRefresh() {
    generateShopItems();
    lastRefreshTime = time(nullptr);
}

void Shop::show() {
    visible = true;
}

void Shop::hide() {
    visible = false;
}


bool Shop::checkCheatClick(const RenderWindow& window, CurrencyManager& currency) {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);

    if (cheatButton.getGlobalBounds().contains({
        static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) })) {

        currency.addSilver(1000);
        cout << "CHEAT: Added 1000 silver! Total silver: " << currency.getSilver() << endl;
        return true;
    }

    return false;
}
// Helper to find item in pool by name
ShopItem* Shop::findItemInPool(const string& name) {
    for (auto& item : itemPool) {
        if (item.name == name) {
            return &item;
        }
    }
    return nullptr;
}

// Save shop state for per-character persistence
void Shop::saveState(ShopSaveData& saveData) const {
    saveData.itemNames.clear();
    saveData.itemRarities.clear();
    saveData.itemPrices.clear();
    saveData.itemPurchased.clear();
    saveData.itemClassIndices.clear();

    for (const auto& item : currentItems) {
        saveData.itemNames.push_back(item.name);
        saveData.itemRarities.push_back(static_cast<int>(item.rarity));
        saveData.itemPrices.push_back(item.price);
        saveData.itemPurchased.push_back(item.isPurchased);
        saveData.itemClassIndices.push_back(item.classIndex);
    }

    saveData.lastResetDate = lastResetDate;

    cout << "[SHOP SAVE] Saved " << currentItems.size() << " shop items" << endl;
}

// Load shop state from saved data
void Shop::loadState(const ShopSaveData& saveData) {
    // If the saved state is empty or invalid, generate new shop
    if (!hasValidSavedState(saveData)) {
        cout << "[SHOP LOAD] No valid saved state, generating new shop items" << endl;
        generateShopItems();
        lastResetDate = getCurrentDate();
        return;
    }

    // Check if we need to auto-refresh based on the saved date
    lastResetDate = saveData.lastResetDate;
    if (shouldReset()) {
        // Time to refresh - generate new items
        cout << "[SHOP LOAD] Daily reset triggered, generating new shop items" << endl;
        generateShopItems();
        lastResetDate = getCurrentDate();
        return;
    }

    // Load the saved shop items
    currentItems.clear();

    for (size_t i = 0; i < saveData.itemNames.size(); ++i) {
        ShopItem item;
        item.name = saveData.itemNames[i];
        item.rarity = static_cast<ItemRarity>(saveData.itemRarities[i]);
        item.price = saveData.itemPrices[i];
        item.isPurchased = saveData.itemPurchased[i];
        item.classIndex = saveData.itemClassIndices[i];

        // Get description from item pool
        ShopItem* poolItem = findItemInPool(item.name);
        if (poolItem) {
            item.description = poolItem->description;
            item.iconPath = poolItem->iconPath;
        }
        else {
            item.description = "Unknown item";
            item.iconPath = "";
        }

        currentItems.push_back(item);
    }

    // Recreate visuals for the loaded items
    createItemVisuals();

    cout << "[SHOP LOAD] Loaded " << currentItems.size() << " shop items from save" << endl;
}

// Check if saved state is valid
bool Shop::hasValidSavedState(const ShopSaveData& saveData) const {
    // Check if we have any saved items
    if (saveData.itemNames.empty()) {
        return false;
    }

    // Check if all vectors have the same size
    if (saveData.itemNames.size() != saveData.itemRarities.size() ||
        saveData.itemNames.size() != saveData.itemPrices.size() ||
        saveData.itemNames.size() != saveData.itemPurchased.size() ||
        saveData.itemNames.size() != saveData.itemClassIndices.size()) {
        return false;
    }

    return true;
}