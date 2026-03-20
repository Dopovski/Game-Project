#pragma once
#include <string>
#include <fstream>
#include <map>
#include <iostream>

// Simple save system that works without CharacterSheet dependency
class SaveSystem {
public:
    struct PlayerData {
        // Stats
        int level = 1;
        int experience = 0;
        int health = 100;
        int attack = 10;
        int armor = 5;
        int dexterity = 10;
        int intelligence = 10;
        int luck = 10;

        // Currency
        int silver = 100;
        int gold = 0;
        int torches = 3;

        // Equipped items (store as strings)
        std::string weapon = "";
        std::string helmet = "";
        std::string chestplate = "";
        std::string pants = "";
        std::string boots = "";
        std::string talisman = "";
        std::string companion = "";

        // Race and class
        std::string race = "";
        std::string className = "";
    };

    // Save to file
    static bool save(const PlayerData& data, const std::string& filename = "savegame.dat") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[SAVE] Failed to create save file!" << std::endl;
            return false;
        }

        // Write all data
        file << "LEVEL:" << data.level << "\n";
        file << "XP:" << data.experience << "\n";
        file << "HEALTH:" << data.health << "\n";
        file << "ATTACK:" << data.attack << "\n";
        file << "ARMOR:" << data.armor << "\n";
        file << "DEXTERITY:" << data.dexterity << "\n";
        file << "INTELLIGENCE:" << data.intelligence << "\n";
        file << "LUCK:" << data.luck << "\n";
        file << "SILVER:" << data.silver << "\n";
        file << "GOLD:" << data.gold << "\n";
        file << "TORCHES:" << data.torches << "\n";
        file << "WEAPON:" << data.weapon << "\n";
        file << "HELMET:" << data.helmet << "\n";
        file << "CHESTPLATE:" << data.chestplate << "\n";
        file << "PANTS:" << data.pants << "\n";
        file << "BOOTS:" << data.boots << "\n";
        file << "TALISMAN:" << data.talisman << "\n";
        file << "COMPANION:" << data.companion << "\n";
        file << "RACE:" << data.race << "\n";
        file << "CLASS:" << data.className << "\n";

        file.close();
        std::cout << "[SAVE] Game saved successfully to " << filename << std::endl;
        return true;
    }

    // Load from file
    static bool load(PlayerData& data, const std::string& filename = "savegame.dat") {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[LOAD] No save file found" << std::endl;
            return false;
        }

        std::string line;
        std::map<std::string, std::string> savedData;

        // Read all lines
        while (std::getline(file, line)) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                savedData[key] = value;
            }
        }
        file.close();

        // Apply loaded data
        if (savedData.count("LEVEL")) data.level = std::stoi(savedData["LEVEL"]);
        if (savedData.count("XP")) data.experience = std::stoi(savedData["XP"]);
        if (savedData.count("HEALTH")) data.health = std::stoi(savedData["HEALTH"]);
        if (savedData.count("ATTACK")) data.attack = std::stoi(savedData["ATTACK"]);
        if (savedData.count("ARMOR")) data.armor = std::stoi(savedData["ARMOR"]);
        if (savedData.count("DEXTERITY")) data.dexterity = std::stoi(savedData["DEXTERITY"]);
        if (savedData.count("INTELLIGENCE")) data.intelligence = std::stoi(savedData["INTELLIGENCE"]);
        if (savedData.count("LUCK")) data.luck = std::stoi(savedData["LUCK"]);
        if (savedData.count("SILVER")) data.silver = std::stoi(savedData["SILVER"]);
        if (savedData.count("GOLD")) data.gold = std::stoi(savedData["GOLD"]);
        if (savedData.count("TORCHES")) data.torches = std::stoi(savedData["TORCHES"]);
        if (savedData.count("WEAPON")) data.weapon = savedData["WEAPON"];
        if (savedData.count("HELMET")) data.helmet = savedData["HELMET"];
        if (savedData.count("CHESTPLATE")) data.chestplate = savedData["CHESTPLATE"];
        if (savedData.count("PANTS")) data.pants = savedData["PANTS"];
        if (savedData.count("BOOTS")) data.boots = savedData["BOOTS"];
        if (savedData.count("TALISMAN")) data.talisman = savedData["TALISMAN"];
        if (savedData.count("COMPANION")) data.companion = savedData["COMPANION"];
        if (savedData.count("RACE")) data.race = savedData["RACE"];
        if (savedData.count("CLASS")) data.className = savedData["CLASS"];

        std::cout << "[LOAD] Game loaded successfully from " << filename << std::endl;
        std::cout << "  Level: " << data.level << ", Silver: " << data.silver << std::endl;
        return true;
    }

    // Check if save exists
    static bool exists(const std::string& filename = "savegame.dat") {
        std::ifstream file(filename);
        return file.good();
    }

    // Delete save file
    static bool deleteSave(const std::string& filename = "savegame.dat") {
        if (std::remove(filename.c_str()) == 0) {
            std::cout << "[SAVE] Save file deleted" << std::endl;
            return true;
        }
        return false;
    }
};