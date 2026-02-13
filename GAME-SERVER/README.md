# Axy-GS-8.51 - Game Server with Ultra-Realistic AI Bot System

## 🎮 Project Overview

Axy-GS-8.51 is an advanced Fortnite Season 8.51 game server modification featuring a revolutionary **ultra-realistic AI bot system** with 350+ unique custom names and advanced behaviors, fully compatible with Project Reboot 3.0.

## ✨ Key Features

### 🤖 Ultra-Realistic AI Bot System

#### 350+ Unique Bot Names
Each bot gets a unique, memorable name from our curated collection including:
- Gaming/Pro names (Ninja_Fan, TFue_Lite, Cloakzy_Clone...)
- Memes and Pop Culture (Thanos_Snap, GigaChad_AI, PepeLover...)
- Cool/Gamer names (ShadowBlade_X, DarkReaper_V2, StormTrooper_X...)
- Skill-based names (NoScopeOnly, QuickDraw_X, HeadshotHunter...)
- And 300+ more unique names!

#### 10 Personality Types
Bots have distinct personalities that affect their behavior:
- **Aggressive** - Rushes, builds quickly, fights often
- **Defensive** - Builds forts, plays safe
- **Strategic** - Positions carefully, zones enemies  
- **Looter** - Prioritizes loot over fighting
- **Sniper** - Keeps distance, aims precisely
- **Builder** - Builds massive structures
- **Rusher** - Pushes aggressively
- **Camper** - Holds positions, plays passive
- **Balanced** - Mix of all traits
- **Random** - Random behavior per spawn

#### 5 Difficulty Levels
From beginner to expert:
| Difficulty | Aim Accuracy | Reaction Time | Build Speed |
|------------|--------------|---------------|-------------|
| Easy | 30-50% | 0.8-1.3s | 0.5-0.8x |
| Medium | 50-70% | 0.5-0.8s | 0.7-1.0x |
| Hard | 70-90% | 0.3-0.5s | 0.9-1.0x |
| Expert | 85-100% | 0.15-0.3s | 1.0-1.2x |
| Realistic | 40-80% | 0.2-0.7s | 0.6-1.1x |

### 🎯 Advanced Behaviors

#### Intelligent Combat
- **Aim System**: Configurable accuracy with human-like inaccuracy
- **Target Prediction**: Predicts enemy movement for realistic shooting
- **Combat Styles**: Shotgun rush, AR peek, snipe, spray, edit takes, box fight, high ground
- **Decision Making**: Analyzes situations to determine best actions

#### Fast Construction
- **Quick Box**: Instantly builds protective boxes
- **Ramp Push**: Builds ramps for aggressive pushes
- **Defensive Forts**: Constructs larger defensive structures
- **Build Types**: Walls, floors, stairs, roofs, pyramids, tunnels, crash pads
- **Speed Multiplier**: Configurable based on difficulty

#### Strategic Looting
- **Priority System**: Calculates loot priority based on personality
- **Smart Pathing**: Efficient movement between loot locations
- **Item Awareness**: Prioritizes items matching playstyle
- **Search Radius**: Configurable loot detection range

#### Natural Movement
- **Smooth Rotation**: Natural-looking camera movements
- **Strafing**: Sideways movement during combat
- **Jump Trajectories**: Smart jumping for elevation
- **Random Variation**: Subtle randomness for human-like motion
- **Follow Behavior**: Can follow players or rotate around points

## 🏗️ Project Structure

```
GAME-SERVER/
├── VolcanoV2/
│   └── Volcano-8.51/
│       ├── BotSystem.h              # Core bot system with 350+ names
│       ├── BotAdvancedBehaviors.h    # Advanced AI behaviors
│       ├── framework.h              # Main framework and hooks
│       ├── dllmain.cpp              # DLL entry point
│       ├── PC.h                    # Player controller hooks
│       ├── Inventory.h              # Inventory system
│       ├── sdk/                    # SDK files
│       └── ...
├── BOT_SYSTEM_README.md            # Detailed bot system documentation
└── README.md                      # This file
```

## 🔧 Compilation

### Prerequisites
- **Visual Studio 2022** (v143 toolset)
- **Windows SDK 10.0**
- **C++20** support

### Quick Build (Windows)
```bash
# Navigate to project directory
cd GAME-SERVER/VolcanoV2

# Open Visual Studio solution
# Double-click Volcano-8.51.sln

# Select Release/x64 configuration
# Press F7 to build

# Output: x64/Release/Axy-GS-8.51.dll
```

### MSBuild Command Line
```bash
cd GAME-SERVER/VolcanoV2
msbuild Volcano-8.51.sln /p:Configuration=Release /p:Platform=x64
```

## ⚙️ Configuration

### Bot System Settings
Configure bots in your code:

```cpp
// Set maximum number of bots (default: 99)
BotSystem::GBotManager->SetMaxBots(99);

// Set spawn interval in seconds (default: 5.0)
BotSystem::GBotManager->SetSpawnInterval(5.0f);

// Clear all bots
BotSystem::GBotManager->ClearBots();
```

### Game Mode Settings
Configure in `framework.h`:

```cpp
namespace Globals
{
    bool bLategame = false;      // Enable lategame mode
    bool bPlayground = true;     // Enable playground mode
    bool bSolo = false;          // Enable solo mode
    bool bOnShotLTM = false;     // Enable One Shot LTM
    bool bFloorIsLava = false;   // Enable Floor Is Lava LTM
}
```

## 🎮 Usage

### Injection
1. Start Project Reboot 3.0 launcher
2. Inject `Axy-GS-8.51.dll` into the game process
3. Bots will spawn automatically based on configuration

### Console Commands
The server provides console output showing:
- Bot spawning events with names and personality
- Bot state changes
- Bot behavior updates

## 🔄 Project Reboot 3.0 Compatibility

This system is **fully compatible** with Project Reboot 3.0:
- ✅ Same SDK structure
- ✅ Compatible with all Reboot 3.0 hooks
- ✅ Supports all Reboot 3.0 game modes
- ✅ No conflicts with Reboot 3.0 features
- ✅ Seamless integration

## 📊 Bot System Architecture

### Core Components

#### FBotManager
Manages all bots in game, handles spawning, updating, and cleanup.

#### FBotPlayer  
Represents individual bot instances with configuration and state.

#### FBotBuildingSystem
Handles all construction-related bot behaviors.

#### FBotCombatSystem
Manages combat logic, aiming, and fighting decisions.

#### FBotLootingSystem
Controls loot detection, prioritization, and collection.

#### FBotMovementSystem
Handles movement, pathing, and natural motion.

#### FBotDecisionSystem
Makes strategic decisions based on game state and situation.

## 🐛 Troubleshooting

### Build Errors

**Missing Includes**
- Ensure all header files are in the correct directory
- Check `BotSystem.h` and `BotAdvancedBehaviors.h` are in `Volcano-2/Volcano-8.51/`

**Linker Errors**
- Verify `libcurl.lib` and `zlib.lib` are in the includes folder
- Check library paths in project settings

**SDK Issues**
- Verify `SDK.hpp` is properly generated and included
- Ensure SDK classes are correctly defined

### Runtime Issues

**Bots not spawning**
- Check that `InitializeBotSystem()` is called in `Main()` in dllmain.cpp
- Verify bot system update is called in `TickFlushHook`

**Crash on injection**
- Verify all hooks are properly set up
- Check SDK offsets match your game version

**Performance issues**
- Reduce max bots: `SetMaxBots(50)`
- Increase spawn interval: `SetSpawnInterval(10.0f)`

## 📝 Features Checklist

### ✅ Implemented
- [x] 350+ unique bot names
- [x] 10 personality types  
- [x] 5 difficulty levels
- [x] Intelligent combat system
- [x] Fast construction
- [x] Strategic looting
- [x] Natural movement
- [x] State machine for behavior
- [x] Decision making system
- [x] Project Reboot 3.0 compatibility
- [x] Configurable settings

### 🚀 Future Enhancements
- [ ] Voice chat simulation
- [ ] Emote usage
- [ ] Team coordination
- [ ] Learning AI (machine learning)
- [ ] Custom bot skins
- [ ] Bot statistics tracking

## 👥 Credits

- **Original Base**: Volcano 8.51 Game Server
- **Modified by**: Axy
- **Bot System**: Ultra-Realistic AI Implementation
- **SDK**: Dumper-7

## 📄 License

This project is provided as-is for educational purposes.

## 💡 Notes

- The bot system simulates realistic player behavior without cheating
- Bots are designed to provide engaging practice experience
- All behaviors are configurable for different skill levels
- System is optimized for performance with up to 99 bots

---

**Ready to compile your Axy-GS-8.51.dll?** Check out [BOT_SYSTEM_README.md](BOT_SYSTEM_README.md) for detailed technical documentation!

**Happy coding! 🎮**
