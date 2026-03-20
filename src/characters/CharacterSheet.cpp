#include "CharacterSheet.h"
#include <iostream>
#include <cmath>
#include "Faction.h"
#include "ItemDatabase.h"

CharacterSheet::CharacterSheet(float winWidth, float winHeight,
    int race, int charClass, Faction fact)
    : windowWidth(winWidth), windowHeight(winHeight),
    raceIndex(race), classIndex(charClass), faction(fact),
    visible(false), critChance(25), critDamage(175), dodgeChance(0.0),
    level(1), currentXP(0.0), xpToNextLevel(100.0), inventory(nullptr), currentMount(nullptr),
    hasCurse(false), hasFirstCurse(false), unlockedClass(false), needsCurseSelection(false) {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for character sheet!" << endl;
    }

    loadRaceStats();
    setupUI();
    setupInventoryDisplay();

    // Initialize tooltip
    hoveredSlotIndex = -1;
    hoveredInventoryIndex = -1;
    tooltipBox.setFillColor(Color(20, 20, 20, 240));
    tooltipBox.setOutlineThickness(2.0f);
    tooltipBox.setOutlineColor(Color(255, 215, 0));
    tooltipText.setCharacterSize(18);
    tooltipText.setFillColor(Color::White);

    // Initialize unequip button tracking
    selectedEquipmentSlot = -1;
    lastClickedSlot = -1;
    doubleClickTime = 0.3f;

    // Setup unequip button
    unequipButton.setSize({ 150.f, 40.f });
    unequipButton.setFillColor(Color(180, 50, 50));
    unequipButton.setOutlineThickness(2.f);
    unequipButton.setOutlineColor(Color::White);

    unequipButtonText = Text(font);
    unequipButtonText.setString("Unequip");
    unequipButtonText.setCharacterSize(20);
    unequipButtonText.setFillColor(Color::White);

    // Initialize inventory item selection
    selectedInventoryIndex = -1;
    selectedInventoryItem = "";
    lastClickedInventoryIndex = -1;

    // Setup Equip button (green)
    equipButton.setSize({ 80.f, 30.f });
    equipButton.setFillColor(Color(50, 180, 50));
    equipButton.setOutlineThickness(2.f);
    equipButton.setOutlineColor(Color::White);

    equipButtonText = Text(font);
    equipButtonText.setString("Equip");
    equipButtonText.setCharacterSize(16);
    equipButtonText.setFillColor(Color::White);

    // Setup Sell button (orange)
    sellButton.setSize({ 80.f, 30.f });
    sellButton.setFillColor(Color(200, 150, 50));
    sellButton.setOutlineThickness(2.f);
    sellButton.setOutlineColor(Color::White);

    sellButtonText = Text(font);
    sellButtonText.setString("Sell");
    sellButtonText.setCharacterSize(16);
    sellButtonText.setFillColor(Color::White);

    // Setup sell confirmation popup
    showingSellConfirmation = false;
    pendingSellPrice = 0;

    confirmPopupBackground.setSize({ windowWidth, windowHeight });
    confirmPopupBackground.setFillColor(Color(0, 0, 0, 150));
    confirmPopupBackground.setPosition({ 0.f, 0.f });

    float popupWidth = 400.f;
    float popupHeight = 200.f;
    confirmPopupBox.setSize({ popupWidth, popupHeight });
    confirmPopupBox.setFillColor(Color(40, 40, 60, 250));
    confirmPopupBox.setOutlineThickness(3.f);
    confirmPopupBox.setOutlineColor(Color(255, 215, 0));
    confirmPopupBox.setPosition({
        (windowWidth - popupWidth) / 2.f,
        (windowHeight - popupHeight) / 2.f
        });

    confirmPopupText = Text(font);
    confirmPopupText.setString("Are you sure?");
    confirmPopupText.setCharacterSize(32);
    confirmPopupText.setFillColor(Color::White);
    // Position popup text centered
    FloatRect popupTextBounds = confirmPopupText.getLocalBounds();
    confirmPopupText.setOrigin({ popupTextBounds.size.x / 2.f, popupTextBounds.size.y / 2.f });
    confirmPopupText.setPosition({
        windowWidth / 2.f,
        confirmPopupBox.getPosition().y + 50.f
        });

    confirmSureButton.setSize({ 120.f, 50.f });
    confirmSureButton.setFillColor(Color(50, 180, 50));
    confirmSureButton.setOutlineThickness(2.f);
    confirmSureButton.setOutlineColor(Color::White);
    confirmSureButton.setPosition({
        confirmPopupBox.getPosition().x + 50.f,
        confirmPopupBox.getPosition().y + 120.f
        });

    confirmSureText = Text(font);
    confirmSureText.setString("Sure");
    confirmSureText.setCharacterSize(24);
    confirmSureText.setFillColor(Color::White);
    // Position Sure text centered on button
    FloatRect sureBounds = confirmSureText.getLocalBounds();
    confirmSureText.setOrigin({ sureBounds.size.x / 2.f, sureBounds.size.y / 2.f });
    confirmSureText.setPosition({
        confirmSureButton.getPosition().x + 60.f,
        confirmSureButton.getPosition().y + 25.f
        });

    confirmNoButton.setSize({ 120.f, 50.f });
    confirmNoButton.setFillColor(Color(180, 50, 50));
    confirmNoButton.setOutlineThickness(2.f);
    confirmNoButton.setOutlineColor(Color::White);
    confirmNoButton.setPosition({
        confirmPopupBox.getPosition().x + popupWidth - 170.f,
        confirmPopupBox.getPosition().y + 120.f
        });

    confirmNoText = Text(font);
    confirmNoText.setString("No");
    confirmNoText.setCharacterSize(24);
    confirmNoText.setFillColor(Color::White);
    // Position No text centered on button
    FloatRect noBounds = confirmNoText.getLocalBounds();
    confirmNoText.setOrigin({ noBounds.size.x / 2.f, noBounds.size.y / 2.f });
    confirmNoText.setPosition({
        confirmNoButton.getPosition().x + 60.f,
        confirmNoButton.getPosition().y + 25.f
        });
}

CharacterSheet::~CharacterSheet() {
}

void CharacterSheet::loadRaceStats() {
    if (faction == Faction::Entente) {
        baseStats = getEntenteRaceStats(raceIndex);
    }
    else {
        baseStats = getThrongRaceStats(raceIndex);
    }

    // Apply class bonuses
    applyClassBonuses();

    // Apply stat conversions
    applyStatConversions();

    // Calculate dodge chance (Assassin only)
    calculateDodgeChance();

    // Calculate crit chance based on luck
    calculateCritChance();
}

void CharacterSheet::applyClassBonuses() {
    switch (classIndex) {
    case 0: // Assassin - Speed & Agility
        baseStats.health += 4.0;
        baseStats.attack += 1.0;
        baseStats.intelligence += 0.0;
        baseStats.dexterity += 3.0;
        break;
    case 1: // Bruiser - Tank & Strength
        baseStats.health += 7.0;
        baseStats.attack += 1.0;
        baseStats.intelligence += 0.0;
        baseStats.dexterity += 0.0;
        break;
    case 2: // Mage - Magic & Control
        baseStats.health += 3.0;
        baseStats.attack += 0.0;
        baseStats.intelligence += 3.0;
        baseStats.dexterity += 0.0;
        break;
    default:
        break;
    }
}

void CharacterSheet::applyStatConversions() {
    // Class-specific stat conversions
    switch (classIndex) {
    case 0: // Assassin:  Every 100 Dexterity adds +1 Attack
    {
        double dexBonus = floor(baseStats.dexterity / 100.0);
        baseStats.attack += dexBonus;
    }
    break;
    case 1: // Bruiser:  No conversion (pure tank)
        break;
    case 2: // Mage: Every 100 Intelligence adds +1 Attack
    {
        double intBonus = floor(baseStats.intelligence / 100.0);
        baseStats.attack += intBonus;
    }
    break;
    default:
        break;
    }
}

void CharacterSheet::calculateDodgeChance() {
    // Only Assassins have dodge
    if (classIndex != 0) {
        dodgeChance = 0.0;
        return;
    }

    // Formula: Dexterity / (Dexterity + 150 + Level × 3)
    double denominator = baseStats.dexterity + 150.0 + (level * 3.0);
    dodgeChance = (baseStats.dexterity / denominator) * 100.0;

    // Base cap at 50%, but Goblins get +5% (55% cap)
    double dodgeCap = 50.0 + getDodgeCapBonus();
    if (dodgeChance > dodgeCap) {
        dodgeChance = dodgeCap;
    }
}

void CharacterSheet::calculateCritChance() {
    // Base crit chance:  10%
    // +1% crit per 50 luck
    // Capped at 50%
    critChance = 10 + static_cast<int>(baseStats.luck / 50.0);

    if (critChance > 50) {
        critChance = 50;
    }
}

RaceStats CharacterSheet::getEntenteRaceStats(int race) {
    switch (race) {
    case 0: // Human - Balanced
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    case 1: // Dwarf - High defense and strength
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    case 2: // Golem - High defense
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    default:
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    }
}

RaceStats CharacterSheet::getThrongRaceStats(int race) {
    switch (race) {
    case 0: // Goblin - Fast
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    case 1: // Elf - Magical
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    case 2: // Orc - Strong
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    default:
        return { 100.0, 10.0, 10.0, 10.0, 0.0, 0.0 };
    }
}

void CharacterSheet::setupUI() {
    // Fixed size 1600x1080 box, aligned to RIGHT side, positioned at top (full screen height)
    float sheetWidth = 1600.f;
    float sheetHeight = 1080.f;
    sheetBackground.setSize({ sheetWidth, sheetHeight });
    sheetBackground.setFillColor(Color(20, 20, 30, 255));
    sheetBackground.setOutlineThickness(5.f);
    sheetBackground.setOutlineColor(Color(200, 150, 50));
    sheetBackground.setPosition({ windowWidth - sheetWidth, 0.f });

    float boxX = sheetBackground.getPosition().x;
    float boxY = sheetBackground.getPosition().y;

    // Title
    titleText.setString("Character");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color(255, 215, 0));
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({ titleBounds.size.x / 2.f, 0.f });
    titleText.setPosition({ boxX + sheetWidth / 2.f, boxY + 30.f });

    // Level text (next to title)
    levelText.setString("Level " + to_string(level));
    levelText.setCharacterSize(40);
    levelText.setFillColor(Color(100, 200, 255));
    levelText.setPosition({ boxX + 50.f, boxY + 35.f });

    // XP Bar (below title)
    float xpBarWidth = 600.f;
    float xpBarHeight = 30.f;
    float xpBarX = boxX + (sheetWidth - xpBarWidth) / 2.f;
    float xpBarY = boxY + 100.f;

    xpBarBackground.setSize({ xpBarWidth, xpBarHeight });
    xpBarBackground.setFillColor(Color(50, 50, 60));
    xpBarBackground.setOutlineThickness(2.f);
    xpBarBackground.setOutlineColor(Color::White);
    xpBarBackground.setPosition({ xpBarX, xpBarY });

    // XP Bar fill (starts at 0%)
    float xpPercent = static_cast<float>(currentXP) / static_cast<float>(xpToNextLevel);
    xpBarFill.setSize({ xpBarWidth * xpPercent, xpBarHeight });
    xpBarFill.setFillColor(Color(50, 200, 50));
    xpBarFill.setPosition({ xpBarX, xpBarY });

    // XP Text (centered on bar)
    char xpBuffer[50];
    snprintf(xpBuffer, sizeof(xpBuffer), "%d / %d XP", (int)currentXP, (int)xpToNextLevel);
    xpText.setString(xpBuffer);
    xpText.setCharacterSize(20);
    xpText.setFillColor(Color::White);
    FloatRect xpTextBounds = xpText.getLocalBounds();
    xpText.setOrigin({ xpTextBounds.size.x / 2.f, xpTextBounds.size.y / 2.f });
    xpText.setPosition({ xpBarX + xpBarWidth / 2.f, xpBarY + xpBarHeight / 2.f });

    float centerX = boxX + sheetWidth / 2.f;
    float topY = boxY + 180.f;  // Moved down to make room for XP bar
    float slotSize = 100.f;
    float spacing = 30.f;

    // ARMOR SLOTS (Left center)
    string armorNames[] = { "Helmet", "Chestplate", "Pants", "Boots" };
    float armorX = centerX - 400.f;

    for (int i = 0; i < 4; ++i) {
        RectangleShape slot;
        slot.setSize({ slotSize, slotSize });
        slot.setFillColor(Color(50, 50, 60));
        slot.setOutlineThickness(3.f);
        slot.setOutlineColor(Color(150, 150, 200));
        slot.setPosition({ armorX, topY + (slotSize + spacing) * i });
        armorSlots.push_back(slot);

        Text label(font);
        label.setString(armorNames[i]);
        label.setCharacterSize(24);
        label.setFillColor(Color::White);
        label.setPosition({ armorX + slotSize + 20.f,
            topY + (slotSize + spacing) * i + slotSize / 2.f - 12.f });
        armorSlotLabels.push_back(label);
    }

    // OTHER SLOTS (Right center)
    string otherNames[] = { "Weapon", "Talisman", "Companion", "Curse" };
    float otherX = centerX + 200.f;

    for (int i = 0; i < 4; ++i) {
        RectangleShape slot;
        slot.setSize({ slotSize, slotSize });
        slot.setFillColor(Color(50, 50, 60));
        slot.setOutlineThickness(3.f);
        slot.setOutlineColor(Color(180, 100, 255));
        slot.setPosition({ otherX, topY + (slotSize + spacing) * i });
        otherSlots.push_back(slot);

        Text label(font);
        label.setString(otherNames[i]);
        label.setCharacterSize(24);
        label.setFillColor(Color::White);
        label.setPosition({ otherX - 150.f,
            topY + (slotSize + spacing) * i + slotSize / 2.f - 12.f });
        otherSlotLabels.push_back(label);
    }

    // STATS (Center, below equipment)
    float statsY = topY + 550.f;
    float statsStartX = centerX - 300.f;

    string statNames[] = { "Health", "Attack", "Intelligence", "Dexterity", "Armor", "Luck" };
    double statValues[] = { baseStats.health, baseStats.attack, baseStats.intelligence,
                        baseStats.dexterity, baseStats.armor, baseStats.luck };

    for (int i = 0; i < 6; ++i) {
        Text statText(font);
        // Format double with 2 decimal places
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%s: %.2f", statNames[i].c_str(), statValues[i]);
        statText.setString(buffer);
        statText.setCharacterSize(26);
        statText.setFillColor(Color(200, 255, 200));

        if (i < 3) {
            statText.setPosition({ statsStartX, statsY + i * 35.f });
        }
        else {
            statText.setPosition({ statsStartX + 400.f, statsY + (i - 3) * 35.f });
        }

        statsTexts.push_back(statText);
    }

    // Dodge chance display (Assassin only)
    if (classIndex == 0) {  // Assassin
        Text dodgeText(font);
        char dodgeBuffer[50];
        snprintf(dodgeBuffer, sizeof(dodgeBuffer), "Dodge: %.2f%%", dodgeChance);
        dodgeText.setString(dodgeBuffer);
        dodgeText.setCharacterSize(26);
        dodgeText.setFillColor(Color(100, 255, 100)); // Bright green
        dodgeText.setPosition({ statsStartX, statsY + 120.f });
        statsTexts.push_back(dodgeText);
    }

    // Mount display
    mountText.setString("");
    mountText.setCharacterSize(26);
    mountText.setFillColor(Color(255, 215, 0));
    mountText.setPosition({ statsStartX, statsY + 155.f });

    // INVENTORY SECTION (Bottom)
    float inventoryY = boxY + sheetHeight - 220.f;

    // Inventory title
    Text inventoryTitle(font);
    inventoryTitle.setString("Inventory");
    inventoryTitle.setCharacterSize(35);
    inventoryTitle.setFillColor(Color(255, 215, 0));
    FloatRect invTitleBounds = inventoryTitle.getLocalBounds();
    inventoryTitle.setOrigin({ invTitleBounds.size.x / 2.f, 0.f });
    inventoryTitle.setPosition({ centerX, inventoryY - 50.f });
    statsTexts.push_back(inventoryTitle);

    // Inventory background bar (use separate member variable)
    inventoryBackground.setSize({ sheetWidth - 100.f, 180.f });
    inventoryBackground.setFillColor(Color(40, 40, 50, 200));
    inventoryBackground.setOutlineThickness(3.f);
    inventoryBackground.setOutlineColor(Color(100, 100, 120));
    inventoryBackground.setPosition({ boxX + 50.f, inventoryY });

    // Inventory slots (10 slots in a row) - use separate vector
    float invSlotSize = 80.f;
    float invSpacing = 20.f;
    float invStartX = centerX - (invSlotSize * 10 + invSpacing * 9) / 2.f;

    inventorySlots.clear();
    for (int i = 0; i < 10; ++i) {
        RectangleShape invSlot;
        invSlot.setSize({ invSlotSize, invSlotSize });
        invSlot.setFillColor(Color(60, 60, 70));
        invSlot.setOutlineThickness(2.f);
        invSlot.setOutlineColor(Color(120, 120, 140));
        invSlot.setPosition({ invStartX + (invSlotSize + invSpacing) * i, inventoryY + 50.f });
        inventorySlots.push_back(invSlot);
    }

    // Initialize stats display
    updateStatsDisplay();
}

void CharacterSheet::setupInventoryDisplay() {
    // Initialize inventory display - will be populated when items are added
    inventoryDisplayItems.clear();
}

void CharacterSheet::updateInventoryDisplay() {
    if (!inventory) return;

    inventoryDisplayItems.clear();

    // Get all items from inventory
    vector<string> allItems = inventory->getAllItems();

    float boxX = sheetBackground.getPosition().x;
    float boxY = sheetBackground.getPosition().y;
    float sheetWidth = 1600.f;
    float centerX = boxX + sheetWidth / 2.f;
    float inventoryY = boxY + 1080.f - 220.f;

    float invSlotSize = 80.f;
    float invSpacing = 20.f;
    float invStartX = centerX - (invSlotSize * 10 + invSpacing * 9) / 2.f;

    int itemIndex = 0;
    for (const auto& itemName : allItems) {
        if (itemIndex >= 10) break;  // Max 10 items visible

        InventoryItemDisplay display(font);  // PASS FONT HERE
        display.itemName = itemName;
        display.quantity = inventory->getItemCount(itemName);

        // Create visual slot
        display.slot.setSize({ invSlotSize, invSlotSize });
        display.slot.setFillColor(Color(60, 100, 150));
        display.slot.setOutlineThickness(2.f);
        display.slot.setOutlineColor(Color(100, 200, 255));
        display.slot.setPosition({ invStartX + (invSlotSize + invSpacing) * itemIndex, inventoryY + 50.f });

        // Item name
        display.nameText.setString(itemName);
        display.nameText.setCharacterSize(12);
        display.nameText.setFillColor(Color::White);
        display.nameText.setPosition({ display.slot.getPosition().x + 5.f, display.slot.getPosition().y + 5.f });

        // Quantity
        display.quantityText.setString("x" + to_string(display.quantity));
        display.quantityText.setCharacterSize(14);
        display.quantityText.setFillColor(Color(0, 255, 0));
        display.quantityText.setPosition({ display.slot.getPosition().x + 5.f, display.slot.getPosition().y + 50.f });

        inventoryDisplayItems.push_back(display);
        itemIndex++;
    }
}

void CharacterSheet::addInventoryItem(const string& itemName) {
    if (inventory && inventory->addItem(itemName)) {
        updateInventoryDisplay();
    }
}

void CharacterSheet::show() {
    visible = true;
    updateInventoryDisplay();  // Refresh inventory display when opened
    updateEquipmentVisuals();  // Refresh equipment colors and names
    updateStatsDisplay();  // Refresh stats display with equipment bonuses
}

void CharacterSheet::hide() {
    visible = false;
    selectedInventoryIndex = -1;
    selectedInventoryItem = "";
    showingSellConfirmation = false;
}

void CharacterSheet::draw(RenderWindow& window) {
    if (!visible) return;

    window.draw(sheetBackground);
    window.draw(titleText);
    window.draw(levelText);
    window.draw(xpBarBackground);
    window.draw(xpBarFill);
    window.draw(xpText);

    // Draw armor slots
    for (const auto& slot : armorSlots) {
        window.draw(slot);
    }
    for (const auto& label : armorSlotLabels) {
        window.draw(label);
    }
    for (const auto& name : armorEquippedNames) {
        window.draw(name);
    }

    // Draw other slots
    for (const auto& slot : otherSlots) {
        window.draw(slot);
    }
    for (const auto& label : otherSlotLabels) {
        window.draw(label);
    }
    for (const auto& name : otherEquippedNames) {
        window.draw(name);
    }

    // Draw stats
    for (const auto& stat : statsTexts) {
        window.draw(stat);
    }

    // Update and draw mount display
    updateMountDisplay();
    window.draw(mountText);

    // Draw inventory section background and slots
    window.draw(inventoryBackground);
    for (const auto& slot : inventorySlots) {
        window.draw(slot);
    }

    // Draw inventory items with selection highlight
    for (int i = 0; i < inventoryDisplayItems.size(); i++) {
        auto& item = inventoryDisplayItems[i];

        // Highlight selected item
        if (i == selectedInventoryIndex) {
            item.slot.setFillColor(Color(70, 120, 70));
            item.slot.setOutlineColor(Color(100, 255, 100));
            item.slot.setOutlineThickness(3.f);
        }
        else {
            item.slot.setFillColor(Color(60, 100, 150));
            item.slot.setOutlineColor(Color(100, 200, 255));
            item.slot.setOutlineThickness(2.f);
        }

        window.draw(item.slot);
        window.draw(item.nameText);
        window.draw(item.quantityText);
    }

    // Draw equip/sell buttons if inventory item is selected
    if (selectedInventoryIndex >= 0 && selectedInventoryIndex < inventoryDisplayItems.size()) {
        window.draw(equipButton);
        window.draw(equipButtonText);
        window.draw(sellButton);
        window.draw(sellButtonText);
    }

    // Draw tooltip if hovering over equipped item
    if (hoveredSlotIndex >= 0) {
        string itemName;
        FloatRect slotBounds;

        if (hoveredSlotIndex < 100) {
            // Armor slot
            if (hoveredSlotIndex >= armorSlots.size()) return;

            EquipmentSlot slot;
            if (hoveredSlotIndex == 0) slot = EquipmentSlot::Helmet;
            else if (hoveredSlotIndex == 1) slot = EquipmentSlot::Chestplate;
            else if (hoveredSlotIndex == 2) slot = EquipmentSlot::Pants;
            else if (hoveredSlotIndex == 3) slot = EquipmentSlot::Boots;
            else return;

            itemName = getEquippedItem(slot);
            slotBounds = armorSlots[hoveredSlotIndex].getGlobalBounds();
        }
        else {
            // Other slot
            int index = hoveredSlotIndex - 100;
            if (index >= otherSlots.size()) return;

            EquipmentSlot slot;
            if (index == 0) slot = EquipmentSlot::Weapon;
            else if (index == 1) slot = EquipmentSlot::Talisman;
            else if (index == 2) slot = EquipmentSlot::Companion;
            else return;

            itemName = getEquippedItem(slot);
            slotBounds = otherSlots[index].getGlobalBounds();
        }

        if (!itemName.empty()) {
            string description = getItemDescription(itemName);
            tooltipText.setString(description);
            FloatRect textBounds = tooltipText.getLocalBounds();

            float padding = 15.0f;
            float tooltipWidth = textBounds.size.x + (padding * 2);
            float tooltipHeight = textBounds.size.y + (padding * 2);

            // Position tooltip to the right of the slot
            float tooltipX = slotBounds.position.x + slotBounds.size.x + 10.0f;
            float tooltipY = slotBounds.position.y;

            // Keep tooltip on screen
            if (tooltipX + tooltipWidth > windowWidth) {
                tooltipX = slotBounds.position.x - tooltipWidth - 10.0f;
            }
            if (tooltipY + tooltipHeight > windowHeight) {
                tooltipY = windowHeight - tooltipHeight;
            }
            if (tooltipX < 0) tooltipX = 10.0f;
            if (tooltipY < 0) tooltipY = 10.0f;

            tooltipBox.setSize({ tooltipWidth, tooltipHeight });
            tooltipBox.setPosition({ tooltipX, tooltipY });
            tooltipText.setPosition({ tooltipX + padding, tooltipY + padding });

            window.draw(tooltipBox);
            window.draw(tooltipText);
        }
    }

    // Draw tooltip if hovering over inventory item
    if (hoveredInventoryIndex >= 0 && hoveredInventoryIndex < inventoryDisplayItems.size()) {
        string itemName = inventoryDisplayItems[hoveredInventoryIndex].itemName;
        FloatRect slotBounds = inventoryDisplayItems[hoveredInventoryIndex].slot.getGlobalBounds();

        if (!itemName.empty()) {
            string description = getItemDescription(itemName);
            tooltipText.setString(description);
            FloatRect textBounds = tooltipText.getLocalBounds();

            float padding = 15.0f;
            float tooltipWidth = textBounds.size.x + (padding * 2);
            float tooltipHeight = textBounds.size.y + (padding * 2);

            // Position tooltip above the slot
            float tooltipX = slotBounds.position.x;
            float tooltipY = slotBounds.position.y - tooltipHeight - 10.0f;

            // Keep tooltip on screen
            if (tooltipX + tooltipWidth > windowWidth) {
                tooltipX = windowWidth - tooltipWidth - 10.0f;
            }
            if (tooltipY < 0) {
                tooltipY = slotBounds.position.y + slotBounds.size.y + 10.0f;
            }
            if (tooltipX < 0) tooltipX = 10.0f;

            tooltipBox.setSize({ tooltipWidth, tooltipHeight });
            tooltipBox.setPosition({ tooltipX, tooltipY });
            tooltipText.setPosition({ tooltipX + padding, tooltipY + padding });

            window.draw(tooltipBox);
            window.draw(tooltipText);
        }
    }

    // Draw unequip button if an item is selected
    if (selectedEquipmentSlot != -1) {
        window.draw(unequipButton);
        window.draw(unequipButtonText);
    }

    // Draw sell confirmation popup if showing
    if (showingSellConfirmation) {
        window.draw(confirmPopupBackground);
        window.draw(confirmPopupBox);
        window.draw(confirmPopupText);
        window.draw(confirmSureButton);
        window.draw(confirmSureText);
        window.draw(confirmNoButton);
        window.draw(confirmNoText);
    }
}

bool CharacterSheet::isCloseClicked(const RenderWindow& window) {
    // Close button has been removed - always return false
    return false;
}

bool CharacterSheet::isClickedOutside(const RenderWindow& window) {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return !sheetBackground.getGlobalBounds().contains(mousePosF);
}


void CharacterSheet::equipItem(EquipmentSlot slot, const string& itemName, Inventory* inventory) {
    // Check if there's already an item in this slot
    string oldItem = getEquippedItem(slot);
    if (!oldItem.empty() && oldItem != "Empty" && inventory) {
        // Return old item to inventory
        inventory->addItem(oldItem);
        cout << "Returned " << oldItem << " to inventory" << endl;
    }

    // Equip new item
    equipment[slot] = itemName;
    loadRaceStats();
    cout << "Equipped " << itemName << " - stats recalculated" << endl;
    updateEquipmentVisuals();
    updateStatsDisplay();
}

string CharacterSheet::getEquippedItem(EquipmentSlot slot) const {
    auto it = equipment.find(slot);
    if (it != equipment.end()) {
        return it->second;
    }
    return "Empty";
}

double CharacterSheet::getStat(const string& statName) const {
    if (statName == "Health") {
        double health = baseStats.health;
        if (hasCurse) {
            health *= (1.0 + currentCurse.modifiers.healthModifier);
        }
        health += getEquipmentStat("Health");
        return health;
    }
    if (statName == "Attack") {
        double attack = baseStats.attack;
        if (hasCurse) {
            attack *= (1.0 + currentCurse.modifiers.attackModifier);
        }
        attack += getEquipmentStat("Attack");
        return attack;
    }
    if (statName == "Intelligence") {
        double intelligence = baseStats.intelligence;
        // Elf Arcane Affinity: +10% intelligence scaling
        if (faction == Faction::Throng && raceIndex == 1) { // Elf
            intelligence *= 1.10; // 10% bonus
        }
        if (hasCurse) {
            intelligence *= (1.0 + currentCurse.modifiers.intelligenceModifier);
        }
        intelligence += getEquipmentStat("Intelligence");
        return intelligence;
    }
    if (statName == "Dexterity") {
        double dexterity = baseStats.dexterity;
        if (hasCurse) {
            dexterity *= (1.0 + currentCurse.modifiers.dexterityModifier);
        }
        dexterity += getEquipmentStat("Dexterity");
        return dexterity;
    }
    if (statName == "Armor") {
        double armor = baseStats.armor;
        // Golem Stone Skin: +10% armor (Bruiser) or +5% armor (other classes)
        if (faction == Faction::Entente && raceIndex == 2) { // Golem
            double bonus = (classIndex == 1) ? 0.10 : 0.05; // 10% for Bruiser, 5% for others
            armor *= (1.0 + bonus);
        }
        if (hasCurse) {
            armor *= (1.0 + currentCurse.modifiers.armorModifier);
        }
        armor += getEquipmentStat("Armor");
        return armor;
    }
    if (statName == "Luck") {
        double luck = baseStats.luck;
        if (hasCurse) {
            luck *= (1.0 + currentCurse.modifiers.luckModifier);
        }
        luck += getEquipmentStat("Luck");
        return luck;
    }
    if (statName == "CritChance") return static_cast<double>(critChance);
    if (statName == "CritDamage") return static_cast<double>(critDamage);
    if (statName == "DodgeChance") return dodgeChance;
    return 0.0;
}

double CharacterSheet::getEquipmentStat(const string& statName) const {
    double total = 0.0;

    // Get all equipped items and sum their stats
    for (const auto& pair : equipment) {
        const string& itemName = pair.second;
        if (itemName.empty() || itemName == "Empty") continue;

        ItemData data = ItemDatabase::getInstance().getItemData(itemName);

        if (statName == "Attack") total += data.attack;
        else if (statName == "Health") total += data.health;
        else if (statName == "Armor") total += data.armor;
        else if (statName == "Dexterity") total += data.dexterity;
        else if (statName == "Intelligence") total += data.intelligence;
        else if (statName == "Luck") total += data.luck;
    }

    return total;
}

void CharacterSheet::addXP(double amount) {
    currentXP += amount;

    // Check for level up
    while (currentXP >= xpToNextLevel && level < 100) {
        levelUp();
    }

    updateXPBar();
}

void CharacterSheet::levelUp() {
    level++;
    currentXP -= xpToNextLevel;
    xpToNextLevel = calculateXPForLevel(level);

    // Percentage-based stat increases based on level range
    float statIncreasePercent;
    if (level >= 1 && level <= 20) {
        statIncreasePercent = 0.02f; // 2%
    }
    else if (level >= 21 && level <= 50) {
        statIncreasePercent = 0.012f; // 1.2%
    }
    else if (level >= 51 && level <= 80) {
        statIncreasePercent = 0.007f; // 0.7%
    }
    else { // 81-100
        statIncreasePercent = 0.004f; // 0.4%
    }

    // Apply percentage increase to all stats
    baseStats.health += baseStats.health * statIncreasePercent;
    baseStats.attack += baseStats.attack * statIncreasePercent;
    baseStats.intelligence += baseStats.intelligence * statIncreasePercent;
    baseStats.dexterity += baseStats.dexterity * statIncreasePercent;
    baseStats.armor += baseStats.armor * statIncreasePercent;
    baseStats.luck += baseStats.luck * statIncreasePercent;

    // CLASS-SPECIFIC BONUSES PER LEVEL
    switch (classIndex) {
    case 0: // Assassin:  +4 Dexterity per level
        baseStats.dexterity += 4.0;
        cout << "  Assassin Bonus: +4 Dexterity!" << endl;
        break;
    case 1: // Bruiser: +50% bonus to health gained this level
    {
        double healthGained = baseStats.health * statIncreasePercent;
        double bruiserBonus = healthGained * 0.5;
        baseStats.health += bruiserBonus;
        cout << "  Bruiser Bonus:  +" << bruiserBonus << " extra Health (50% bonus)!" << endl;
    }
    break;
    case 2: // Mage:  +4 Intelligence per level
        baseStats.intelligence += 4.0;
        cout << "  Mage Bonus: +4 Intelligence!" << endl;
        break;
    default:
        break;
    }

    // Apply stat conversions after leveling up
    applyStatConversions();

    // Recalculate dodge chance (Assassin only)
    calculateDodgeChance();

    // Recalculate crit chance based on luck
    calculateCritChance();

    cout << "LEVEL UP!  Now level " << level << " (+" << (statIncreasePercent * 100) << "% to all stats)" << endl;

    // Update UI
    levelText.setString("Level " + to_string(level));

    // Update stat displays
    updateStatsDisplay();


    // Trigger curse selection
    if (level == 1 && !hasFirstCurse) {
        // First curse is automatic at level 1
        needsCurseSelection = true;
    }
    else {
        // Every other level triggers curse selection
        needsCurseSelection = true;
    }

    // Class unlock at level 25
    if (level == 25 && !unlockedClass) {
        unlockedClass = true;
        cout << "Class unlocked at level 25!" << endl;
    }
}

void CharacterSheet::updateXPBar() {
    float xpPercent = static_cast<float>(currentXP) / static_cast<float>(xpToNextLevel);
    float xpBarWidth = 600.f;
    xpBarFill.setSize({ xpBarWidth * xpPercent, 30.f });

    char xpBuffer[50];
    snprintf(xpBuffer, sizeof(xpBuffer), "%.0f / %.0f XP", currentXP, xpToNextLevel);
    xpText.setString(xpBuffer);
    FloatRect xpTextBounds = xpText.getLocalBounds();
    xpText.setOrigin({ xpTextBounds.size.x / 2.f, xpTextBounds.size.y / 2.f });
}

double CharacterSheet::calculateXPForLevel(int lvl) {
    // XP formula: 100 × Level^1.6 (exponential scaling)
    return round(100.0 * pow(static_cast<double>(lvl), 1.6));
}

// RACIAL PASSIVES IMPLEMENTATION

double CharacterSheet::getSilverBonus() const {
    // Human (Entente race 0): +5% silver gain
    if (faction == Faction::Entente && raceIndex == 0) {
        return 0.05; // 5% bonus
    }
    return 0.0;
}

double CharacterSheet::getDodgeCapBonus() const {
    // Goblin (Throng race 0): +5% to dodge cap
    if (faction == Faction::Throng && raceIndex == 0) {
        return 5.0; // +5% to cap (50% → 55%)
    }
    return 0.0;
}

double CharacterSheet::getArmorBonus() const {
    // Dwarf (Entente race 1): +10% armor for Bruiser, +5% for others
    if (faction == Faction::Entente && raceIndex == 1) {
        if (classIndex == 1) { // Bruiser
            return baseStats.armor * 0.10; // +10% for Bruiser
        }
        else {
            return baseStats.armor * 0.05; // +5% for other classes
        }
    }
    return 0.0;
}

double CharacterSheet::getIntelligenceScaling() const {
    // Elf (Throng race 1): +10% intelligence scaling
    if (faction == Faction::Throng && raceIndex == 1) {
        return 0.10; // 10% bonus to intelligence effects
    }
    return 0.0;
}

double CharacterSheet::getArmorEffectiveness() const {
    // Golem (Entente race 2): Armor effectiveness with class scaling
    if (faction == Faction::Entente && raceIndex == 2) {
        if (classIndex == 1) { // Bruiser
            return 0.20; // +20% effectiveness
        }
        else {
            return 0.10; // +10% effectiveness for other classes
        }
    }
    return 0.0;
}

double CharacterSheet::getHealthRegenPerTurn() const {
    // Orc (Throng race 2): Health regen per turn based on level
    if (faction == Faction::Throng && raceIndex == 2) {
        return level * 0.1; // 0.1 HP per level per turn
    }
    return 0.0;
}
void CharacterSheet::updateMountDisplay() {
    if (currentMount && currentMount->type != MountType::None && currentMount->remainingDays > 0) {
        string mountName = (currentMount->type == MountType::Horse) ? "Horse" : "Griffin";
        int speedBonus = static_cast<int>(currentMount->speedBonus * 100);
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Mount: %s (+%d%% speed, %d days)",
            mountName.c_str(), speedBonus, currentMount->remainingDays);
        mountText.setString(buffer);
    }
    else {
        mountText.setString("Mount: None");
    }
}

void CharacterSheet::applyCurse(const CurseData& curse) {
    if (hasCurse && !hasFirstCurse) {
        removeCurse();
    }

    currentCurse = curse;

    if (curse.isFirstCurse) {
        hasFirstCurse = true;
    }

    hasCurse = true;

    loadRaceStats();

    cout << "Applied curse: " << curse.curseName << endl;
}

void CharacterSheet::removeCurse() {
    if (!hasCurse || hasFirstCurse) {
        return;
    }

    currentCurse = CurseData();
    hasCurse = false;

    loadRaceStats();

    cout << "Removed curse" << endl;
}

string CharacterSheet::getItemDescription(const string& itemName) const {
    // ============================================
    // ASSASSIN WEAPONS
    // ============================================
    if (itemName == "Broken Sword") return "+10 Attack";
    if (itemName == "Short Blade") return "+10 Attack, +80 Health\nHeal 2.5 HP after every attack";
    if (itemName == "Dagger") return "+15 Attack, +15 Dexterity\nEach hit shreds 3 armor (max 15)";
    if (itemName == "The Farmer's Scythe") return "+17 Attack\n+3 Health on hit, +10 Silver on dungeon finish";
    if (itemName == "Long Blade") return "+30 Attack, +150 Health";
    if (itemName == "Basic Sword") return "+40 Attack";
    if (itemName == "Legion Sword") return "+60 Attack";
    if (itemName == "Stun") return "Stun enemy and attack again";

    // ============================================
    // ASSASSIN HELMETS
    // ============================================
    if (itemName == "Silent Hood") return "+10 Armor\nFirst crit: +10 bonus Attack";
    if (itemName == "Nightblade Mask") return "+10 Armor\nDodge second enemy attack";
    if (itemName == "Rogue's Cap") return "+12 Armor, +5 Attack\nHeal 2 HP after crit";
    if (itemName == "Shadow Hood") return "+15 Armor, +5 Dexterity\n+5 Dex for 1 turn after dodge";
    if (itemName == "Assassin's Helm") return "+15 Armor\n+5 Dex while below 50% HP";

    // ============================================
    // ASSASSIN CHESTPLATES
    // ============================================
    if (itemName == "Vest") return "+25 Armor";
    if (itemName == "Silent Armor") return "+15 Armor\n+5 Dex when attacked";
    if (itemName == "Thief's Cloak") return "+20 Armor\nDodge first boss/elite attack";
    if (itemName == "Rogue's Garb") return "+20 Armor, +5 Dexterity\n<50% HP: +15 Attack";
    if (itemName == "Nightstalker Vest") return "+25 Armor\n+5 Attack per crit in combat";

    // ============================================
    // ASSASSIN LEGGINGS
    // ============================================
    if (itemName == "Poisoned Trousers") return "+10 Armor\nPoison: 3 dmg/turn for 2 turns";
    if (itemName == "Nightblade Pants") return "+12 Armor, +5 Attack\n+5 Attack if below 50% HP";
    if (itemName == "Shadow Leggings") return "+15 Armor\n+5 Dex after each attack";
    if (itemName == "Rogue's Greaves") return "+15 Armor\nAfter crit: +5 Dex for next attack";
    if (itemName == "Silent Steps") return "+10 Armor, +10 Dexterity\nDodge second enemy attack";

    // ============================================
    // ASSASSIN BOOTS
    // ============================================
    if (itemName == "Leather Boots") return "+20 Armor";
    if (itemName == "Long Boots") return "+25 Armor";
    if (itemName == "Dynamites Boots") return "+30 Attack\n+10 Armor Penetration";
    if (itemName == "Boots of Healing") return "+20 Attack\n+4 Health per attack";
    if (itemName == "Shadowstep Boots") return "+25 Attack, +10 Dexterity\nBelow 50% HP: +15 Attack";

    // ============================================
    // ASSASSIN TALISMANS
    // ============================================
    if (itemName == "I DONT KNOW") return "+150 Health";
    if (itemName == "Bloodstone") return "+10 Attack while below 30% HP";
    if (itemName == "Metal") return "Gain 20 Shield at start of combat";
    if (itemName == "Flask") return "Second enemy attack deals 0 damage";
    if (itemName == "Vial of Venom") return "Poison: 5 dmg/turn for 3 turns";

    // ============================================
    // ASSASSIN COMPANIONS
    // ============================================
    if (itemName == "Wolf") return "Dungeon win: +80 XP\nHeal 3 HP per attack";
    if (itemName == "Merchant Rat") return "Dungeon wins grant +25 Silver";
    if (itemName == "Rogue Cat") return "First attack guaranteed crit";
    if (itemName == "Shadow Rat") return "+10 Dex per dodge in combat";
    if (itemName == "Crows of Misfortune") return "First enemy attack: 20% miss chance";

    // ============================================
    // MAGE WEAPONS
    // ============================================
    if (itemName == "Dark Ring") return "+15 Attack, +50 Health\nArena: +12 Attack";
    if (itemName == "Lost Ring") return "+18 Attack, +90 Health\nNormal attacks +5 damage";
    if (itemName == "Book of Secrets") return "+20 Attack\n+5 damage to burning enemies";
    if (itemName == "Stick") return "+45 Attack";
    if (itemName == "Rod") return "+50 Attack";
    if (itemName == "Book of Healing") return "+5 Attack\n+10 Health on hit";

    // ============================================
    // MAGE HELMETS
    // ============================================
    if (itemName == "Wizard's Hat") return "+10 Armor, +10 Intelligence\nFirst spell +5 damage";
    if (itemName == "Arcane Hood") return "+12 Intelligence\n+5 Intelligence after hit";
    if (itemName == "Mage's Circlet") return "+8 Armor, +8 Intelligence\nHeal 5 HP on spell crit";
    if (itemName == "Scholar's Cap") return "+10 Intelligence\n+5 Attack after spell crit";
    if (itemName == "Enchanted Hood") return "+10 Armor, +5 Intelligence\nSecond enemy attack deals 0 damage";

    // ============================================
    // MAGE CHESTPLATES
    // ============================================
    if (itemName == "Apprentice Robe") return "+20 Armor";
    if (itemName == "Arcane Vestments") return "+25 Armor\n+5 Attack per spell";
    if (itemName == "Pyromancer's Cloak") return "+15 Armor\nBurn: 3 dmg/turn for 2 turns";
    if (itemName == "Mage's Mantle") return "+18 Armor, +8 Intelligence\nHeal 3 HP when hit";
    if (itemName == "Robe of Shielding") return "+20 Armor\nGain 15 Shield when below 50% HP";

    // ============================================
    // MAGE LEGGINGS
    // ============================================
    if (itemName == "Arcane Pants") return "+10 Armor, +12 Intelligence\nGain 20 Shield at combat start";
    if (itemName == "Mystic Legwraps") return "+8 Armor, +15 Intelligence\n+3 Attack per spell";
    if (itemName == "Scholar's Trousers") return "+12 Armor\nHeal 2 HP per spell";
    if (itemName == "Pyromancer's Leggings") return "+10 Armor\nBurn: 2 dmg/turn for 2 turns";
    if (itemName == "Mana Flow Pants") return "+8 Armor, +10 Intelligence\n+5 Intelligence below 50% HP";

    // ============================================
    // MAGE BOOTS
    // ============================================
    if (itemName == "Magical Boots") return "+12 Intelligence";
    if (itemName == "Enchanted Boots") return "+18 Intelligence";
    if (itemName == "Boots of Witch") return "+40 Attack\n+8 Armor Penetration";
    if (itemName == "Arcane Slippers") return "+25 Attack, +12 Intelligence\nFirst spell +10 damage";
    if (itemName == "Boots of the Pyromancer") return "+30 Attack\n+5 Armor Pen, Deal 5 damage at start";

    // ============================================
    // MAGE TALISMANS
    // ============================================
    if (itemName == "Orb of the Fortune Teller") return "+30 Attack, +80 Health";
    if (itemName == "Ancient Coin") return "+80 Health\nGain 30 Silver every 3rd dungeon";
    if (itemName == "Some Other Crystal") return "+15 Attack, +15 Intelligence";
    if (itemName == "Big CRYSTAL") return "+30 Attack, +10 Armor\nGain 20 Shield at start";
    if (itemName == "Meteor") return "Deal 25 damage at combat start";
    if (itemName == "STAR") return "Deal 50 damage at start (Arena only)";
    if (itemName == "GOLDEN ARMOR") return "First enemy attack deals 0 damage";

    // ============================================
    // MAGE COMPANIONS
    // ============================================
    if (itemName == "Fox") return "Dungeon win: +80 XP\nHeal 3 HP per attack";
    if (itemName == "Fire Imp") return "+5 spell damage per spell (max +25)";
    if (itemName == "Mana Sprite") return "+10 Intelligence in combat";
    if (itemName == "Crystal Owl") return "Restore 5 HP per turn";
    if (itemName == "Arcane Familiar") return "5% chance any enemy attack deals 0";

    // ============================================
    // BRUISER WEAPONS
    // ============================================
    if (itemName == "Battle Shield") return "+110 Health\nHeal 4 HP after attack";
    if (itemName == "Pickaxe") return "+15 Attack\n+5 Attack per hit (max +25)";
    if (itemName == "Mallet") return "+8 Attack\n+8 Health on hit";
    if (itemName == "Tomahawk") return "+25 Attack, +130 Health";
    if (itemName == "Hammer") return "+35 Attack";
    if (itemName == "Hammer of George") return "+25 Attack, +150 Health";
    if (itemName == "Pointy Stick") return "+40 Attack, +20 Armor";
    if (itemName == "Enchanted Shield") return "+35 Shield at combat start";

    // ============================================
    // BRUISER HELMETS
    // ============================================
    if (itemName == "Iron Helm") return "+30 Armor\nReduce first enemy attack by 5";
    if (itemName == "Battle Helm") return "+25 Armor, +10 Health\nHeal 3 HP when hit";
    if (itemName == "Warrior's Cap") return "+20 Armor\nGain 10 Shield at combat start";
    if (itemName == "Guardian Helm") return "+25 Armor\nFirst enemy attack deals 0 damage";
    if (itemName == "Heavy Plate Helm") return "+50 Health";

    // ============================================
    // BRUISER CHESTPLATES
    // ============================================
    if (itemName == "Light Vest") return "+15 Armor\n+3 HP per attack";
    if (itemName == "Enchanted Armor") return "+25 Shield at combat start";
    if (itemName == "Guardian Plate") return "+30 Armor\nHeal 3 HP after attack";
    if (itemName == "Juggernaut Armor") return "+35 Health\nReduce first enemy attack by 5";
    if (itemName == "Cloak") return "+20 Armor, +10 Attack";

    // ============================================
    // BRUISER LEGGINGS
    // ============================================
    if (itemName == "Iron Legguards") return "+25 Armor\nReduce damage taken by 3%";
    if (itemName == "Stone Greaves") return "+20 Armor, +10 Health\nGain 5 Shield at combat start";
    if (itemName == "Vanguard Pants") return "+15 Armor, +5 Health on hit\nHeal 2 HP per attack";
    if (itemName == "Juggernaut Leggings") return "+20 Armor\nEnemy attacks deal -2 damage";
    if (itemName == "Titan's Legguards") return "+25 Health\nGain 5 HP per turn";

    // ============================================
    // BRUISER BOOTS
    // ============================================
    if (itemName == "Boots of Steel") return "+30 Armor\nReduce damage taken by 10%";
    if (itemName == "Boots of Mage") return "+35 Armor\nGain Shield vs Mage enemies";
    if (itemName == "Boots of Health") return "+150 Health";
    if (itemName == "Iron Treads") return "+30 Armor\nReduce damage taken by 5%";
    if (itemName == "Boulder Boots") return "+25 Health, +10 Armor\nGain 10 Shield at combat start";

    // ============================================
    // BRUISER TALISMANS
    // ============================================
    if (itemName == "Horn for Battle") return "+150 Health\nArena: Enemy has -15 Attack";
    if (itemName == "Map") return "+30 Health\nHeal 2.5 HP per attack; Execute <25% HP";
    if (itemName == "Chain") return "+10 Health on hit";
    if (itemName == "Crystal") return "+150 Health";
    if (itemName == "Money Shield") return "+125 Health\nGain 65 Silver every 5th dungeon";
    if (itemName == "Scycle") return "+20 Attack\n+20 Silver on dungeon win";

    // ============================================
    // BRUISER COMPANIONS
    // ============================================
    if (itemName == "Rabbit") return "First attack is a guaranteed crit";
    if (itemName == "Turtle") return "+50 XP on dungeon win\nHeal 3 HP per attack, gain Shield";
    if (itemName == "Hedgehog") return "Below 30% HP: Gain 40 Shield";
    if (itemName == "Iron Golem") return "Gain 20 Shield at combat start";
    if (itemName == "Bruiser Merchant Rat") return "Dungeon win: +25 Silver";

    // ============================================
    // RARE ITEMS - ASSASSIN
    // ============================================
    if (itemName == "Venomfang Blade") return "+15 Attack\nApply Open Wound (enemy healing -40%)";
    if (itemName == "Phantom Edge") return "+25 Attack, +25 Armor\nGain 30 Shield after first attack";
    if (itemName == "Shadowpiercer Crossbow") return "+20 Attack, +18 Armor Penetration";
    if (itemName == "Deathwhisper Dagger") return "+20 Attack, +10 Armor Penetration";
    if (itemName == "Rhythm Blade") return "+10 Attack\nEvery 3rd attack guaranteed crit";
    if (itemName == "Soulreaver Bow") return "+30 Attack\nEach attack deals +15 bonus damage";
    if (itemName == "Vampiric Fang") return "+20 Attack, +10 HP on hit";
    if (itemName == "Shield Crusher") return "+20 Attack, +10 Shield\nAttacks remove enemy shield";
    if (itemName == "Executioner's Blade") return "+15 Attack, +250 Health";
    if (itemName == "Armor Shredder") return "+20 Attack, +20 Armor Penetration";
    if (itemName == "Nightstalker's Cowl") return "+20 Armor, +100 Health\n+10 Dex when below 50% HP";
    if (itemName == "Phantom Visage") return "+15 Armor\nDodge the third enemy attack";
    if (itemName == "Veil of Shadows") return "+25 Armor\nFirst crit deals +20 bonus damage";
    if (itemName == "Death's Shroud") return "+10 Armor, +150 Health\n+5 Attack after each dodge";
    if (itemName == "Whisper Mask") return "+30 Armor\nGain 15 Shield when you crit";
    if (itemName == "Shadowweave Armor") return "+30 Armor\nGain 30 Shield at combat start";
    if (itemName == "Nightfall Vest") return "+40 Armor\n+10 Attack when below 50% HP";
    if (itemName == "Cloak of Evasion") return "+25 Armor\n10% chance to dodge any attack";
    if (itemName == "Deathdealer's Garb") return "+35 Armor, +3 HP per attack";
    if (itemName == "Assassin's Shroud") return "+20 Armor, +100 Health\nFirst attack deals +25 damage";
    if (itemName == "Swift Striders") return "+20 Armor, +10 Dex\n+5 Dex after each attack";
    if (itemName == "Phantom Greaves") return "+25 Armor\nDodge 2nd and 4th enemy attacks";
    if (itemName == "Venom-Laced Pants") return "+15 Armor\nPoison: 5 dmg/turn for 3 turns";
    if (itemName == "Shadow Pants") return "+30 Armor\n+10 Attack after dodging";
    if (itemName == "Death's Step") return "+20 Armor, +100 Health\n+5% crit chance";
    if (itemName == "Stalker's Treads") return "+15 Attack, +175 Health";
    if (itemName == "Nightblade Boots") return "+20 Armor\n+15 Attack on first attack";
    if (itemName == "Phantom Step") return "+25 Armor, +5 Dex\nDodge first enemy attack";
    if (itemName == "Deathwalk Boots") return "+15 Attack, +15 Armor\n+5 HP per crit";
    if (itemName == "Silent Runners") return "+30 Armor, +10 Armor Penetration";
    if (itemName == "Heart of Shadows") return "+250 Health\n+5 Attack per kill (dungeon)";
    if (itemName == "Venom Vial") return "+150 Health\nPoison: 8 dmg/turn for 2 turns";
    if (itemName == "Dark Pendant") return "+200 Health\n+20 Attack when below 30% HP";
    if (itemName == "Assassin's Mark") return "+100 Health\n+25% crit damage";
    if (itemName == "Shadow Crystal") return "+175 Health\nGain 20 Shield at combat start";
    if (itemName == "Shadow Panther") return "+15 Attack on first attack\n+5 Dex per dodge";
    if (itemName == "Venomous Spider") return "Apply poison on hit (4 dmg/turn, 3 turns)";
    if (itemName == "Ghost Raven") return "25% chance enemy misses first attack";
    if (itemName == "Blood Bat") return "+8 HP on hit, +100 XP on dungeon win";
    if (itemName == "Phantom Serpent") return "+10 Armor Pen\nCrits apply Open Wound";

    // ============================================
    // RARE ITEMS - MAGE
    // ============================================
    if (itemName == "Inferno Wand") return "+45 Attack\nDeal 45 damage at combat start";
    if (itemName == "Wand of Devastation") return "+25 Attack, +150 Health\nDeal 25 damage at start";
    if (itemName == "Arcane Staff") return "+25 Attack, +70 Health\n+7 Armor Pen, +5 Attack/spell";
    if (itemName == "Grand Rod") return "+60 Attack";
    if (itemName == "Bleeding Orb") return "+30 Attack\nApply Open Wound on attack";
    if (itemName == "Mage Shield Piercer") return "+15 Attack, +10 Shield\nRemove enemy shield";
    if (itemName == "Mask of Embers") return "+30 Attack, +100 Health\nDeal 2 dmg at turn start";
    if (itemName == "Crown of Flames") return "+20 Attack, +15 Armor\n+10 spell dmg below 50% HP";
    if (itemName == "Archmage's Hood") return "+25 Armor, +20 Intel\n+5 Intel per spell";
    if (itemName == "Veil of Sorcery") return "+30 Armor\nBlock third enemy attack";
    if (itemName == "Mindfire Circlet") return "+15 Attack, +100 Health\nCrits deal +15 bonus dmg";
    if (itemName == "Battlemage Cloak") return "+45 Armor, +10 Intelligence";
    if (itemName == "Robes of the Inferno") return "+35 Armor\nBurn: 5 dmg/turn for 2 turns";
    if (itemName == "Arcane Vestment") return "+30 Armor\nGain 25 Shield at combat start";
    if (itemName == "Sorcerer's Mantle") return "+25 Armor, +100 Health\n+5 HP per spell";
    if (itemName == "Spellweaver's Robe") return "+40 Armor\n+8 Attack per spell (max +40)";
    if (itemName == "Mana-Woven Pants") return "+25 Armor, +15 Intel\n+5 Intel below 50% HP";
    if (itemName == "Flamecaster Leggings") return "+20 Armor\nBurn: 4 dmg/turn for 2 turns";
    if (itemName == "Arcane Greaves") return "+30 Armor\nGain 15 Shield at combat start";
    if (itemName == "Sorcerer's Trousers") return "+20 Armor, +150 Health\n+3 HP per turn";
    if (itemName == "Runic Legwraps") return "+25 Armor\n+5 Attack per crit";
    if (itemName == "Boots of the Archmage") return "+20 Attack, +20 Armor\nFirst spell +15 dmg";
    if (itemName == "Flamestep Boots") return "+25 Attack\nDeal 10 damage at combat start";
    if (itemName == "Runic Treads") return "+30 Armor, +15 Intelligence";
    if (itemName == "Spellfire Slippers") return "+15 Attack, +100 Health\n+5 spell dmg/turn";
    if (itemName == "Mage's Striders") return "+35 Armor, +10 Armor Penetration";
    if (itemName == "Flame Orb") return "+30 Attack\nDeal 8 damage at combat start";
    if (itemName == "Arcane Chains") return "+40 Attack, +25 Armor";
    if (itemName == "Golden Aegis") return "+20 Armor\nFirst enemy attack deals 0 damage";
    if (itemName == "Arcane Reliquary") return "+150 Health, +25 Armor\n+10 Shield at start";
    if (itemName == "Orb of Devastation") return "Every 3rd attack deals +50 bonus damage";
    if (itemName == "Crystal of Vitality") return "+250 Health\nHeal 2.5% max HP per attack";
    if (itemName == "Runic Pendant") return "+30 Attack, +12 Armor Penetration";
    if (itemName == "Phoenix Hatchling") return "Deal 20 damage at start\n+5 HP per turn";
    if (itemName == "Arcane Golem") return "+25 Shield at combat start\n+10 Intelligence";
    if (itemName == "Fire Elemental") return "All attacks apply Burn (3 dmg/turn, 2 turns)";
    if (itemName == "Mana Wyrm") return "+8 Attack per spell cast (max +40)";
    if (itemName == "Crystal Guardian") return "+30 Shield at start\nBlock one attack below 25% HP";

    // ============================================
    // RARE ITEMS - BRUISER
    // ============================================
    if (itemName == "Colossus Hammer") return "+15 Attack, +200 Health\n+5 HP on hit";
    if (itemName == "Brutal Cleaver") return "+25 Attack, +5 Armor Penetration";
    if (itemName == "Titan's Axe") return "+20 Attack\nEvery 5th attack guaranteed crit";
    if (itemName == "Bruiser Shield Basher") return "+20 Attack, +10 Shield\nRemove enemy shield";
    if (itemName == "Warlord's Mace") return "+30 Attack, +100 Health\n+3 HP per attack";
    if (itemName == "Bone Crusher") return "+35 Attack\nShred 5 armor/hit (max 25)";
    if (itemName == "Titan's Visage") return "+150 Health, +30 Armor\n+5 HP when hit";
    if (itemName == "Juggernaut Helm") return "+100 Health, +40 Armor\nFirst attack -10 dmg";
    if (itemName == "Warlord's Crown") return "+200 Health, +20 Armor\n+5 Attack below 50% HP";
    if (itemName == "Berserker's Mask") return "+35 Armor\n+10 Attack when below 30% HP";
    if (itemName == "Fortress Helm") return "+50 Armor\nGain 20 Shield at combat start";
    if (itemName == "Spiked Bulwark") return "+30 Armor\nReflect 15 dmg, apply Open Wound";
    if (itemName == "Ironclad Vest") return "+50 Armor, +5 HP per attack";
    if (itemName == "Titan's Plate") return "+40 Armor, +100 Health";
    if (itemName == "Warlord's Mail") return "+40 Armor\nFirst attack -20% damage";
    if (itemName == "Berserker's Harness") return "+30 Armor\n+5 Attack per hit taken (max +25)";
    if (itemName == "Fortress Armor") return "+55 Armor";
    if (itemName == "Titan's Greaves") return "+200 Health\n+5 HP per turn";
    if (itemName == "Juggernaut Pants") return "+35 Armor\nReduce all damage by 5%";
    if (itemName == "Warlord's Pants") return "+150 Health, +20 Armor\n+3 HP per attack";
    if (itemName == "Berserker's Legguards") return "+25 Armor, +100 Health\n+5 Attack below 50% HP";
    if (itemName == "Fortress Greaves") return "+250 Health\n+10 Shield at combat start";
    if (itemName == "Titan's Treads") return "+175 Health, +20 Armor\n+3 HP per turn";
    if (itemName == "Juggernaut Boots") return "+40 Armor\nReduce damage by 8%";
    if (itemName == "Warlord's Stompers") return "+150 Health\n+15 Shield at combat start";
    if (itemName == "Berserker's Boots") return "+30 Armor\n+10 Attack when below 40% HP";
    if (itemName == "Fortress Steps") return "+35 Armor, +100 Health";
    if (itemName == "Heart of the Mountain") return "+300 Health, +10 HP on hit";
    if (itemName == "Warlord's Emblem") return "+250 Health\nApply Open Wound on attack";
    if (itemName == "Titan's Heart") return "+350 Health\n+5 Shield per turn";
    if (itemName == "Berserker's Totem") return "+200 Health\n+15 Attack when below 30% HP";
    if (itemName == "Stone of Vitality") return "+250 Health\n+3% max HP heal per attack";
    if (itemName == "Iron Pendant") return "+200 Health, +30 Armor";
    if (itemName == "War Bear") return "+40 Shield at start\n+5 HP per attack";
    if (itemName == "Iron Guardian") return "+55 Shield at combat start";
    if (itemName == "Stone Tortoise") return "+30 Shield, +5 HP/turn\n+75 XP on win";
    if (itemName == "Berserker Wolf") return "+10 Attack per hit taken (max +30)\nFirst attack crits";
    if (itemName == "Mountain Golem") return "+25 Shield at start\nReduce damage by 5%";

    return "No description available";
}

void CharacterSheet::checkEquipmentHover(const RenderWindow& window) {
    if (!visible) return;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    hoveredSlotIndex = -1;
    hoveredInventoryIndex = -1;

    // Check armor slots
    for (int i = 0; i < armorSlots.size(); ++i) {
        if (armorSlots[i].getGlobalBounds().contains(mousePosF)) {
            EquipmentSlot slot;
            if (i == 0) slot = EquipmentSlot::Helmet;
            else if (i == 1) slot = EquipmentSlot::Chestplate;
            else if (i == 2) slot = EquipmentSlot::Pants;
            else if (i == 3) slot = EquipmentSlot::Boots;
            else continue;

            string itemName = getEquippedItem(slot);
            if (!itemName.empty()) {
                hoveredSlotIndex = i;
                return;
            }
        }
    }

    // Check other slots (weapon, talisman, companion)
    for (int i = 0; i < otherSlots.size(); ++i) {
        if (otherSlots[i].getGlobalBounds().contains(mousePosF)) {
            EquipmentSlot slot;
            if (i == 0) slot = EquipmentSlot::Weapon;
            else if (i == 1) slot = EquipmentSlot::Talisman;
            else if (i == 2) slot = EquipmentSlot::Companion;
            else continue;

            string itemName = getEquippedItem(slot);
            if (!itemName.empty()) {
                hoveredSlotIndex = 100 + i;
                return;
            }
        }
    }

    // Check inventory items for hover
    for (int i = 0; i < inventoryDisplayItems.size(); ++i) {
        if (inventoryDisplayItems[i].slot.getGlobalBounds().contains(mousePosF)) {
            if (!inventoryDisplayItems[i].itemName.empty()) {
                hoveredInventoryIndex = i;
                return;
            }
        }
    }
}

string CharacterSheet::checkInventoryItemClick(const RenderWindow& window) {
    if (!visible) return "";

    // If showing confirmation popup, don't process other clicks
    if (showingSellConfirmation) return "";

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Check if Equip button was clicked
    if (selectedInventoryIndex >= 0 && selectedInventoryIndex < inventoryDisplayItems.size()) {
        if (equipButton.getGlobalBounds().contains(mousePosF)) {
            string itemToEquip = selectedInventoryItem;
            selectedInventoryIndex = -1;
            selectedInventoryItem = "";
            return itemToEquip;
        }

        // Check if Sell button was clicked
        if (sellButton.getGlobalBounds().contains(mousePosF)) {
            showingSellConfirmation = true;
            pendingSellPrice = static_cast<int>(getItemShopPrice(selectedInventoryItem) * 0.75);

            // Update popup text
            string confirmText = "Sell for " + to_string(pendingSellPrice) + " silver?";
            confirmPopupText.setString(confirmText);
            FloatRect textBounds = confirmPopupText.getLocalBounds();
            confirmPopupText.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
            confirmPopupText.setPosition({
                windowWidth / 2.f,
                confirmPopupBox.getPosition().y + 50.f
                });

            // Position Sure text
            FloatRect sureBounds = confirmSureText.getLocalBounds();
            confirmSureText.setOrigin({ sureBounds.size.x / 2.f, sureBounds.size.y / 2.f });
            confirmSureText.setPosition({
                confirmSureButton.getPosition().x + 60.f,
                confirmSureButton.getPosition().y + 25.f
                });

            // Position No text
            FloatRect noBounds = confirmNoText.getLocalBounds();
            confirmNoText.setOrigin({ noBounds.size.x / 2.f, noBounds.size.y / 2.f });
            confirmNoText.setPosition({
                confirmNoButton.getPosition().x + 60.f,
                confirmNoButton.getPosition().y + 25.f
                });

            return "";
        }
    }

    // Check each inventory display item
    for (size_t i = 0; i < inventoryDisplayItems.size(); ++i) {
        const auto& item = inventoryDisplayItems[i];
        FloatRect bounds = item.slot.getGlobalBounds();

        if (bounds.contains(mousePosF)) {
            if (!item.itemName.empty()) {
                // Check for double-click
                if (static_cast<int>(i) == lastClickedInventoryIndex &&
                    inventoryDoubleClickClock.getElapsedTime().asSeconds() < doubleClickTime) {
                    // Double-click - equip immediately
                    lastClickedInventoryIndex = -1;
                    selectedInventoryIndex = -1;
                    selectedInventoryItem = "";
                    return item.itemName;
                }
                else {
                    // Single click - select item and show buttons
                    lastClickedInventoryIndex = static_cast<int>(i);
                    selectedInventoryIndex = static_cast<int>(i);
                    selectedInventoryItem = item.itemName;
                    inventoryDoubleClickClock.restart();

                    // Position Equip button above the item
                    equipButton.setPosition({
                        bounds.position.x,
                        bounds.position.y - 35.f
                        });
                    FloatRect equipTextBounds = equipButtonText.getLocalBounds();
                    equipButtonText.setOrigin({ equipTextBounds.size.x / 2.f, equipTextBounds.size.y / 2.f });
                    equipButtonText.setPosition({
                        equipButton.getPosition().x + equipButton.getSize().x / 2.f,
                        equipButton.getPosition().y + equipButton.getSize().y / 2.f - 2.f
                        });

                    // Position Sell button next to Equip button
                    sellButton.setPosition({
                        bounds.position.x + 85.f,
                        bounds.position.y - 35.f
                        });
                    FloatRect sellTextBounds = sellButtonText.getLocalBounds();
                    sellButtonText.setOrigin({ sellTextBounds.size.x / 2.f, sellTextBounds.size.y / 2.f });
                    sellButtonText.setPosition({
                        sellButton.getPosition().x + sellButton.getSize().x / 2.f,
                        sellButton.getPosition().y + sellButton.getSize().y / 2.f - 2.f
                        });
                }
                return "";
            }
        }
    }

    // Click elsewhere in character sheet - deselect inventory item
    if (sheetBackground.getGlobalBounds().contains(mousePosF)) {
        // Don't deselect if clicking on equipment slots or buttons
        bool clickedOnEquipSlot = false;
        for (size_t i = 0; i < 4 && i < armorSlots.size(); i++) {
            if (armorSlots[i].getGlobalBounds().contains(mousePosF)) {
                clickedOnEquipSlot = true;
                break;
            }
        }
        for (size_t i = 0; i < 4 && i < otherSlots.size(); i++) {
            if (otherSlots[i].getGlobalBounds().contains(mousePosF)) {
                clickedOnEquipSlot = true;
                break;
            }
        }

        if (!clickedOnEquipSlot && selectedInventoryIndex >= 0) {
            if (selectedInventoryIndex < inventoryDisplayItems.size()) {
                if (!equipButton.getGlobalBounds().contains(mousePosF) &&
                    !sellButton.getGlobalBounds().contains(mousePosF)) {
                    selectedInventoryIndex = -1;
                    selectedInventoryItem = "";
                }
            }
        }
    }

    return "";
}

void CharacterSheet::updateEquipmentVisuals() {
    // Clear old equipped names
    armorEquippedNames.clear();
    otherEquippedNames.clear();

    // Update armor slots (Helmet, Chestplate, Pants, Boots)
    EquipmentSlot armorSlotTypes[] = {
        EquipmentSlot::Helmet,
        EquipmentSlot::Chestplate,
        EquipmentSlot::Pants,
        EquipmentSlot::Boots
    };

    for (int i = 0; i < 4; ++i) {
        string equippedItem = getEquippedItem(armorSlotTypes[i]);

        // Update slot color
        if (!equippedItem.empty() && equippedItem != "Empty") {
            armorSlots[i].setFillColor(Color(80, 150, 80));  // Green when equipped
            armorSlots[i].setOutlineColor(Color(100, 255, 100));  // Bright green outline
        }
        else {
            armorSlots[i].setFillColor(Color(50, 50, 60));  // Dark when empty
            armorSlots[i].setOutlineColor(Color(150, 150, 200));  // Gray outline
        }

        // Create text for equipped item name
        Text equippedName(font);
        if (!equippedItem.empty() && equippedItem != "Empty") {
            equippedName.setString(equippedItem);
            equippedName.setCharacterSize(14);
            equippedName.setFillColor(Color::White);
            equippedName.setStyle(Text::Bold);

            FloatRect bounds = armorSlots[i].getGlobalBounds();
            FloatRect textBounds = equippedName.getLocalBounds();
            equippedName.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
            equippedName.setPosition({
                bounds.position.x + bounds.size.x / 2.f,
                bounds.position.y + bounds.size.y / 2.f
                });
        }
        armorEquippedNames.push_back(equippedName);
    }

    // Update other slots (Weapon, Talisman, Companion)
    EquipmentSlot otherSlotTypes[] = {
        EquipmentSlot::Weapon,
        EquipmentSlot::Talisman,
        EquipmentSlot::Companion
    };

    for (int i = 0; i < 3; ++i) {  // Only 3 (not 4, skipping Curse slot)
        string equippedItem = getEquippedItem(otherSlotTypes[i]);

        // Update slot color
        if (!equippedItem.empty() && equippedItem != "Empty") {
            otherSlots[i].setFillColor(Color(150, 100, 180));  // Purple when equipped
            otherSlots[i].setOutlineColor(Color(255, 150, 255));  // Bright purple outline
        }
        else {
            otherSlots[i].setFillColor(Color(50, 50, 60));  // Dark when empty
            otherSlots[i].setOutlineColor(Color(180, 100, 255));  // Purple outline
        }

        // Create text for equipped item name
        Text equippedName(font);
        if (!equippedItem.empty() && equippedItem != "Empty") {
            equippedName.setString(equippedItem);
            equippedName.setCharacterSize(14);
            equippedName.setFillColor(Color::White);
            equippedName.setStyle(Text::Bold);

            FloatRect bounds = otherSlots[i].getGlobalBounds();
            FloatRect textBounds = equippedName.getLocalBounds();
            equippedName.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
            equippedName.setPosition({
                bounds.position.x + bounds.size.x / 2.f,
                bounds.position.y + bounds.size.y / 2.f
                });
        }
        otherEquippedNames.push_back(equippedName);
    }
}

void CharacterSheet::updateStatsDisplay() {
    cout << "DEBUG: updateStatsDisplay() called" << endl;

    // Update stat displays with equipment bonuses
    statsTexts.clear();

    string statNames[] = { "Health", "Attack", "Intelligence", "Dexterity", "Armor", "Luck" };

    float statsY = sheetBackground.getPosition().y + 730.f;
    float centerX = sheetBackground.getPosition().x + 800.f;
    float statsStartX = centerX - 300.f;

    for (int i = 0; i < 6; ++i) {
        // Get base and equipment stats
        double baseStat = 0.0;
        double equipStat = getEquipmentStat(statNames[i]);
        double totalStat = getStat(statNames[i]);

        cout << "DEBUG: " << statNames[i] << " - Base: ??? Equipment: " << equipStat << " Total: " << totalStat << endl;

        // Build display string
        char buffer[80];
        if (equipStat > 0.01) {  // Small threshold for floating point
            // Show: "Stat: base + equip = total"
            if (statNames[i] == "Health") baseStat = baseStats.health;
            else if (statNames[i] == "Attack") baseStat = baseStats.attack;
            else if (statNames[i] == "Intelligence") baseStat = baseStats.intelligence;
            else if (statNames[i] == "Dexterity") baseStat = baseStats.dexterity;
            else if (statNames[i] == "Armor") baseStat = baseStats.armor;
            else if (statNames[i] == "Luck") baseStat = baseStats.luck;

            snprintf(buffer, sizeof(buffer), "%s: %.0f +%.0f = %.0f",
                statNames[i].c_str(), baseStat, equipStat, totalStat);
            cout << "DEBUG: Display string: " << buffer << endl;
        }
        else {
            // No equipment bonus, show total only
            snprintf(buffer, sizeof(buffer), "%s: %.0f", statNames[i].c_str(), totalStat);
            cout << "DEBUG: Display string (no equip): " << buffer << endl;
        }

        Text statText(font);
        statText.setString(buffer);
        statText.setCharacterSize(24);

        // Color based on whether there's equipment bonus
        if (equipStat > 0.01) {
            statText.setFillColor(Color(150, 255, 150));  // Bright green with equipment
        }
        else {
            statText.setFillColor(Color(200, 255, 200));  // Normal green
        }

        if (i < 3) {
            statText.setPosition({ statsStartX, statsY + i * 35.f });
        }
        else {
            statText.setPosition({ statsStartX + 400.f, statsY + (i - 3) * 35.f });
        }

        statsTexts.push_back(statText);
    }

    // Re-add dodge chance display (Assassin only)
    if (classIndex == 0) {
        Text dodgeText(font);
        char dodgeBuffer[50];
        snprintf(dodgeBuffer, sizeof(dodgeBuffer), "Dodge: %.2f%%", dodgeChance);
        dodgeText.setString(dodgeBuffer);
        dodgeText.setCharacterSize(26);
        dodgeText.setFillColor(Color(100, 255, 100));
        dodgeText.setPosition({ statsStartX, statsY + 120.f });
        statsTexts.push_back(dodgeText);
    }

    // Add inventory title
    float inventoryY = sheetBackground.getPosition().y + 860.f;
    Text inventoryTitle(font);
    inventoryTitle.setString("Inventory");
    inventoryTitle.setCharacterSize(35);
    inventoryTitle.setFillColor(Color(255, 215, 0));
    FloatRect invTitleBounds = inventoryTitle.getLocalBounds();
    inventoryTitle.setOrigin({ invTitleBounds.size.x / 2.f, 0.f });
    inventoryTitle.setPosition({ centerX, inventoryY - 50.f });
    statsTexts.push_back(inventoryTitle);
}

map<int, string> CharacterSheet::saveEquipment() const {
    map<int, string> savedEquipment;
    for (const auto& pair : equipment) {
        savedEquipment[static_cast<int>(pair.first)] = pair.second;
    }
    return savedEquipment;
}

void CharacterSheet::loadEquipment(const map<int, string>& savedEquipment) {
    equipment.clear();
    for (const auto& pair : savedEquipment) {
        equipment[static_cast<EquipmentSlot>(pair.first)] = pair.second;
    }
    updateEquipmentVisuals();
    updateStatsDisplay();
    cout << "Loaded " << savedEquipment.size() << " equipped items" << endl;
}
void CharacterSheet::unequipItem(EquipmentSlot slot, Inventory* inventory) {
    // Get the item name safely
    string itemName = getEquippedItem(slot);

    // Check if slot is actually empty (no item or "Empty" string)
    if (itemName.empty() || itemName == "Empty") {
        cout << "[Unequip] No item in this slot" << endl;
        return;
    }

    cout << "[Unequip] Attempting to unequip: " << itemName << endl;

    // Add back to inventory
    if (inventory) {
        bool addSuccess = inventory->addItem(itemName);
        cout << "[Unequip] addItem returned: " << (addSuccess ? "SUCCESS" : "FAILED") << endl;

        if (addSuccess) {
            cout << "[Unequip] " << itemName << " returned to inventory" << endl;

            // Remove from equipment slot
            equipment[slot] = "";

            // Recalculate stats
            loadRaceStats();
            updateStatsDisplay();
            updateEquipmentVisuals();

            // Force inventory display update
            updateInventoryDisplay();

            cout << "[Unequip] Equipment cleared, stats updated, inventory refreshed" << endl;
        }
        else {
            cout << "[Unequip ERROR] Failed to add " << itemName << " to inventory!" << endl;
            return; // Don't remove from equipment if inventory add failed
        }
    }
    else {
        cout << "[Unequip ERROR] Inventory pointer is NULL!" << endl;
    }

    // Hide the unequip button
    selectedEquipmentSlot = -1;
}

int CharacterSheet::checkEquippedItemClick(const RenderWindow& window) {
    if (!visible) return -1;

    // Don't process equipment clicks if sell confirmation is showing
    if (showingSellConfirmation) return -1;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // CRITICAL: Only process clicks INSIDE the character sheet background
    // This prevents triggering when clicking inventory items
    if (!sheetBackground.getGlobalBounds().contains(mousePosF)) {
        // Click is outside character sheet - hide button and ignore
        selectedEquipmentSlot = -1;
        return -1;
    }

    // First, check if the unequip button itself was clicked
    if (selectedEquipmentSlot != -1) {
        if (unequipButton.getGlobalBounds().contains(mousePosF)) {
            EquipmentSlot slot = static_cast<EquipmentSlot>(selectedEquipmentSlot);
            unequipItem(slot, inventory);
            return -1;
        }
    }

    // Check armor slots (Helmet, Chestplate, Pants, Boots) - only first 4 slots
    for (size_t i = 0; i < 4 && i < armorSlots.size(); i++) {
        if (armorSlots[i].getGlobalBounds().contains(mousePosF)) {
            EquipmentSlot slot;
            if (i == 0) slot = EquipmentSlot::Helmet;
            else if (i == 1) slot = EquipmentSlot::Chestplate;
            else if (i == 2) slot = EquipmentSlot::Pants;
            else slot = EquipmentSlot::Boots;

            int slotIndex = static_cast<int>(slot);

            // Get item name and check if slot actually has an item (not empty or "Empty")
            string itemName = getEquippedItem(slot);
            if (!itemName.empty() && itemName != "Empty") {
                // Check for double-click
                if (slotIndex == lastClickedSlot &&
                    doubleClickClock.getElapsedTime().asSeconds() < doubleClickTime) {
                    // Double-click detected = instant unequip
                    unequipItem(slot, inventory);
                    lastClickedSlot = -1;
                    selectedEquipmentSlot = -1;
                    return -1;
                }

                // Single click = show unequip button
                selectedEquipmentSlot = slotIndex;
                lastClickedSlot = slotIndex;
                doubleClickClock.restart();

                // Position unequip button next to the clicked slot
                unequipButton.setPosition({
                    armorSlots[i].getPosition().x + armorSlots[i].getSize().x + 20.f,
                    armorSlots[i].getPosition().y + 10.f
                    });

                // Center the text on the button
                FloatRect textBounds = unequipButtonText.getLocalBounds();
                unequipButtonText.setPosition({
                    unequipButton.getPosition().x + (unequipButton.getSize().x - textBounds.size.x) / 2.f,
                    unequipButton.getPosition().y + (unequipButton.getSize().y - textBounds.size.y) / 2.f - 5.f
                    });

                return slotIndex;
            }
        }
    }

    // Check other slots (Weapon, Talisman, Companion, Curse) - only first 4 slots
    for (size_t i = 0; i < 4 && i < otherSlots.size(); i++) {
        if (otherSlots[i].getGlobalBounds().contains(mousePosF)) {
            EquipmentSlot slot;
            if (i == 0) slot = EquipmentSlot::Weapon;
            else if (i == 1) slot = EquipmentSlot::Talisman;
            else if (i == 2) slot = EquipmentSlot::Companion;
            else slot = EquipmentSlot::Curse;

            int slotIndex = static_cast<int>(slot);

            // Get item name and check if slot actually has an item (not empty or "Empty")
            string itemName = getEquippedItem(slot);
            if (!itemName.empty() && itemName != "Empty") {
                // Check for double-click
                if (slotIndex == lastClickedSlot &&
                    doubleClickClock.getElapsedTime().asSeconds() < doubleClickTime) {
                    // Double-click detected = instant unequip
                    unequipItem(slot, inventory);
                    lastClickedSlot = -1;
                    selectedEquipmentSlot = -1;
                    return -1;
                }

                // Single click = show unequip button
                selectedEquipmentSlot = slotIndex;
                lastClickedSlot = slotIndex;
                doubleClickClock.restart();

                // Position unequip button next to the clicked slot
                unequipButton.setPosition({
                    otherSlots[i].getPosition().x + otherSlots[i].getSize().x + 20.f,
                    otherSlots[i].getPosition().y + 10.f
                    });

                // Center the text on the button
                FloatRect textBounds = unequipButtonText.getLocalBounds();
                unequipButtonText.setPosition({
                    unequipButton.getPosition().x + (unequipButton.getSize().x - textBounds.size.x) / 2.f,
                    unequipButton.getPosition().y + (unequipButton.getSize().y - textBounds.size.y) / 2.f - 5.f
                    });

                return slotIndex;
            }
        }
    }

    // Click outside any slot = hide unequip button
    selectedEquipmentSlot = -1;
    return -1;
}

bool CharacterSheet::checkConfirmSellClick(const RenderWindow& window) {
    if (!showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    FloatRect sureBounds = confirmSureButton.getGlobalBounds();
    cout << "[DEBUG] Mouse: (" << mousePosF.x << ", " << mousePosF.y << ")" << endl;
    cout << "[DEBUG] Sure button bounds: (" << sureBounds.position.x << ", " << sureBounds.position.y
        << ") size: (" << sureBounds.size.x << ", " << sureBounds.size.y << ")" << endl;

    if (sureBounds.contains(mousePosF)) {
        cout << "[DEBUG] Sure button HIT!" << endl;
        showingSellConfirmation = false;
        return true;
    }
    return false;
}

bool CharacterSheet::checkCancelSellClick(const RenderWindow& window) {
    if (!showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    FloatRect noBounds = confirmNoButton.getGlobalBounds();
    cout << "[DEBUG] No button bounds: (" << noBounds.position.x << ", " << noBounds.position.y
        << ") size: (" << noBounds.size.x << ", " << noBounds.size.y << ")" << endl;

    if (noBounds.contains(mousePosF)) {
        cout << "[DEBUG] No button HIT!" << endl;
        showingSellConfirmation = false;
        return true;
    }
    return false;
}

bool CharacterSheet::checkClickOutsidePopup(const RenderWindow& window) {
    if (!showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (!confirmPopupBox.getGlobalBounds().contains(mousePosF)) {
        showingSellConfirmation = false;
        return true;
    }
    return false;
}

void CharacterSheet::clearInventorySelection() {
    selectedInventoryIndex = -1;
    selectedInventoryItem = "";
    lastClickedInventoryIndex = -1;
    showingSellConfirmation = false;
}

int CharacterSheet::getItemShopPrice(const string& itemName) const {
    return ItemDatabase::getInstance().getItemPrice(itemName);
}