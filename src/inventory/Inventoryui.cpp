#include "InventoryUI.h"
#include "ItemDatabase.h"
#include <iostream>

InventoryUI::InventoryUI(float width, float height)
    : visible(false), windowWidth(width), windowHeight(height), lastClickedIndex(-1), doubleClickTime(0.3f),
    selectedItemIndex(-1), selectedItemName("") {

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Error loading font for InventoryUI!" << endl;
    }

    // Create background panel (centered on screen)
    float bgWidth = 1000.0f;
    float bgHeight = 800.0f;
    background.setSize({ bgWidth, bgHeight });
    background.setFillColor(Color(30, 30, 50, 240));
    background.setOutlineThickness(4.0f);
    background.setOutlineColor(Color(100, 100, 150));
    background.setPosition({
        (width - bgWidth) / 2.0f,
        (height - bgHeight) / 2.0f
        });

    // Title
    titleText = new Text(font);
    titleText->setString("INVENTORY");
    titleText->setCharacterSize(48);
    titleText->setFillColor(Color(255, 215, 0));
    titleText->setStyle(Text::Bold);
    FloatRect titleBounds = titleText->getLocalBounds();
    titleText->setOrigin({ titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f });
    titleText->setPosition({
        background.getPosition().x + bgWidth / 2.0f,
        background.getPosition().y + 40.0f
        });

    // Close button (top right of panel)
    closeButton.setSize({ 100.0f, 50.0f });
    closeButton.setFillColor(Color(150, 50, 50));
    closeButton.setOutlineThickness(2.0f);
    closeButton.setOutlineColor(Color::White);
    closeButton.setPosition({
        background.getPosition().x + bgWidth - 120.0f,
        background.getPosition().y + 20.0f
        });

    closeButtonText = new Text(font);
    closeButtonText->setString("Close");
    closeButtonText->setCharacterSize(24);
    closeButtonText->setFillColor(Color::White);
    closeButtonText->setStyle(Text::Bold);
    FloatRect closeBounds = closeButtonText->getLocalBounds();
    closeButtonText->setOrigin({ closeBounds.size.x / 2.0f, closeBounds.size.y / 2.0f });
    closeButtonText->setPosition({
        closeButton.getPosition().x + 50.0f,
        closeButton.getPosition().y + 25.0f
        });

    // Item display settings
    itemWidth = 280.0f;
    itemHeight = 80.0f;
    itemSpacing = 20.0f;
    itemsPerRow = 3;
    itemStartX = background.getPosition().x + 40.0f;
    itemStartY = background.getPosition().y + 120.0f;

    // Setup Equip button (green background)
    equipButton.setSize({ 120.0f, 40.0f });
    equipButton.setFillColor(Color(50, 180, 50));
    equipButton.setOutlineThickness(2.0f);
    equipButton.setOutlineColor(Color::White);

    equipButtonText = new Text(font);
    equipButtonText->setString("Equip");
    equipButtonText->setCharacterSize(20);
    equipButtonText->setFillColor(Color::White);
    equipButtonText->setStyle(Text::Bold);

    // Setup Sell button (orange/yellow background)
    sellButton.setSize({ 120.0f, 40.0f });
    sellButton.setFillColor(Color(200, 150, 50));
    sellButton.setOutlineThickness(2.0f);
    sellButton.setOutlineColor(Color::White);

    sellButtonText = new Text(font);
    sellButtonText->setString("Sell");
    sellButtonText->setCharacterSize(20);
    sellButtonText->setFillColor(Color::White);
    sellButtonText->setStyle(Text::Bold);

    // Setup confirmation popup
    showingSellConfirmation = false;
    pendingSellPrice = 0;

    // Semi-transparent overlay
    confirmPopupBackground.setSize({ windowWidth, windowHeight });
    confirmPopupBackground.setFillColor(Color(0, 0, 0, 150));
    confirmPopupBackground.setPosition({ 0.0f, 0.0f });

    // Popup box (centered)
    float popupWidth = 400.0f;
    float popupHeight = 200.0f;
    confirmPopupBox.setSize({ popupWidth, popupHeight });
    confirmPopupBox.setFillColor(Color(40, 40, 60, 250));
    confirmPopupBox.setOutlineThickness(3.0f);
    confirmPopupBox.setOutlineColor(Color(255, 215, 0));
    confirmPopupBox.setPosition({
        (windowWidth - popupWidth) / 2.0f,
        (windowHeight - popupHeight) / 2.0f
        });

    // Popup text
    confirmPopupText = new Text(font);
    confirmPopupText->setString("Are you sure?");
    confirmPopupText->setCharacterSize(32);
    confirmPopupText->setFillColor(Color::White);
    confirmPopupText->setStyle(Text::Bold);
    FloatRect popupTextBounds = confirmPopupText->getLocalBounds();
    confirmPopupText->setOrigin({ popupTextBounds.size.x / 2.0f, popupTextBounds.size.y / 2.0f });
    confirmPopupText->setPosition({
        windowWidth / 2.0f,
        confirmPopupBox.getPosition().y + 50.0f
        });

    // Sure button (green)
    confirmSureButton.setSize({ 120.0f, 50.0f });
    confirmSureButton.setFillColor(Color(50, 180, 50));
    confirmSureButton.setOutlineThickness(2.0f);
    confirmSureButton.setOutlineColor(Color::White);
    confirmSureButton.setPosition({
        confirmPopupBox.getPosition().x + 50.0f,
        confirmPopupBox.getPosition().y + 120.0f
        });

    confirmSureText = new Text(font);
    confirmSureText->setString("Sure");
    confirmSureText->setCharacterSize(24);
    confirmSureText->setFillColor(Color::White);
    confirmSureText->setStyle(Text::Bold);
    FloatRect sureBounds = confirmSureText->getLocalBounds();
    confirmSureText->setOrigin({ sureBounds.size.x / 2.0f, sureBounds.size.y / 2.0f });
    confirmSureText->setPosition({
        confirmSureButton.getPosition().x + 60.0f,
        confirmSureButton.getPosition().y + 25.0f
        });

    // No button (red)
    confirmNoButton.setSize({ 120.0f, 50.0f });
    confirmNoButton.setFillColor(Color(180, 50, 50));
    confirmNoButton.setOutlineThickness(2.0f);
    confirmNoButton.setOutlineColor(Color::White);
    confirmNoButton.setPosition({
        confirmPopupBox.getPosition().x + popupWidth - 170.0f,
        confirmPopupBox.getPosition().y + 120.0f
        });

    confirmNoText = new Text(font);
    confirmNoText->setString("No");
    confirmNoText->setCharacterSize(24);
    confirmNoText->setFillColor(Color::White);
    confirmNoText->setStyle(Text::Bold);
    FloatRect noBounds = confirmNoText->getLocalBounds();
    confirmNoText->setOrigin({ noBounds.size.x / 2.0f, noBounds.size.y / 2.0f });
    confirmNoText->setPosition({
        confirmNoButton.getPosition().x + 60.0f,
        confirmNoButton.getPosition().y + 25.0f
        });
}

InventoryUI::~InventoryUI() {
    delete titleText;
    delete closeButtonText;
    delete equipButtonText;
    delete sellButtonText;
    delete confirmPopupText;
    delete confirmSureText;
    delete confirmNoText;
}

FloatRect InventoryUI::getItemBounds(int index) const {
    int row = index / itemsPerRow;
    int col = index % itemsPerRow;

    float x = itemStartX + col * (itemWidth + itemSpacing);
    float y = itemStartY + row * (itemHeight + itemSpacing);

    return FloatRect({ x, y }, { itemWidth, itemHeight });
}

void InventoryUI::draw(RenderWindow& window, Inventory* inventory) {
    if (!visible || !inventory) return;

    // Draw background
    window.draw(background);
    window.draw(*titleText);
    window.draw(closeButton);
    window.draw(*closeButtonText);

    // Get all items from inventory
    vector<string> items = inventory->getAllItems();

    // Draw each item
    for (int i = 0; i < items.size(); ++i) {
        FloatRect itemBounds = getItemBounds(i);

        // Item box
        RectangleShape itemBox;
        itemBox.setSize({ itemBounds.size.x, itemBounds.size.y });
        itemBox.setPosition({ itemBounds.position.x, itemBounds.position.y });

        // Highlight selected item
        if (i == selectedItemIndex) {
            itemBox.setFillColor(Color(70, 100, 70, 220));
            itemBox.setOutlineThickness(3.0f);
            itemBox.setOutlineColor(Color(100, 200, 100));
        }
        else {
            itemBox.setFillColor(Color(50, 50, 70, 200));
            itemBox.setOutlineThickness(2.0f);
            itemBox.setOutlineColor(Color(100, 100, 120));
        }
        window.draw(itemBox);

        // Item name
        Text itemName(font);
        itemName.setString(items[i]);
        itemName.setCharacterSize(22);
        itemName.setFillColor(Color::White);
        itemName.setStyle(Text::Bold);
        itemName.setPosition({
            itemBounds.position.x + 10.0f,
            itemBounds.position.y + 10.0f
            });
        window.draw(itemName);

        // Item count
        int count = inventory->getItemCount(items[i]);
        Text itemCount(font);
        itemCount.setString("x" + to_string(count));
        itemCount.setCharacterSize(20);
        itemCount.setFillColor(Color(200, 200, 200));
        itemCount.setPosition({
            itemBounds.position.x + 10.0f,
            itemBounds.position.y + 45.0f
            });
        window.draw(itemCount);
    }

    // Draw Equip and Sell buttons if an item is selected
    if (selectedItemIndex >= 0 && selectedItemIndex < items.size()) {
        window.draw(equipButton);
        window.draw(*equipButtonText);
        window.draw(sellButton);
        window.draw(*sellButtonText);
    }

    // If inventory is empty, show message
    if (items.empty()) {
        Text emptyText(font);
        emptyText.setString("Your inventory is empty");
        emptyText.setCharacterSize(28);
        emptyText.setFillColor(Color(150, 150, 150));
        emptyText.setStyle(Text::Italic);
        FloatRect emptyBounds = emptyText.getLocalBounds();
        emptyText.setOrigin({ emptyBounds.size.x / 2.0f, emptyBounds.size.y / 2.0f });
        emptyText.setPosition({
            background.getPosition().x + background.getSize().x / 2.0f,
            background.getPosition().y + background.getSize().y / 2.0f
            });
        window.draw(emptyText);
    }

    // Draw sell confirmation popup if active
    if (showingSellConfirmation) {
        window.draw(confirmPopupBackground);
        window.draw(confirmPopupBox);
        window.draw(*confirmPopupText);
        window.draw(confirmSureButton);
        window.draw(*confirmSureText);
        window.draw(confirmNoButton);
        window.draw(*confirmNoText);
    }
}

bool InventoryUI::checkCloseClick(const RenderWindow& window) {
    if (!visible) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    return closeButton.getGlobalBounds().contains({
        static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y)
        });
}

int InventoryUI::checkItemClick(const RenderWindow& window) {
    if (!visible) return -1;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Check which item was clicked
    for (int i = 0; i < 50; ++i) {  // Max items to check
        FloatRect itemBounds = getItemBounds(i);
        if (itemBounds.contains(mousePosF)) {
            return i;
        }
    }

    return -1;
}

void InventoryUI::checkHover(const RenderWindow& window) {
    if (!visible) return;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Hover effect for close button
    if (closeButton.getGlobalBounds().contains(mousePosF)) {
        closeButton.setFillColor(Color(200, 70, 70));
    }
    else {
        closeButton.setFillColor(Color(150, 50, 50));
    }
}

void InventoryUI::show() {
    visible = true;
}

void InventoryUI::hide() {
    visible = false;
    selectedItemIndex = -1;
    selectedItemName = "";
    lastClickedIndex = -1;
    showingSellConfirmation = false;
}

string InventoryUI::checkItemDoubleClick(const RenderWindow& window, Inventory* inventory) {
    if (!visible || !inventory) return "";

    // If showing confirmation popup, don't process item clicks
    if (showingSellConfirmation) return "";

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // First check if Equip button was clicked
    if (selectedItemIndex >= 0) {
        if (equipButton.getGlobalBounds().contains(mousePosF)) {
            // Equip button clicked - return the selected item name
            string itemToEquip = selectedItemName;
            selectedItemIndex = -1;
            selectedItemName = "";
            return itemToEquip;
        }

        // Check if Sell button was clicked
        if (sellButton.getGlobalBounds().contains(mousePosF)) {
            // Show confirmation popup
            showingSellConfirmation = true;

            // Calculate sell price (75% of shop price)
            pendingSellPrice = static_cast<int>(getItemShopPrice(selectedItemName) * 0.75);

            // Update popup text to show price
            string confirmText = "Sell for " + to_string(pendingSellPrice) + " silver?";
            confirmPopupText->setString(confirmText);
            FloatRect popupTextBounds = confirmPopupText->getLocalBounds();
            confirmPopupText->setOrigin({ popupTextBounds.size.x / 2.0f, popupTextBounds.size.y / 2.0f });
            confirmPopupText->setPosition({
                windowWidth / 2.0f,
                confirmPopupBox.getPosition().y + 50.0f
                });

            return ""; // Don't equip, show popup instead
        }
    }

    vector<string> items = inventory->getAllItems();

    // Check which item was clicked
    for (int i = 0; i < items.size(); ++i) {
        FloatRect itemBounds = getItemBounds(i);
        if (itemBounds.contains(mousePosF)) {
            // Check if this is the same item clicked recently (double-click)
            if (i == lastClickedIndex && doubleClickClock.getElapsedTime().asSeconds() < doubleClickTime) {
                // Double-click detected - equip immediately
                lastClickedIndex = -1;
                selectedItemIndex = -1;
                selectedItemName = "";
                return items[i]; // Return item name to equip
            }
            else {
                // Single click - select item and show Equip/Sell buttons
                lastClickedIndex = i;
                selectedItemIndex = i;
                selectedItemName = items[i];
                doubleClickClock.restart();

                // Position Equip button next to the selected item
                equipButton.setPosition({
                    itemBounds.position.x + itemBounds.size.x + 10.0f,
                    itemBounds.position.y + (itemBounds.size.y - equipButton.getSize().y) / 2.0f - 25.0f
                    });

                // Center text on equip button
                FloatRect equipTextBounds = equipButtonText->getLocalBounds();
                equipButtonText->setOrigin({ equipTextBounds.size.x / 2.0f, equipTextBounds.size.y / 2.0f });
                equipButtonText->setPosition({
                    equipButton.getPosition().x + equipButton.getSize().x / 2.0f,
                    equipButton.getPosition().y + equipButton.getSize().y / 2.0f - 3.0f
                    });

                // Position Sell button below Equip button
                sellButton.setPosition({
                    itemBounds.position.x + itemBounds.size.x + 10.0f,
                    itemBounds.position.y + (itemBounds.size.y - sellButton.getSize().y) / 2.0f + 25.0f
                    });

                // Center text on sell button
                FloatRect sellTextBounds = sellButtonText->getLocalBounds();
                sellButtonText->setOrigin({ sellTextBounds.size.x / 2.0f, sellTextBounds.size.y / 2.0f });
                sellButtonText->setPosition({
                    sellButton.getPosition().x + sellButton.getSize().x / 2.0f,
                    sellButton.getPosition().y + sellButton.getSize().y / 2.0f - 3.0f
                    });
            }
            break;
        }
    }

    // Click outside any item - deselect
    bool clickedOnItem = false;
    for (int i = 0; i < items.size(); ++i) {
        FloatRect itemBounds = getItemBounds(i);
        if (itemBounds.contains(mousePosF)) {
            clickedOnItem = true;
            break;
        }
    }

    // Also check if clicked on equip or sell button
    if (selectedItemIndex >= 0) {
        if (equipButton.getGlobalBounds().contains(mousePosF) ||
            sellButton.getGlobalBounds().contains(mousePosF)) {
            clickedOnItem = true;
        }
    }

    if (!clickedOnItem && background.getGlobalBounds().contains(mousePosF)) {
        // Clicked inside inventory but not on an item or button - deselect
        if (!closeButton.getGlobalBounds().contains(mousePosF)) {
            selectedItemIndex = -1;
            selectedItemName = "";
        }
    }

    return ""; // No equip action
}

bool InventoryUI::checkEquipButtonClick(const RenderWindow& window) {
    if (!visible || selectedItemIndex < 0 || showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return equipButton.getGlobalBounds().contains(mousePosF);
}

bool InventoryUI::checkSellButtonClick(const RenderWindow& window) {
    if (!visible || selectedItemIndex < 0 || showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return sellButton.getGlobalBounds().contains(mousePosF);
}

bool InventoryUI::checkConfirmSellClick(const RenderWindow& window) {
    if (!showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (confirmSureButton.getGlobalBounds().contains(mousePosF)) {
        showingSellConfirmation = false;
        return true; // Confirmed sell
    }
    return false;
}

bool InventoryUI::checkCancelSellClick(const RenderWindow& window) {
    if (!showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (confirmNoButton.getGlobalBounds().contains(mousePosF)) {
        showingSellConfirmation = false;
        return true; // Cancelled
    }
    return false;
}

bool InventoryUI::checkClickOutsidePopup(const RenderWindow& window) {
    if (!showingSellConfirmation) return false;

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Check if clicked outside the popup box
    if (!confirmPopupBox.getGlobalBounds().contains(mousePosF)) {
        showingSellConfirmation = false;
        return true;
    }
    return false;
}

int InventoryUI::getItemShopPrice(const string& itemName) const {
    // Get price from ItemDatabase
    return ItemDatabase::getInstance().getItemPrice(itemName);
}

void InventoryUI::clearSelection() {
    selectedItemIndex = -1;
    selectedItemName = "";
    lastClickedIndex = -1;
    showingSellConfirmation = false;
}