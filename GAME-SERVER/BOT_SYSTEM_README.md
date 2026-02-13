# Axy-GS-8.51 - Ultra-Realistic AI Bot System

## Overview
Axy-GS-8.51 is an advanced game server modification for Fortnite Season 8.51 featuring an ultra-realistic AI bot system with 350+ unique custom names and advanced behaviors.

## Bot System Features

### Core Features
- **350+ Unique Bot Names**: Each bot has a unique, memorable name for easy identification
- **10 Personality Types**: Aggressive, Defensive, Strategic, Looter, Sniper, Builder, Rusher, Camper, Balanced, and Random
- **5 Difficulty Levels**: Easy, Medium, Hard, Expert, and Realistic (human-like variation)
- **Advanced AI**: Bots make intelligent decisions based on game state, health, and situation

### Advanced Behaviors

#### 1. Intelligent Combat
- **Aim System**: Configurable aim accuracy with human-like inaccuracy
- **Target Prediction**: Predicts enemy movement for more realistic shooting
- **Combat Styles**: Shotgun rush, AR peek, snipe, spray, edit takes, box fight, high ground
- **Decision Making**: Analyzes combat situations to determine best course of action

#### 2. Fast Construction
- **Quick Box**: Instantly builds protective boxes around the bot
- **Ramp Push**: Builds ramps for aggressive pushes
- **Defensive Forts**: Constructs larger defensive structures
- **Build Types**: Walls, floors, stairs, roofs, pyramids, tunnels, crash pads
- **Speed Multiplier**: Configurable build speed based on difficulty

#### 3. Strategic Looting
- **Priority System**: Calculates loot priority based on personality and needs
- **Smart Pathing**: Efficiently moves between loot locations
- **Item Awareness**: Bots prioritize items that match their playstyle
- **Search Radius**: Configurable loot detection range

#### 4. Natural Movement
- **Smooth Rotation**: Natural-looking camera movements
- **Strafing**: Sideways movement during combat
- **Jump Trajectories**: Smart jumping for elevation changes
- **Random Variation**: Subtle randomness for more human-like movement
- **Follow Behavior**: Can follow players or rotate around points

### Bot Personalities

| Personality | Description | Traits |
|-------------|-------------|--------|
| **Aggressive** | Rushes enemies, fights often | High aggression, low flee threshold |
| **Defensive** | Builds forts, plays safe | Low aggression, high build speed |
| **Strategic** | Positions carefully, zones enemies | Balanced, better aim |
| **Looter** | Prioritizes loot over fighting | Prioritizes weapons, low engagement |
| **Sniper** | Keeps distance, aims precisely | Long range, high accuracy |
| **Builder** | Builds massive structures | Very fast building, high flee threshold |
| **Rusher** | Pushes aggressively | Very high aggression, fast reactions |
| **Camper** | Holds positions, plays passive | Very low aggression |
| **Balanced** | Mix of all traits | Average stats across the board |
| **Random** | Random behavior | Varies per spawn |

### Difficulty Settings

| Difficulty | Aim Accuracy | Reaction Time | Build Speed |
|------------|--------------|---------------|-------------|
| **Easy** | 30-50% | 0.8-1.3s | 0.5-0.8x |
| **Medium** | 50-70% | 0.5-0.8s | 0.7-1.0x |
| **Hard** | 70-90% | 0.3-0.5s | 0.9-1.0x |
| **Expert** | 85-100% | 0.15-0.3s | 1.0-1.2x |
| **Realistic** | 40-80% | 0.2-0.7s | 0.6-1.1x |

## Bot System Architecture

### Core Components

#### FBotManager
Manages all bots in the game, handles spawning, updating, and cleanup.

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

#### FBotLobbySystem
Handles lobby behaviors including dancing, walking, practice shooting, and emotes.

#### FBotBattleBusSystem
Manages battle bus behavior including jump timing and landing location selection.

## Compilation

### Prerequisites
- Visual Studio 2022 (v143 toolset)
- Windows SDK 10.0
- C++20 support

### Build Instructions

1. **Open the Solution**
   ```bash
   cd GAME-SERVER/VolcanoV2
   ```

2. **Open in Visual Studio**
   ```
   Double-click Volcano-8.51.sln
   ```

3. **Select Build Configuration**
   - Release/x64 for production
   - Debug/x64 for development

4. **Build the Solution**
   - Press F7 or go to Build → Build Solution

5. **Output**
   - The compiled DLL will be at: `x64/Release/Axy-GS-8.51.dll`

### Build Automation (MSBuild)
```bash
cd GAME-SERVER/VolcanoV2
msbuild Volcano-8.51.sln /p:Configuration=Release /p:Platform=x64
```

## Configuration

### Bot Settings in framework.h

The bot system can be configured through the `GBotManager` instance:

```cpp
// Set maximum number of bots (default: 99)
BotSystem::GBotManager->SetMaxBots(99);

// Set spawn interval in seconds (default: 5.0)
BotSystem::GBotManager->SetSpawnInterval(5.0f);

// Clear all bots
BotSystem::GBotManager->ClearBots();
```

### Game Mode Settings in framework.h

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

## Project Reboot 3.0 Compatibility

This bot system is fully compatible with Project Reboot 3.0:
- Uses same SDK structure
- Compatible with all Reboot 3.0 hooks
- Supports all Reboot 3.0 game modes
- No conflicts with Reboot 3.0 features

## Bot Names Sample

The system includes 350+ unique names across various categories:
- Gaming/Pro names (Ninja_Fan, TFue_Lite, etc.)
- Memes and Pop Culture (Thanos_Snap, GigaChad_AI, etc.)
- Cool/Gamer names (ShadowBlade_X, DarkReaper_V2, etc.)
- Skill-based names (NoScopeOnly, QuickDraw_X, etc.)
- Competitive names (FNCS_Winner, CashCup_AI, etc.)
- Mythology/Lore (Zeus_Thunder, Poseidon_Waves, etc.)
- And many more...

## Troubleshooting

### Build Errors

If you encounter build errors:

1. **Missing Includes**: Ensure all header files are in the correct directory
2. **Linker Errors**: Check that libcurl.lib and zlib.lib are in the includes folder
3. **SDK Issues**: Verify SDK.hpp is properly generated and included

### Runtime Issues

1. **Crash on injection**: Verify all hooks are properly set up
2. **Bots not visible on minimap**: Check that GameState->PlayerArray is properly updated
3. **Storm timer incorrect**: Ensure ResetStormConfiguration() is called at game start
4. **Performance issues**: Reduce max bots or increase spawn interval

## Features Checklist

### Implemented
- [x] 350+ unique bot names
- [x] 10 personality types
- [x] 5 difficulty levels
- [x] Intelligent combat system
- [x] Fast construction
- [x] Strategic looting
- [x] Natural movement
- [x] State machine for bot behavior
- [x] Decision making system
- [x] Project Reboot 3.0 compatibility

### New in This Update
- [x] **Actual Bot Spawning**: Bots now spawn as real game entities (Controller, Pawn, PlayerState)
- [x] **Lobby Visibility**: Bots appear in lobby with player counter showing correct count
- [x] **Minimap Visibility**: Bots are visible on the minimap with proper GameState registration
- [x] **Lobby Behaviors**: 
  - Dancing in lobby
  - Walking around lobby
  - Practice shooting in lobby
  - Random emotes
- [x] **Battle Bus Integration**:
  - Bots jump from battle bus at different times
  - Land at different POIs based on personality
  - Strategic landing location selection
- [x] **Storm Timer Fix**: Fixed storm timer showing 3 hours - now uses proper BR timing (5 min first zone, 3 min shrink)
- [x] **24 Named POIs**: Bots can land at Tilted Towers, Pleasant Park, Retail Row, and 21 other locations

### Future Enhancements
- [ ] Voice chat simulation
- [ ] Team coordination
- [ ] Learning AI (machine learning)
- [ ] Custom bot skins
- [ ] Bot statistics tracking

## Credits

- **Original Base**: Volcano 8.51 Game Server
- **Modified by**: Axy
- **Bot System**: Ultra-Realistic AI Implementation
- **SDK**: Dumper-7

## License

This project is provided as-is for educational purposes.

## Support

For issues or questions, please refer to the original Volcano documentation.

---

**Note**: This bot system is designed to provide an engaging single-player or practice experience. It simulates realistic player behavior without cheating or unfair advantages.
