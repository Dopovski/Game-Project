# Horde and Entente

**Horde and Entente** is a turn-based 2D RPG built in C++ and SFML where you fight monsters in dungeons, earn currency, and purchase items to upgrade your stats and take on stronger enemies.

---

## Screenshots

| Menu | Combat |
|------|--------|
| ![Menu](Screenshot 2026-04-17 230834.png) | ![Combat](Screenshot 2026-04-17 230848 - Copy.png) |

| Shop | Arena |
|------|-------|
| ![Shop](Screenshot 2026-04-17 230859.png) | ![Arena](Screenshot 2026-04-17 230925.png) |

---

## Features
- Turn-based combat system with adjustable speed (1x, 2x, 3x)
- Dungeon and Arena modes with unique enemies
- Shop with randomized items, rarity tiers, and auto-refresh
- Brewer, Rider, Fortress, Curse, and Guild systems
- Full progression loop — XP, leveling, Silver, Gold, and items
- Fatigue system and torch management
- Save system

---

## Architecture

The project is built around dedicated manager classes, each responsible for a distinct part of the game:

| Manager | Responsibility |
|---------|----------------|
| `GameStateManager` | Controls game flow and state transitions |
| `InputManager` | Handles all user input and event processing |
| `SystemsManager` | Coordinates core game systems |
| `ResourceManager` | Manages loading and access to assets |
| `CurrencyManager` | Tracks Silver, Gold, and economy logic |
| `SaveSystem` | Handles saving and loading game state |

---

## How to Build

**Requirements:**
- C++20
- SFML 3.0 — download from [sfml-dev.org](https://sfml-dev.org)

Link SFML to the project and compile with a C++20 compatible compiler.

---

## Source Structure
```
src/        — implementation files
header/     — header files
```
Organized into subfolders: `characters`, `combat`, `core`, `dialogs`, `inventory`, `menus`, `missions`, `shop`, `ui`
