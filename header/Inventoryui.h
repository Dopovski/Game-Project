#pragma once
#include <SFML/Graphics.hpp>
#include "Inventory.h"
#include <vector>
#include <string>

using namespace std;
using namespace sf;

class InventoryUI {
public:
    InventoryUI(float windowWidth, float windowHeight);
    ~InventoryUI();

    void draw(RenderWindow& window, Inventory* inventory);
    void show();
    void hide();
    bool isVisible() const { return visible; }

    // Check if the close button was clicked
    bool checkCloseClick(const RenderWindow& window);

    // Check if an item was clicked (returns index, -1 if none)
    int checkItemClick(const RenderWindow& window);

    // Check if an item was double-clicked (returns item name, empty if none)
    string checkItemDoubleClick(const RenderWindow& window, Inventory* inventory);

    // Check if Equip button was clicked (returns true if clicked)
    bool checkEquipButtonClick(const RenderWindow& window);

    // Check if Sell button was clicked (returns true if clicked)
    bool checkSellButtonClick(const RenderWindow& window);

    // Get the currently selected item name
    string getSelectedItemName() const { return selectedItemName; }

    // Get the currently selected item index
    int getSelectedItemIndex() const { return selectedItemIndex; }

    // Clear selection (call after equipping)
    void clearSelection();

    void checkHover(const RenderWindow& window);

    // Confirmation popup methods
    bool isShowingConfirmation() const { return showingSellConfirmation; }
    bool checkConfirmSellClick(const RenderWindow& window);  // Returns true if "Sure" clicked
    bool checkCancelSellClick(const RenderWindow& window);   // Returns true if "No" clicked
    bool checkClickOutsidePopup(const RenderWindow& window); // Returns true if clicked outside popup
    int getSellPrice() const { return pendingSellPrice; }

private:
    bool visible;
    float windowWidth;
    float windowHeight;

    Font font;

    // UI Elements
    RectangleShape background;
    Text* titleText;
    RectangleShape closeButton;
    Text* closeButtonText;

    // Item display area
    float itemStartX;
    float itemStartY;
    float itemWidth;
    float itemHeight;
    float itemSpacing;
    int itemsPerRow;

    // Helper to get item box bounds
    FloatRect getItemBounds(int index) const;

    // Helper to get item shop price
    int getItemShopPrice(const string& itemName) const;

    // Double-click tracking
    Clock doubleClickClock;
    int lastClickedIndex;
    float doubleClickTime;

    // Selected item tracking
    int selectedItemIndex;
    string selectedItemName;

    // Equip and Sell buttons
    RectangleShape equipButton;
    Text* equipButtonText;
    RectangleShape sellButton;
    Text* sellButtonText;

    // Sell confirmation popup
    bool showingSellConfirmation;
    RectangleShape confirmPopupBackground;
    RectangleShape confirmPopupBox;
    Text* confirmPopupText;
    RectangleShape confirmSureButton;
    Text* confirmSureText;
    RectangleShape confirmNoButton;
    Text* confirmNoText;
    int pendingSellPrice;
};