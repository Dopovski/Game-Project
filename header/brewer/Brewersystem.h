#pragma once
#pragma warning(disable : 4996)
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <iostream>
#include "PotionData.h"
#include "CharacterData.h"

using namespace std;
using namespace sf;

class BrewerSystem {
private:
    Font font;
    bool visible;
    bool isLocked;
    time_t lastPurchaseTime;
    time_t lockResetTime;

    vector<Potion> availablePotions;

    RectangleShape background;
    Text* titleText;
    Text* lockText;

    const Color BUTTON_COLOR_DEFAULT = Color(0, 0, 100, 180);
    const Color BUTTON_COLOR_HOVER = Color(0, 50, 200, 255);
    const Color OUTLINE_COLOR = Color::Black;

    struct PotionSlot {
        RectangleShape background;
        Text* name;
        Text* description;
        Text* cost;
        RectangleShape buyButton;
        Text* buyButtonText;
    };
    vector<PotionSlot> potionSlots;

    RectangleShape refreshButton;
    Text* refreshButtonText;

    RectangleShape skipButton;
    Text* skipButtonText;

    int hoveredSlot;
    int hoveredRefresh;
    int hoveredSkip;

    float windowWidth;
    float windowHeight;

    mt19937 rng;

    void setupUI() {
        float bgWidth = 1600.f;
        float bgHeight = 1080.f;
        float bgX = windowWidth - bgWidth;
        float bgY = (windowHeight - bgHeight) / 2.f;

        background.setSize({ bgWidth, bgHeight });
        background.setPosition({ bgX, bgY });
        background.setFillColor(Color(20, 15, 10));

        titleText = new Text(font);
        titleText->setFont(font);
        titleText->setString("The Brewer");
        titleText->setCharacterSize(60);
        titleText->setFillColor(Color(255, 215, 0));
        FloatRect titleBounds = titleText->getLocalBounds();
        titleText->setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
        titleText->setPosition({ bgX + bgWidth / 2.f, bgY + 80.f });

        lockText = new Text(font);
        lockText->setFont(font);
        lockText->setCharacterSize(32);
        lockText->setFillColor(Color::Red);

        const float SLOT_WIDTH = 500.f;
        const float SLOT_HEIGHT = 350.f;
        const float SLOT_SPACING = 550.f;
        const float TOTAL_WIDTH = SLOT_WIDTH * 2 + (SLOT_SPACING - SLOT_WIDTH);
        float startX = bgX + (bgWidth - TOTAL_WIDTH) / 2.f;
        float slotY = bgY + 200.f;

        for (int i = 0; i < 2; i++) {
            PotionSlot slot;

            slot.background.setSize({ SLOT_WIDTH, SLOT_HEIGHT });
            slot.background.setPosition({ startX + i * SLOT_SPACING, slotY });
            slot.background.setFillColor(Color(60, 50, 40));
            slot.background.setOutlineThickness(4.f);
            slot.background.setOutlineColor(Color(100, 80, 60));

            slot.name = new Text(font);
            slot.name->setFont(font);
            slot.name->setCharacterSize(32);
            slot.name->setFillColor(Color::White);
            slot.name->setPosition({ startX + i * SLOT_SPACING + 20.f, slotY + 20.f });

            slot.description = new Text(font);
            slot.description->setFont(font);
            slot.description->setCharacterSize(24);
            slot.description->setFillColor(Color(200, 200, 200));
            slot.description->setPosition({ startX + i * SLOT_SPACING + 20.f, slotY + 80.f });

            slot.cost = new Text(font);
            slot.cost->setFont(font);
            slot.cost->setCharacterSize(28);
            slot.cost->setFillColor(Color(255, 215, 0));
            slot.cost->setPosition({ startX + i * SLOT_SPACING + 20.f, slotY + 150.f });

            slot.buyButton.setSize({ 460.f, 70.f });
            slot.buyButton.setPosition({ startX + i * SLOT_SPACING + 20.f, slotY + 250.f });
            slot.buyButton.setFillColor(BUTTON_COLOR_DEFAULT);
            slot.buyButton.setOutlineThickness(3.f);
            slot.buyButton.setOutlineColor(OUTLINE_COLOR);

            slot.buyButtonText = new Text(font);
            slot.buyButtonText->setFont(font);
            slot.buyButtonText->setString("Buy Potion");
            slot.buyButtonText->setCharacterSize(36);
            slot.buyButtonText->setFillColor(Color::White);
            FloatRect buyBounds = slot.buyButtonText->getLocalBounds();
            slot.buyButtonText->setOrigin({ buyBounds.size.x / 2.f, buyBounds.size.y / 2.f });
            slot.buyButtonText->setPosition({ startX + i * SLOT_SPACING + 250.f, slotY + 280.f });

            potionSlots.push_back(slot);
        }

        refreshButton.setSize({ 400.f, 70.f });
        refreshButton.setPosition({ bgX + bgWidth / 2.f - 200.f, bgY + 620.f });
        refreshButton.setFillColor(BUTTON_COLOR_DEFAULT);
        refreshButton.setOutlineThickness(3.f);
        refreshButton.setOutlineColor(OUTLINE_COLOR);

        refreshButtonText = new Text(font);
        refreshButtonText->setFont(font);
        refreshButtonText->setString("Refresh (1 Gold)");
        refreshButtonText->setCharacterSize(36);
        refreshButtonText->setFillColor(Color::White);
        FloatRect refreshBounds = refreshButtonText->getLocalBounds();
        refreshButtonText->setOrigin({ refreshBounds.size.x / 2.f, refreshBounds.size.y / 2.f });
        refreshButtonText->setPosition({ bgX + bgWidth / 2.f, bgY + 652.f });

        skipButton.setSize({ 450.f, 70.f });
        skipButton.setPosition({ bgX + bgWidth / 2.f - 225.f, bgY + 750.f });
        skipButton.setFillColor(Color(180, 140, 0, 180));
        skipButton.setOutlineThickness(3.f);
        skipButton.setOutlineColor(OUTLINE_COLOR);

        skipButtonText = new Text(font);
        skipButtonText->setFont(font);
        skipButtonText->setString("Skip Timer (1 Gold)");
        skipButtonText->setCharacterSize(36);
        skipButtonText->setFillColor(Color::White);
        FloatRect skipBounds = skipButtonText->getLocalBounds();
        skipButtonText->setOrigin({ skipBounds.size.x / 2.f, skipBounds.size.y / 2.f });
        skipButtonText->setPosition({ bgX + bgWidth / 2.f, bgY + 782.f });

        hoveredSlot = -1;
        hoveredRefresh = -1;
        hoveredSkip = -1;
    }

    Potion generateRandomPotion() {
        uniform_real_distribution<double> rarityRoll(0.0, 100.0);
        double roll = rarityRoll(rng);

        PotionRarity rarity;
        if (roll < 5.0) {
            return Potion(PotionType::Speed, PotionRarity::Legendary);
        }
        else if (roll < 20.0) {
            rarity = PotionRarity::Mythical;
        }
        else if (roll < 50.0) {
            rarity = PotionRarity::Rare;
        }
        else {
            rarity = PotionRarity::Common;
        }

        uniform_int_distribution<int> typeDist(0, 4);
        PotionType type = static_cast<PotionType>(typeDist(rng));

        return Potion(type, rarity);
    }

    void refreshPotions() {
        availablePotions.clear();
        availablePotions.push_back(generateRandomPotion());
        availablePotions.push_back(generateRandomPotion());
        updatePotionDisplay();
    }

    void updatePotionDisplay() {
        for (size_t i = 0; i < availablePotions.size() && i < 2; i++) {
            const Potion& potion = availablePotions[i];

            potionSlots[i].name->setString(potion.name);
            potionSlots[i].name->setFillColor(potion.getRarityColor());

            string desc = potion.description;
            desc += "\nLasts " + to_string(potion.durationDays) + " days";
            potionSlots[i].description->setString(desc);

            potionSlots[i].cost->setString("Cost: " + to_string(potion.cost) + " Gold");

            potionSlots[i].background.setOutlineColor(potion.getRarityColor());
        }
    }

    bool shouldResetLock() const {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        if (ltm->tm_hour >= 8 && now >= lockResetTime) {
            return true;
        }
        return false;
    }

    void updateLockResetTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        tm nextReset = *ltm;
        nextReset.tm_hour = 8;
        nextReset.tm_min = 0;
        nextReset.tm_sec = 0;

        if (ltm->tm_hour >= 8) {
            nextReset.tm_mday += 1;
        }

        lockResetTime = mktime(&nextReset);
    }

    string getTimeUntilReset() const {
        time_t now = time(0);
        double secondsRemaining = difftime(lockResetTime, now);

        if (secondsRemaining <= 0) return "Ready";

        int hours = static_cast<int>(secondsRemaining / 3600);
        int minutes = static_cast<int>((secondsRemaining - hours * 3600) / 60);

        return to_string(hours) + "h " + to_string(minutes) + "m";
    }

public:
    BrewerSystem(float winWidth, float winHeight)
        : windowWidth(winWidth), windowHeight(winHeight),
        visible(false), isLocked(false),
        lastPurchaseTime(0), lockResetTime(0),
        rng(static_cast<unsigned int>(time(0))),
        titleText(nullptr), lockText(nullptr),
        refreshButtonText(nullptr), skipButtonText(nullptr),
        hoveredSlot(-1), hoveredRefresh(-1), hoveredSkip(-1) {

        if (!font.openFromFile("arial.ttf")) {
            cerr << "Error loading font for Brewer!" << endl;
        }

        setupUI();
        refreshPotions();
        updateLockResetTime();
    }

    ~BrewerSystem() {
        delete titleText;
        delete lockText;
        delete refreshButtonText;
        delete skipButtonText;

        for (auto& slot : potionSlots) {
            delete slot.name;
            delete slot.description;
            delete slot.cost;
            delete slot.buyButtonText;
        }
    }

    void show() {
        if (isLocked && shouldResetLock()) {
            isLocked = false;
            refreshPotions();
            updateLockResetTime();
        }
        visible = true;
    }

    void hide() {
        visible = false;
    }

    bool isVisible() const {
        return visible;
    }

    void checkMousePosition(const RenderWindow& window) {
        if (!visible) return;

        Vector2i mousePos = Mouse::getPosition(window);

        hoveredSlot = -1;
        hoveredRefresh = -1;
        hoveredSkip = -1;

        if (!isLocked) {
            for (int i = 0; i < 2; i++) {
                if (potionSlots[i].buyButton.getGlobalBounds().contains({
                    static_cast<float>(mousePos.x),
                    static_cast<float>(mousePos.y)
                    })) {
                    hoveredSlot = i;
                    break;
                }
            }

            if (refreshButton.getGlobalBounds().contains({
                static_cast<float>(mousePos.x),
                static_cast<float>(mousePos.y)
                })) {
                hoveredRefresh = 0;
            }
        }
        else {
            if (skipButton.getGlobalBounds().contains({
                static_cast<float>(mousePos.x),
                static_cast<float>(mousePos.y)
                })) {
                hoveredSkip = 0;
            }
        }

        if (!isLocked) {
            for (int i = 0; i < 2; i++) {
                if (hoveredSlot == i) {
                    potionSlots[i].buyButton.setFillColor(BUTTON_COLOR_HOVER);
                }
                else {
                    potionSlots[i].buyButton.setFillColor(BUTTON_COLOR_DEFAULT);
                }
            }

            if (hoveredRefresh == 0) {
                refreshButton.setFillColor(BUTTON_COLOR_HOVER);
            }
            else {
                refreshButton.setFillColor(BUTTON_COLOR_DEFAULT);
            }
        }
        else {
            if (hoveredSkip == 0) {
                skipButton.setFillColor(Color(220, 180, 0, 255));
            }
            else {
                skipButton.setFillColor(Color(180, 140, 0, 180));
            }
        }
    }

    int checkBuyButtonClick(const RenderWindow& window, int& gold) {
        if (!visible || isLocked) return -1;

        Vector2i mousePos = Mouse::getPosition(window);

        for (int i = 0; i < 2; i++) {
            if (potionSlots[i].buyButton.getGlobalBounds().contains({
                static_cast<float>(mousePos.x),
                static_cast<float>(mousePos.y)
                })) {
                if (gold >= availablePotions[i].cost) {
                    gold -= availablePotions[i].cost;
                    isLocked = true;
                    return i;
                }
                else {
                    cout << "Not enough gold!" << endl;
                }
            }
        }

        return -1;
    }

    bool checkRefreshButtonClick(const RenderWindow& window, int& gold) {
        if (!visible || isLocked) return false;

        Vector2i mousePos = Mouse::getPosition(window);

        if (refreshButton.getGlobalBounds().contains({
            static_cast<float>(mousePos.x),
            static_cast<float>(mousePos.y)
            })) {
            if (gold >= 1) {
                gold -= 1;
                refreshPotions();
                cout << "Potions refreshed!" << endl;
                return true;
            }
            else {
                cout << "Not enough gold to refresh!" << endl;
            }
        }

        return false;
    }

    bool checkSkipButtonClick(const RenderWindow& window, int& gold) {
        if (!visible || !isLocked) return false;

        Vector2i mousePos = Mouse::getPosition(window);

        if (skipButton.getGlobalBounds().contains({
            static_cast<float>(mousePos.x),
            static_cast<float>(mousePos.y)
            })) {
            if (gold >= 1) {
                gold -= 1;
                isLocked = false;
                refreshPotions();
                updateLockResetTime();
                return true;
            }
            else {
                cout << "Not enough gold to skip timer!" << endl;
            }
        }

        return false;
    }

    bool checkCloseButtonClick(const RenderWindow& window) {
        return false;
    }

    Potion getPotionAtIndex(int index) const {
        if (index >= 0 && index < availablePotions.size()) {
            Potion potion = availablePotions[index];
            potion.purchaseTime = time(0);
            return potion;
        }
        return Potion();
    }

    void draw(RenderWindow& window) {
        if (!visible) return;

        window.draw(background);

        if (titleText) window.draw(*titleText);

        if (isLocked) {
            if (lockText) {
                float bgWidth = 1600.f;
                float bgHeight = 1080.f;
                float bgX = windowWidth - bgWidth;
                float bgY = (windowHeight - bgHeight) / 2.f;

                lockText->setString("Brewer locked until 8:00 AM tomorrow");
                FloatRect lockBounds = lockText->getLocalBounds();
                lockText->setOrigin({ lockBounds.size.x / 2.f, lockBounds.size.y / 2.f });
                lockText->setPosition({ bgX + bgWidth / 2.f, bgY + 350.f });
                window.draw(*lockText);
            }

            window.draw(skipButton);
            if (skipButtonText) window.draw(*skipButtonText);
        }
        else {
            for (size_t i = 0; i < potionSlots.size(); i++) {
                window.draw(potionSlots[i].background);
                if (potionSlots[i].name) window.draw(*potionSlots[i].name);
                if (potionSlots[i].description) window.draw(*potionSlots[i].description);
                if (potionSlots[i].cost) window.draw(*potionSlots[i].cost);
                window.draw(potionSlots[i].buyButton);
                if (potionSlots[i].buyButtonText) window.draw(*potionSlots[i].buyButtonText);
            }

            window.draw(refreshButton);
            if (refreshButtonText) window.draw(*refreshButtonText);
        }
    }

    // Per-character state save/load
    void saveState(BrewerStateData& state) const {
        state.isLocked = isLocked;
        state.lastPurchaseTime = static_cast<long long>(lastPurchaseTime);
        state.lockResetTime = static_cast<long long>(lockResetTime);
    }

    void loadState(const BrewerStateData& state) {
        isLocked = state.isLocked;
        lastPurchaseTime = static_cast<time_t>(state.lastPurchaseTime);
        lockResetTime = static_cast<time_t>(state.lockResetTime);

        if (isLocked && shouldResetLock()) {
            isLocked = false;
            refreshPotions();
            updateLockResetTime();
        }
    }
};