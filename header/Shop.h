#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>

using namespace std;
using namespace sf;

enum class ItemRarity {
    Common,
    Rare,
    Mythic,
    Legendary
};

// Forward declaration
class Inventory;
class CurrencyManager;

struct ShopItem {
    string name;
    string description;
    ItemRarity rarity;
    int price;  // Price in silver
    string iconPath;  // Path to item icon texture
    bool isPurchased;  // Track if item was bought this refresh
    int classIndex;  // -1 = all classes, 0 = Assassin, 1 = Bruiser, 2 = Mage
};

// Structure to save/load shop state per character
struct ShopSaveData {
    vector<string> itemNames;           // Names of items in shop
    vector<int> itemRarities;           // Rarity as int (for serialization)
    vector<int> itemPrices;             // Prices
    vector<bool> itemPurchased;         // Purchase status
    vector<int> itemClassIndices;       // Class restrictions
    string lastResetDate;               // Last reset date for this character's shop

    ShopSaveData() : lastResetDate("") {}
};

struct ShopItemVisual {
    RectangleShape itemBox;
    float x;  // Store position for drawing text later
    float y;
};

class Shop {
public:
    Shop(float width, float height);
    ~Shop();

    void draw(RenderWindow& window);
    void update();
    bool checkItemClick(const RenderWindow& window, CurrencyManager& currency, Inventory* inventory);
    bool checkRefreshClick(const RenderWindow& window, CurrencyManager& currency);
    bool checkCloseClick(const RenderWindow& window);
    void checkHover(const RenderWindow& window);

    void show();
    void hide();
    bool isVisible() const { return visible; }

    void forceRefresh();  // For manual refresh (costs 1 gold)
    void setPlayerClass(int classIndex) { playerClassIndex = classIndex; }
    bool checkCheatClick(const RenderWindow& window, CurrencyManager& currency);

    // Save and load shop state for per-character shops
    void saveState(ShopSaveData& saveData) const;
    void loadState(const ShopSaveData& saveData);
    bool hasValidSavedState(const ShopSaveData& saveData) const;

private:
    void initializeItemPool();
    void generateShopItems();
    ItemRarity rollRarity();
    void createItemVisuals();
    void checkAutoRefresh();  // Check if 24 hours passed

    // Helper to find item in pool by name
    ShopItem* findItemInPool(const string& name);

    bool visible;
    float windowWidth;
    float windowHeight;

    Font font;

    // Shop UI elements
    RectangleShape shopBackground;
    Text* titleText;
    RectangleShape refreshButton;
    Text* refreshButtonText;
    Text* refreshCostText;
    Text* timeUntilRefreshText;

    // Item pool (all possible items in the game)
    vector<ShopItem> itemPool;

    // Current shop items (6 items)
    vector<ShopItem> currentItems;

    // Visual representations of current items
    vector<ShopItemVisual> itemVisuals;

    // Refresh tracking
    time_t lastRefreshTime;
    const int RESET_HOUR = 8;

    sf::RectangleShape refillButton;
    sf::Text* refillButtonText;
    bool isRefillButtonHovered;

    std::string lastResetDate;  // Format: "YYYY-MM-DD"

    // Get current date as string
    std::string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        char buffer[11];
        sprintf(buffer, "%04d-%02d-%02d",
            1900 + ltm->tm_year,
            1 + ltm->tm_mon,
            ltm->tm_mday);
        return std::string(buffer);
    }

    // Check if it's past reset time (8 AM) today
    bool isPastResetTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        return ltm->tm_hour >= RESET_HOUR;
    }

    // Check if we should reset 
    bool shouldReset() {
        std::string today = getCurrentDate();

        // If last reset was yesterday or earlier, and it's past 8 AM
        if (lastResetDate != today && isPastResetTime()) {
            return true;
        }

        return false;
    }

    // Rarity chances
    static constexpr float COMMON_CHANCE = 0.65f;
    static constexpr float RARE_CHANCE = 0.25f;
    static constexpr float MYTHIC_CHANCE = 0.09f;
    static constexpr float LEGENDARY_CHANCE = 0.01f;

    // UI Constants
    static const int ITEMS_PER_ROW = 3;
    static const int MAX_SHOP_ITEMS = 6;

    // Player class filtering
    int playerClassIndex;

    // Tooltip for hover
    RectangleShape tooltipBox;
    Text tooltipText{ font };
    int hoveredItemIndex;

    // Cheat button
    RectangleShape cheatButton;
    Text cheatButtonText{ font };
};