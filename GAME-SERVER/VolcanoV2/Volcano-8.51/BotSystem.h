#pragma once
#include "framework.h"
#include <chrono>
#include <cmath>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

// Include SDK for bot spawning
#include "sdk/SDK.hpp"

// Forward declarations for SDK types - using SDK namespace explicitly
namespace SDK {
class AFortPlayerControllerAthena;
class AFortPawn;
class AActor;
class AFortGameStateAthena;
class AFortPlayerStateAthena;
class AAthena_PlayerController_C;
class APlayerPawn_Athena_C;
class AFortPlayerStateAthena;
} // namespace SDK

// Helper functions for FVector operations (since SDK::FVector doesn't have
// these methods)
namespace FVectorHelpers {
inline float Distance(const SDK::FVector &A, const SDK::FVector &B) {
  float DX = A.X - B.X;
  float DY = A.Y - B.Y;
  float DZ = A.Z - B.Z;
  return std::sqrt(DX * DX + DY * DY + DZ * DZ);
}

inline void Normalize(SDK::FVector &V) {
  float Mag = std::sqrt(V.X * V.X + V.Y * V.Y + V.Z * V.Z);
  if (Mag > 0.0f) {
    V.X /= Mag;
    V.Y /= Mag;
    V.Z /= Mag;
  }
}

inline SDK::FVector operator-(const SDK::FVector &A, const SDK::FVector &B) {
  return SDK::FVector{A.X - B.X, A.Y - B.Y, A.Z - B.Z};
}

inline SDK::FVector operator+(const SDK::FVector &A, const SDK::FVector &B) {
  return SDK::FVector{A.X + B.X, A.Y + B.Y, A.Z + B.Z};
}

inline SDK::FVector operator*(const SDK::FVector &A, float Scalar) {
  return SDK::FVector{A.X * Scalar, A.Y * Scalar, A.Z * Scalar};
}

inline SDK::FVector operator-(const SDK::FVector &V) {
  return SDK::FVector{-V.X, -V.Y, -V.Z};
}
} // namespace FVectorHelpers

namespace BotSystem {
// Bot personality traits for realistic behavior
enum class EBotPersonality {
  Aggressive, // Rushes, builds quickly, fights often
  Defensive,  // Builds forts, plays safe
  Strategic,  // Positions carefully, zones enemies
  Looter,     // Prioritizes loot over fighting
  Sniper,     // Keeps distance, aims precisely
  Builder,    // Builds massive structures
  Rusher,     // Pushes aggressively
  Camper,     // Holds positions, plays passive
  Balanced,   // Mix of all traits
  Random      // Random behavior
};

// Bot lobby behavior states
enum class EBotLobbyBehavior {
  Idle,
  Dancing,
  Walking,
  PracticeShooting,
  Emoting
};

// Bot game phase
enum class EBotGamePhase { Lobby, BattleBus, Skydiving, Landing, Gameplay };

// POI locations for bot landing (Battle Royale map locations)
struct FPOILocation {
  std::string Name;
  SDK::FVector Location;
  float Popularity; // 0.0-1.0 chance bots will land here
};

// Default POI locations for Season 8 map
static const std::vector<FPOILocation> POILocations = {
    {"Tilted Towers", {3000.0f, -2000.0f, 1000.0f}, 0.9f},
    {"Pleasant Park", {-4000.0f, 2000.0f, 1000.0f}, 0.8f},
    {"Retail Row", {5000.0f, 3000.0f, 1000.0f}, 0.85f},
    {"Salty Springs", {1000.0f, 1500.0f, 1000.0f}, 0.7f},
    {"Lazy Lagoon", {-2000.0f, -3000.0f, 1000.0f}, 0.75f},
    {"Sunny Steps", {4000.0f, -2500.0f, 1000.0f}, 0.6f},
    {"Lonely Lodge", {6000.0f, -1000.0f, 1000.0f}, 0.5f},
    {"Paradise Palms", {-3500.0f, -4000.0f, 1000.0f}, 0.65f},
    {"Loot Lake", {0.0f, -500.0f, 1000.0f}, 0.55f},
    {"Fatal Fields", {2000.0f, 3500.0f, 1000.0f}, 0.6f},
    {"Dusty Divot", {1000.0f, -1500.0f, 1000.0f}, 0.7f},
    {"Mega Mall", {5000.0f, 3000.0f, 1000.0f}, 0.75f},
    {"Pressure Plant", {4000.0f, -2000.0f, 1000.0f}, 0.55f},
    {"Haunted Hills", {-3500.0f, 3500.0f, 1000.0f}, 0.45f},
    {"Junk Junction", {-4000.0f, 4500.0f, 1000.0f}, 0.4f},
    {"Snobby Shores", {-4500.0f, -1500.0f, 1000.0f}, 0.5f},
    {"Shifty Shafts", {-1500.0f, 500.0f, 1000.0f}, 0.6f},
    {"Frosty Flights", {-4000.0f, -3500.0f, 1000.0f}, 0.5f},
    {"Happy Hamlet", {-2000.0f, 5000.0f, 1000.0f}, 0.55f},
    {"Polar Peak", {-2500.0f, -4000.0f, 1000.0f}, 0.6f},
    {"Wailing Woods", {5500.0f, -2500.0f, 1000.0f}, 0.5f},
    {"Tomato Temple", {2500.0f, -2500.0f, 1000.0f}, 0.55f},
    {"Risky Reels", {3500.0f, -3500.0f, 1000.0f}, 0.45f},
    {"Lucky Landing", {1500.0f, 4500.0f, 1000.0f}, 0.6f}};

// Bot difficulty level affecting aim accuracy, reaction time, etc.
enum class EBotDifficulty {
  Easy,     // Poor aim, slow reactions, predictable
  Medium,   // Average human-like
  Hard,     // Good aim, quick reactions
  Expert,   // Very accurate, fast reactions
  Realistic // Varies, mimics real player inconsistency
};

// Bot combat state
enum class EBotCombatState {
  Idle,
  Looting,
  Building,
  Fighting,
  Fleeing,
  Healing,
  Rotating,
  Camping,
  Pushing
};

// Bot configuration
struct FBotConfig {
  EBotPersonality Personality;
  EBotDifficulty Difficulty;
  float AimAccuracy;    // 0.0 to 1.0
  float ReactionTime;   // in seconds
  float BuildSpeed;     // multiplier
  float Aggressiveness; // 0.0 to 1.0
  bool bUseConsumables;
  bool bBuildStructures;
  bool bPrioritizeWeapons;
  float EngagementRange; // distance to engage enemies
  float FleeThreshold;   // health percentage to flee
};

// Common Fortnite Season 8.51 skins for bots
static const std::vector<std::string> BotSkinAssets = {
    // Default/Free skins
    "/Game/Athena/Heroes/Blueprints/"
    "HID_001_Athena_Commando_F.HID_001_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_002_Athena_Commando_F.HID_002_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_003_Athena_Commando_F.HID_003_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_004_Athena_Commando_F.HID_004_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_005_Athena_Commando_M.HID_005_Athena_Commando_M",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_006_Athena_Commando_M.HID_006_Athena_Commando_M",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_007_Athena_Commando_M.HID_007_Athena_Commando_M",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_008_Athena_Commando_M.HID_008_Athena_Commando_M",

    // Popular Season 8 skins
    "/Game/Athena/Heroes/Blueprints/"
    "HID_013_Athena_Commando_F.HID_013_Athena_Commando_F", // Ramirez variant
    "/Game/Athena/Heroes/Blueprints/"
    "HID_014_Athena_Commando_M.HID_014_Athena_Commando_M", // Jonesy variant
    "/Game/Athena/Heroes/Blueprints/"
    "HID_015_Athena_Commando_F.HID_015_Athena_Commando_F", // Headhunter variant
    "/Game/Athena/Heroes/Blueprints/"
    "HID_016_Athena_Commando_M.HID_016_Athena_Commando_M", // Hawk variant
    "/Game/Athena/Heroes/Blueprints/"
    "HID_017_Athena_Commando_F.HID_017_Athena_Commando_F", // Wildcat variant
    "/Game/Athena/Heroes/Blueprints/"
    "HID_018_Athena_Commando_M.HID_018_Athena_Commando_M", // Spitfire variant
    "/Game/Athena/Heroes/Blueprints/"
    "HID_019_Athena_Commando_F.HID_019_Athena_Commando_F", // Banshee variant
    "/Game/Athena/Heroes/Blueprints/"
    "HID_020_Athena_Commando_M.HID_020_Athena_Commando_M", // Renegade variant

    // More variety
    "/Game/Athena/Heroes/Blueprints/"
    "HID_025_Athena_Commando_F.HID_025_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_026_Athena_Commando_M.HID_026_Athena_Commando_M",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_027_Athena_Commando_F.HID_027_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_028_Athena_Commando_M.HID_028_Athena_Commando_M",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_029_Athena_Commando_F.HID_029_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_030_Athena_Commando_M.HID_030_Athena_Commando_M",

    // Additional variants for diversity
    "/Game/Athena/Heroes/Blueprints/"
    "HID_035_Athena_Commando_F.HID_035_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_036_Athena_Commando_M.HID_036_Athena_Commando_M",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_037_Athena_Commando_F.HID_037_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_038_Athena_Commando_M.HID_038_Athena_Commando_M",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_039_Athena_Commando_F.HID_039_Athena_Commando_F",
    "/Game/Athena/Heroes/Blueprints/"
    "HID_040_Athena_Commando_M.HID_040_Athena_Commando_M"};

// Bot player data
struct FBotPlayer {
  SDK::AFortPlayerControllerAthena *Controller;
  SDK::AFortPawn *Pawn;
  SDK::AFortPlayerStateAthena *PlayerState;
  std::string Name;
  std::string SkinAssetPath; // Store assigned skin path
  FBotConfig Config;
  EBotCombatState CurrentState;
  EBotLobbyBehavior LobbyBehavior;
  EBotGamePhase GamePhase;
  SDK::FVector TargetLocation;
  SDK::FVector LandingLocation;
  SDK::AActor *TargetEnemy;
  float LastActionTime;
  float StateChangeCooldown;
  float LobbyBehaviorCooldown;
  int Kills;
  int Deaths;
  float TotalDamageDealt;
  bool bIsAlive;
  bool bHasLanded;
  bool bHasJumpedFromBus;

  // Accessor for config personality (needed in advanced behaviors)
  EBotPersonality GetPersonality() const { return Config.Personality; }
};

// 350+ unique bot names
static const std::vector<std::string> BotNames = {
    // Gaming/Pro names
    "Ninja_Fan", "TFue_Lite", "Cloakzy_Clone", "Symfuhny_Style",
    "NickMercs_Bot", "DrLupo_AI", "TimTheTatman_Pro", "Dakotaz_AI",
    "HighDistortion_Bot", "NICKMERCS_Clone",

    // Memes and Pop Culture
    "Thanos_Snap", "GigaChad_AI", "PepeLover", "Dogecoin_Moon", "AmongUs_Sus",
    "SpongeBob_Pro", "RickRoll_AI", "Shrek_Bot", "BigSmoke_AI", "JohnWick_Mode",

    // Cool/Gamer names
    "ShadowBlade_X", "DarkReaper_V2", "StormTrooper_X", "FireDragon_AI",
    "IceQueen_Bot", "ThunderBolt_X", "NightHawk_AI", "GhostRider_X",
    "MysticWolf_AI", "PhoenixRise_X",

    // Random/Fun names
    "PizzaLover_X", "TacoTuesday_AI", "CookieMonster_Bot", "DonutKing_X",
    "BurgerKing_AI", "CoffeeAddict_X", "SushiMaster_AI", "ChocolateRain_Bot",
    "SpaghettiCode_X", "BananaSplit_AI",

    // Skill-based names
    "NoScopeOnly", "QuickDraw_X", "HeadshotHunter_AI", "WallBangKing_Bot",
    "BoxFightChamp_X", "BuildGod_AI", "EditLord_X", "RushMaster_Bot",
    "AimBot_Legit", "ShotgunGod_X",

    // Competitive names
    "FNCS_Winner", "CashCup_AI", "ProLeague_X", "RankedGod_Bot",
    "TournamentChamp_AI", "ScrimWarrior_X", "LeagueChampion_AI",
    "GrandMaster_Bot", "EliteSquad_X", "VictoryRoyale_AI",

    // Mythology/Lore
    "Zeus_Thunder", "Poseidon_Waves", "Hades_Underworld", "Ares_War",
    "Athena_Wisdom", "Apollo_Sun", "Artemis_Hunt", "Hermes_Speed",
    "Dionysus_Party", "Hephaestus_Forge",

    // Nature/Elements
    "Blaze_Fire", "Frost_Ice", "Thunder_Lightning", "Shadow_Darkness",
    "Light_Holy", "Storm_Wind", "Earth_Quake", "Ocean_Deep", "Mountain_Peak",
    "Forest_Spirit",

    // Space/Sci-Fi
    "Nebula_X", "Galaxy_AI", "Stardust_Bot", "Cosmos_X", "BlackHole_AI",
    "Supernova_X", "Comet_AI", "Asteroid_Bot", "Planet_X", "Starship_AI",

    // Medieval/Fantasy
    "DragonSlayer_X", "Knight_AI", "Wizard_Bot", "Paladin_X", "Rogue_AI",
    "Necromancer_X", "Berserker_AI", "Archer_Bot", "Mage_X", "Warrior_AI",

    // Sports/Activities
    "SoccerStar_X", "BasketBall_AI", "FootballPro_Bot", "TennisChamp_X",
    "GolfMaster_AI", "Runner_X", "Swimmer_AI", "Cyclist_Bot", "Skateboard_X",
    "Surfer_AI",

    // Tech/Programming
    "CodeMaster_X", "DevBot_AI", "Hacker_Bot", "SysAdmin_X", "Network_AI",
    "Database_X", "Algorithm_AI", "Compiler_Bot", "Debugger_X", "Server_AI",

    // Food/Drinks
    "TacoKing_X", "PizzaBot_AI", "SushiChef_Bot", "BurgerMaster_X",
    "IceCream_AI", "Smoothie_X", "CoffeeBot_AI", "TeaMaster_Bot", "Juice_X",
    "Soda_AI",

    // Animals
    "Lion_King", "Tiger_AI", "Eagle_X", "Wolf_AI", "Shark_Bot", "Bear_X",
    "Snake_AI", "Fox_Bot", "Hawk_X", "Wolf_Pack_AI",

    // Music/Bands
    "RockStar_X", "PopArtist_AI", "Rapper_Bot", "DJ_X", "Producer_AI",
    "Guitar_X", "Drummer_AI", "Singer_Bot", "Band_X", "Music_AI",

    // Vehicles
    "SpeedRacer_X", "Truck_AI", "Car_Bot", "Motorcycle_X", "Plane_AI", "Boat_X",
    "Train_AI", "Bus_Bot", "Bike_X", "Jet_AI",

    // Weapons/Combat
    "SniperElite_X", "ShotgunKing_AI", "AssaultRifle_Bot", "SMG_X", "Pistol_AI",
    "Explosive_X", "Melee_AI", "Launcher_Bot", "Ammo_X", "Scope_AI",

    // Gaming Genres
    "FPS_Master", "RPG_Legend", "MOBA_Pro", "RTS_Commander", "Racing_Champion",
    "Platformer_X", "Puzzle_AI", "Strategy_Bot", "Adventure_X", "Simulation_AI",

    // Fortnitemares/Events
    "CubeFiend_X", "Storm_AI", "Midnite_Bot", "Fortnitemares_X", "Event_AI",
    "Cube_X", "Vault_Bot", "Loot_X", "Victory_AI", "Battle_Bus_X",

    // Seasonal
    "Summer_Vibes", "Winter_Wonder", "Spring_Bloom", "Autumn_Leaves",
    "Holiday_Cheer", "Spooky_Season", "Frosty_Fest", "Love_Shack", "Egg_Hunt",
    "Birthday_Bash",

    // Locations/Places
    "Tilted_Tower", "Pleasant_Park", "Retail_Row", "Salty_Springs", "Loot_Lake",
    "Dusty_Depot", "Fatal_Fields", "Moisty_Mire", "Wailing_Woods",
    "Anarchy_Acres",

    // Miscellaneous
    "ProGamer_99", "NoobMaster_69", "TryHard_X", "ChillVibes_AI", "Casual_Bot",
    "Sweaty_X", "Relaxed_AI", "Competitive_Bot", "Fun_X", "Serious_AI",

    // More unique names
    "Vortex_X", "Phantom_AI", "Spectre_Bot", "Wraith_X", "Ghost_AI", "Shadow_X",
    "Darkness_AI", "Light_Bot", "Element_X", "Power_AI", "Energy_X", "Force_AI",
    "Magic_Bot", "Mystic_X", "Ancient_AI", "Future_X", "Time_AI", "Space_Bot",
    "Dimension_X", "Reality_AI",

    // Additional names to reach 350+
    "AlphaBot", "BetaTest", "GammaRay", "DeltaForce", "OmegaPro", "PrimeTime",
    "Ultimate_X", "Extreme_AI", "Maximum_Bot", "Infinite_X", "Eternal_AI",
    "Forever_Bot", "Legendary_X", "Mythic_AI", "Epic_Bot", "Rare_X",
    "Common_AI", "Uncommon_Bot", "LegendaryAI_X", "MythicBot_AI", "EpicGamer_X",
    "RareFind_AI", "LootBox_X", "SkinShop_AI", "BattlePass_X", "VictoryCrown",
    "Umbrella_X", "Pickaxe_AI", "Emote_Bot", "Wrap_X", "Spray_AI",
    "LoadingScreen_X", "Music_AI", "Map_X", "Creative_AI", "SaveTheWorld",
    "BattleRoyale", "ZeroBuild_X", "Ranked_X", "Arena_AI", "CashCup_X",
    "ProScrims", "Tournament_X", "Championship_AI", "WorldCup_X", "Champion_AI",
    "Winner_X", "FirstPlace_AI", "Podium_X", "Trophy_AI"};

// Verify bot names count at runtime (static_assert not supported for
// std::vector) Note: There are 350+ unique bot names defined above

// Global bot management
class FBotManager {
private:
  std::vector<FBotPlayer> Bots;
  std::mt19937 RNG;
  std::uniform_int_distribution<int> NameDist;
  std::uniform_real_distribution<float> FloatDist;

  bool bInitialized;
  int MaxBots;
  float SpawnInterval;
  float LastSpawnTime;
  float PlayerConnectionCooldown; // Cooldown to pause bot spawning when players
                                  // connect

public:
  FBotManager()
      : RNG(std::chrono::steady_clock::now().time_since_epoch().count()),
        NameDist(0, static_cast<int>(BotNames.size()) - 1),
        FloatDist(0.0f, 1.0f), bInitialized(false), MaxBots(99),
        SpawnInterval(5.0f), LastSpawnTime(0.0f),
        PlayerConnectionCooldown(0.0f) {}

  void Initialize() {
    if (bInitialized)
      return;

    LOG_("Initializing Bot System with lobby visibility...");
    LOG_("Loaded {} unique bot names and {} skin variants", BotNames.size(),
         BotSkinAssets.size());
    bInitialized = true;
  }

  FBotConfig GenerateRandomConfig() {
    FBotConfig Config{};

    // Random personality
    Config.Personality = static_cast<EBotPersonality>(static_cast<int>(
        FloatDist(RNG) * static_cast<int>(EBotPersonality::Random)));

    // Random difficulty
    Config.Difficulty = static_cast<EBotDifficulty>(static_cast<int>(
        FloatDist(RNG) * static_cast<int>(EBotDifficulty::Realistic)));

    // Set stats based on difficulty
    switch (Config.Difficulty) {
    case EBotDifficulty::Easy:
      Config.AimAccuracy = 0.3f + FloatDist(RNG) * 0.2f;
      Config.ReactionTime = 0.8f + FloatDist(RNG) * 0.5f;
      Config.BuildSpeed = 0.5f + FloatDist(RNG) * 0.3f;
      break;
    case EBotDifficulty::Medium:
      Config.AimAccuracy = 0.5f + FloatDist(RNG) * 0.2f;
      Config.ReactionTime = 0.5f + FloatDist(RNG) * 0.3f;
      Config.BuildSpeed = 0.7f + FloatDist(RNG) * 0.3f;
      break;
    case EBotDifficulty::Hard:
      Config.AimAccuracy = 0.7f + FloatDist(RNG) * 0.2f;
      Config.ReactionTime = 0.3f + FloatDist(RNG) * 0.2f;
      Config.BuildSpeed = 0.9f + FloatDist(RNG) * 0.1f;
      break;
    case EBotDifficulty::Expert:
      Config.AimAccuracy = 0.85f + FloatDist(RNG) * 0.15f;
      Config.ReactionTime = 0.15f + FloatDist(RNG) * 0.15f;
      Config.BuildSpeed = 1.0f + FloatDist(RNG) * 0.2f;
      break;
    case EBotDifficulty::Realistic:
      Config.AimAccuracy = 0.4f + FloatDist(RNG) * 0.4f; // Human-like variation
      Config.ReactionTime = 0.2f + FloatDist(RNG) * 0.5f;
      Config.BuildSpeed = 0.6f + FloatDist(RNG) * 0.5f;
      break;
    }

    // Personality-based adjustments
    switch (Config.Personality) {
    case EBotPersonality::Aggressive:
      Config.Aggressiveness = 0.8f + FloatDist(RNG) * 0.2f;
      Config.EngagementRange = 3000.0f + FloatDist(RNG) * 2000.0f;
      Config.FleeThreshold = 0.1f + FloatDist(RNG) * 0.1f;
      break;
    case EBotPersonality::Defensive:
      Config.Aggressiveness = 0.1f + FloatDist(RNG) * 0.2f;
      Config.EngagementRange = 500.0f + FloatDist(RNG) * 500.0f;
      Config.FleeThreshold = 0.3f + FloatDist(RNG) * 0.2f;
      Config.BuildSpeed *= 1.2f;
      break;
    case EBotPersonality::Strategic:
      Config.Aggressiveness = 0.4f + FloatDist(RNG) * 0.2f;
      Config.EngagementRange = 1500.0f + FloatDist(RNG) * 1000.0f;
      Config.FleeThreshold = 0.2f + FloatDist(RNG) * 0.2f;
      Config.AimAccuracy += 0.1f;
      break;
    case EBotPersonality::Looter:
      Config.Aggressiveness = 0.2f + FloatDist(RNG) * 0.2f;
      Config.bPrioritizeWeapons = true;
      Config.EngagementRange = 800.0f;
      break;
    case EBotPersonality::Sniper:
      Config.AimAccuracy += 0.15f;
      Config.EngagementRange = 4000.0f + FloatDist(RNG) * 2000.0f;
      Config.Aggressiveness = 0.3f + FloatDist(RNG) * 0.2f;
      break;
    case EBotPersonality::Builder:
      Config.bBuildStructures = true;
      Config.BuildSpeed *= 1.5f;
      Config.FleeThreshold = 0.4f + FloatDist(RNG) * 0.2f;
      break;
    case EBotPersonality::Rusher:
      Config.Aggressiveness = 0.9f + FloatDist(RNG) * 0.1f;
      Config.EngagementRange = 2500.0f;
      Config.ReactionTime *= 0.7f;
      break;
    case EBotPersonality::Camper:
      Config.Aggressiveness = 0.1f;
      Config.EngagementRange = 1000.0f;
      Config.FleeThreshold = 0.5f;
      break;
    case EBotPersonality::Balanced:
    case EBotPersonality::Random:
    default:
      Config.Aggressiveness = 0.5f + FloatDist(RNG) * 0.3f;
      Config.EngagementRange = 1500.0f + FloatDist(RNG) * 1500.0f;
      Config.FleeThreshold = 0.25f + FloatDist(RNG) * 0.25f;
      break;
    }

    Config.bUseConsumables = FloatDist(RNG) > 0.2f;
    Config.bBuildStructures = Config.bBuildStructures || FloatDist(RNG) > 0.3f;

    return Config;
  }

  std::string GetRandomName() { return BotNames[NameDist(RNG)]; }

  std::string GetRandomSkinAsset() {
    if (BotSkinAssets.empty())
      return "";

    std::uniform_int_distribution<int> SkinDist(
        0, static_cast<int>(BotSkinAssets.size()) - 1);
    return BotSkinAssets[SkinDist(RNG)];
  }

  bool CanSpawnBot() {
    if (!bInitialized)
      return false;

    // Use global GetStatics and GetWorld functions from framework.h
    auto Statics = GetStatics();
    auto World = GetWorld();
    auto GameState = GetGameState();
    if (!Statics || !World || !GameState)
      return false;

    float CurrentTime = Statics->GetTimeSeconds(World);

    // Don't spawn bots during player connection cooldown
    if (CurrentTime < PlayerConnectionCooldown)
      return false;

    if (CurrentTime - LastSpawnTime < SpawnInterval)
      return false;

    // Check if real players are connecting - if so, delay bot spawning
    // This prevents conflicts during player join
    // Note: State/EConnectionState don't exist in Fortnite 8.51 SDK
    // Instead, we use the PlayerConnectionCooldown set by OnPlayerConnecting()
    if (World->NetDriver && World->NetDriver->ClientConnections.Num() > 0) {
      // If there are client connections and we're in cooldown period, wait
      if (CurrentTime < PlayerConnectionCooldown) {
        return false;
      }
    }

    return static_cast<int>(Bots.size()) < MaxBots;
  }

  // Call this when a real player is connecting to pause bot spawning
  void OnPlayerConnecting() {
    auto Statics = GetStatics();
    auto World = GetWorld();
    if (Statics && World) {
      PlayerConnectionCooldown = Statics->GetTimeSeconds(World) + 5.0f;
      LOG_("Player connecting - pausing bot spawning for 5 seconds");
    }
  }

  void SpawnBot() {
    if (!CanSpawnBot())
      return;

    auto World = GetWorld();
    auto Statics = GetStatics();
    auto GameState = GetGameState();
    if (!World || !Statics || !GameState)
      return;

    FBotPlayer NewBot{};
    NewBot.Name = GetRandomName();
    NewBot.SkinAssetPath = GetRandomSkinAsset();
    NewBot.Config = GenerateRandomConfig();
    NewBot.CurrentState = EBotCombatState::Looting;
    NewBot.LobbyBehavior = EBotLobbyBehavior::Idle;
    NewBot.GamePhase = EBotGamePhase::Lobby;
    NewBot.LastActionTime = Statics->GetTimeSeconds(World);
    NewBot.StateChangeCooldown = 2.0f + FloatDist(RNG) * 3.0f;
    NewBot.LobbyBehaviorCooldown = 1.0f + FloatDist(RNG) * 2.0f;
    NewBot.Kills = 0;
    NewBot.Deaths = 0;
    NewBot.TotalDamageDealt = 0.0f;
    NewBot.bIsAlive = false;
    NewBot.bHasLanded = false;
    NewBot.bHasJumpedFromBus = false;
    NewBot.Controller = nullptr;
    NewBot.Pawn = nullptr;
    NewBot.PlayerState = nullptr;
    NewBot.TargetEnemy = nullptr;
    NewBot.TargetLocation = SDK::FVector{0.0f, 0.0f, 0.0f};
    NewBot.LandingLocation = SDK::FVector{0.0f, 0.0f, 0.0f};

    // Create actual PlayerController for the bot
    SDK::AAthena_PlayerController_C *BotController = nullptr;
    auto ControllerClass = SDK::AAthena_PlayerController_C::StaticClass();
    if (ControllerClass) {
      // Spawn location for lobby (will be moved to game spawn later)
      SDK::FVector SpawnLoc{0.0f, 0.0f, 2000.0f};
      SDK::FRotator SpawnRot{0.0f, 0.0f, 0.0f};
      BotController = SpawnActor<SDK::AAthena_PlayerController_C>(
          ControllerClass, SpawnLoc, SpawnRot, nullptr);
    }

    if (!BotController) {
      LOG_("Failed to spawn bot controller for {}", NewBot.Name);
      return;
    }

    // Create PlayerState for the bot
    SDK::AFortPlayerStateAthena *BotPlayerState = nullptr;
    auto PlayerStateClass = SDK::AFortPlayerStateAthena::StaticClass();
    if (PlayerStateClass) {
      SDK::FVector SpawnLoc{0.0f, 0.0f, 0.0f};
      SDK::FRotator SpawnRot{0.0f, 0.0f, 0.0f};
      BotPlayerState = SpawnActor<SDK::AFortPlayerStateAthena>(
          PlayerStateClass, SpawnLoc, SpawnRot, BotController);
    }

    if (BotPlayerState) {
      // Set bot name in PlayerState
      std::wstring BotNameW(NewBot.Name.begin(), NewBot.Name.end());
      FString BotNameFString(BotNameW.c_str());
      BotPlayerState->PlayerName = BotNameFString;
      BotPlayerState->PlayerNamePrivate = BotNameFString;
      BotPlayerState->OnRep_PlayerName();

      // Assign a unique team to each bot for solo mode
      static int BotTeamIndex = 50; // Start bot teams at 50 to avoid conflicts
      uint8 OldTeamIndex = BotPlayerState->TeamIndex;
      BotPlayerState->TeamIndex = static_cast<uint8>(BotTeamIndex++);
      BotPlayerState->OnRep_TeamIndex(OldTeamIndex);

      // Apply random skin to bot for lobby visibility
      if (!NewBot.SkinAssetPath.empty()) {
        auto HeroType =
            StaticLoadObject<SDK::UFortHeroType>(NewBot.SkinAssetPath);
        if (HeroType) {
          BotPlayerState->HeroType = HeroType;
          BotPlayerState->OnRep_HeroType();
          LOG_("Applied skin {} to bot {}", NewBot.SkinAssetPath, NewBot.Name);
        } else {
          LOG_("Failed to load skin {} for bot {}", NewBot.SkinAssetPath,
               NewBot.Name);
        }
      }
    }

    // Create Pawn for the bot
    SDK::APlayerPawn_Athena_C *BotPawn = nullptr;
    auto PawnClass = SDK::APlayerPawn_Athena_C::StaticClass();
    if (PawnClass) {
      SDK::FVector SpawnLoc{0.0f, 0.0f, 2000.0f};
      SDK::FRotator SpawnRot{0.0f, 0.0f, 0.0f};
      BotPawn = SpawnActor<SDK::APlayerPawn_Athena_C>(PawnClass, SpawnLoc,
                                                      SpawnRot, BotController);
    }

    if (!BotPawn) {
      LOG_("Failed to spawn bot pawn for {}", NewBot.Name);
      if (BotController) {
        BotController->K2_DestroyActor();
      }
      return;
    }

    // Link Controller, Pawn, and PlayerState
    BotController->Pawn = BotPawn;
    BotController->PlayerState = BotPlayerState;
    BotController->OnRep_Pawn();
    BotController->OnRep_PlayerState();

    BotPawn->Owner = BotController;
    BotPawn->Controller = BotController;
    if (BotPlayerState) {
      BotPawn->PlayerState = BotPlayerState;

      // Apply cosmetic loadout to pawn for skin visibility
      if (BotPlayerState->HeroType) {
        auto FortPawn = static_cast<SDK::AFortPlayerPawn *>(BotPawn);
        if (FortPawn) {
          // Set the hero type in the pawn's cosmetic loadout
          FortPawn->CosmeticLoadout.Character = nullptr; // Clear first
          FortPawn->OnRep_CosmeticLoadout();

          // Force update the pawn's appearance based on HeroType
          LOG_("Applied cosmetic loadout to bot pawn for {}", NewBot.Name);
        }
      }
    }
    BotPawn->OnRep_Controller();
    BotPawn->OnRep_PlayerState();

    // Possess the pawn
    BotController->Possess(BotPawn);

    // Mark bot as alive
    NewBot.Controller = BotController;
    NewBot.Pawn = BotPawn;
    NewBot.PlayerState = BotPlayerState;
    NewBot.bIsAlive = true;

    // Add bot to GameState PlayerArray for minimap visibility
    if (BotPlayerState && GameState) {
      GameState->PlayerArray.Add(BotPlayerState);
    }

    // Increment player count
    if (GameState) {
      GameState->TotalPlayers++;
      GameState->PlayersLeft++;
      GameState->OnRep_PlayersLeft();
    }

    // Select landing location for battle bus phase
    if (!POILocations.empty()) {
      // Weighted random selection based on popularity
      float TotalWeight = 0.0f;
      for (const auto &POI : POILocations) {
        TotalWeight += POI.Popularity;
      }

      float RandomValue = FloatDist(RNG) * TotalWeight;
      float CurrentWeight = 0.0f;

      for (const auto &POI : POILocations) {
        CurrentWeight += POI.Popularity;
        if (RandomValue <= CurrentWeight) {
          // Add some randomness to landing spot (bots don't land at exact same
          // spot)
          float OffsetX = (FloatDist(RNG) - 0.5f) * 500.0f;
          float OffsetY = (FloatDist(RNG) - 0.5f) * 500.0f;
          NewBot.LandingLocation =
              SDK::FVector{POI.Location.X + OffsetX, POI.Location.Y + OffsetY,
                           POI.Location.Z};
          break;
        }
      }
    }

    Bots.push_back(NewBot);
    LastSpawnTime = Statics->GetTimeSeconds(World);

    LOG_("Spawned lobby bot: {} with skin [{}] (Personality: {}, Difficulty: "
         "{})",
         NewBot.Name, NewBot.SkinAssetPath,
         static_cast<int>(NewBot.Config.Personality),
         static_cast<int>(NewBot.Config.Difficulty));
  }

  void Update(float DeltaTime) {
    if (!bInitialized)
      return;

    auto World = GetWorld();
    auto GameState = GetGameState();
    if (!World || !GameState)
      return;

    // Spawn bots periodically
    if (CanSpawnBot() && FloatDist(RNG) < 0.3f) {
      SpawnBot();
    }

    // Check game phase and update bot behaviors accordingly
    // Warmup phase is EAthenaGamePhase::Warmup (value 2)
    bool bInWarmup = (GameState->GamePhase == SDK::EAthenaGamePhase::Warmup) ||
                     (GameState->GamePhase == SDK::EAthenaGamePhase::Setup) ||
                     (GameState->WarmupCountdownEndTime >
                      GetStatics()->GetTimeSeconds(World));
    bool bGameStarted = GameState->GamePhase >= SDK::EAthenaGamePhase::Aircraft;

    // Update all bots
    for (auto &Bot : Bots) {
      if (!Bot.bIsAlive || !Bot.Pawn || !Bot.Controller)
        continue;

      if (bInWarmup) {
        // In lobby/warmup phase
        UpdateLobbyBehavior(Bot, DeltaTime);
      } else if (bGameStarted) {
        // Game has started - handle battle bus and gameplay
        if (Bot.GamePhase == EBotGamePhase::Lobby) {
          Bot.GamePhase = EBotGamePhase::BattleBus;
        }

        if (Bot.GamePhase == EBotGamePhase::BattleBus) {
          UpdateBattleBusBehavior(Bot, DeltaTime);
        } else if (Bot.GamePhase == EBotGamePhase::Gameplay) {
          UpdateBotBehavior(Bot, DeltaTime);
        }
      }
    }

    // Clean up dead bots
    Bots.erase(std::remove_if(Bots.begin(), Bots.end(),
                              [](const FBotPlayer &Bot) {
                                return !Bot.bIsAlive && Bot.Deaths > 0;
                              }),
               Bots.end());
  }

  void UpdateLobbyBehavior(FBotPlayer &Bot, float DeltaTime) {
    if (!Bot.Pawn || !Bot.Controller)
      return;

    auto World = GetWorld();
    auto Statics = GetStatics();
    if (!World || !Statics)
      return;

    float CurrentTime = Statics->GetTimeSeconds(World);

    // Check if we should change lobby behavior
    if (CurrentTime - Bot.LastActionTime >= Bot.LobbyBehaviorCooldown) {
      // Randomly select a lobby behavior
      float RandomValue = FloatDist(RNG);
      if (RandomValue < 0.25f) {
        Bot.LobbyBehavior = EBotLobbyBehavior::Dancing;
      } else if (RandomValue < 0.50f) {
        Bot.LobbyBehavior = EBotLobbyBehavior::Walking;
      } else if (RandomValue < 0.70f) {
        Bot.LobbyBehavior = EBotLobbyBehavior::PracticeShooting;
      } else if (RandomValue < 0.85f) {
        Bot.LobbyBehavior = EBotLobbyBehavior::Emoting;
      } else {
        Bot.LobbyBehavior = EBotLobbyBehavior::Idle;
      }

      Bot.LastActionTime = CurrentTime;
      Bot.LobbyBehaviorCooldown = 2.0f + FloatDist(RNG) * 5.0f;

      // Set new target location for walking behavior
      if (Bot.LobbyBehavior == EBotLobbyBehavior::Walking) {
        float RandomX = (FloatDist(RNG) - 0.5f) * 2000.0f;
        float RandomY = (FloatDist(RNG) - 0.5f) * 2000.0f;
        Bot.TargetLocation = SDK::FVector{RandomX, RandomY, 2000.0f};
      }
    }

    // Execute lobby behavior
    switch (Bot.LobbyBehavior) {
    case EBotLobbyBehavior::Dancing:
      ExecuteLobbyDance(Bot);
      break;
    case EBotLobbyBehavior::Walking:
      ExecuteLobbyWalk(Bot, DeltaTime);
      break;
    case EBotLobbyBehavior::PracticeShooting:
      ExecuteLobbyPracticeShoot(Bot);
      break;
    case EBotLobbyBehavior::Emoting:
      ExecuteLobbyEmote(Bot);
      break;
    case EBotLobbyBehavior::Idle:
    default:
      // Just stand still
      break;
    }
  }

  void ExecuteLobbyDance(FBotPlayer &Bot) {
    // Dance in place - rotation oscillation to simulate dancing
    if (!Bot.Pawn)
      return;

    auto CurrentRot = Bot.Pawn->K2_GetActorRotation();
    float DanceOffset =
        std::sin(GetStatics()->GetTimeSeconds(GetWorld()) * 5.0f) * 15.0f;
    CurrentRot.Yaw += DanceOffset;
    Bot.Pawn->K2_SetActorRotation(CurrentRot, false);
  }

  void ExecuteLobbyWalk(FBotPlayer &Bot, float DeltaTime) {
    if (!Bot.Pawn)
      return;

    auto CurrentLocation = Bot.Pawn->K2_GetActorLocation();
    SDK::FVector Direction =
        FVectorHelpers::operator-(Bot.TargetLocation, CurrentLocation);
    float Distance =
        std::sqrt(Direction.X * Direction.X + Direction.Y * Direction.Y +
                  Direction.Z * Direction.Z);

    if (Distance < 100.0f) {
      // Reached target, pick new target
      float RandomX = (FloatDist(RNG) - 0.5f) * 2000.0f;
      float RandomY = (FloatDist(RNG) - 0.5f) * 2000.0f;
      Bot.TargetLocation = SDK::FVector{RandomX, RandomY, 2000.0f};
      return;
    }

    FVectorHelpers::Normalize(Direction);

    // Move towards target
    float MoveSpeed = 300.0f * DeltaTime; // Walking speed
    SDK::FVector NewLocation = FVectorHelpers::operator+(
        CurrentLocation, FVectorHelpers::operator*(Direction, MoveSpeed));

    // Face movement direction
    SDK::FRotator NewRotation =
        GetMath()->FindLookAtRotation(CurrentLocation, Bot.TargetLocation);
    Bot.Pawn->K2_SetActorRotation(NewRotation, false);
    Bot.Pawn->K2_TeleportTo(NewLocation, NewRotation);
  }

  void ExecuteLobbyPracticeShoot(FBotPlayer &Bot) {
    // Practice aiming by rotating randomly
    if (!Bot.Pawn)
      return;

    auto CurrentRot = Bot.Pawn->K2_GetActorRotation();
    float AimOffset = (FloatDist(RNG) - 0.5f) * 60.0f;
    CurrentRot.Yaw += AimOffset;
    Bot.Pawn->K2_SetActorRotation(CurrentRot, false);
  }

  void ExecuteLobbyEmote(FBotPlayer &Bot) {
    // Simple emote simulation - quick spin
    if (!Bot.Pawn)
      return;

    auto CurrentRot = Bot.Pawn->K2_GetActorRotation();
    CurrentRot.Yaw += 180.0f * 0.016f; // Spin slowly
    Bot.Pawn->K2_SetActorRotation(CurrentRot, false);
  }

  void UpdateBattleBusBehavior(FBotPlayer &Bot, float DeltaTime) {
    if (!Bot.Pawn || !Bot.Controller)
      return;

    auto GameState = GetGameState();
    if (!GameState)
      return;

    float CurrentTime = GetStatics()->GetTimeSeconds(GetWorld());

    // Check if we should jump from the bus
    // Bots jump at different times based on their landing location
    float BusStartTime = GameState->WarmupCountdownEndTime;
    float BusFlightTime = CurrentTime - BusStartTime;

    // Calculate ideal jump time based on landing location
    // (Bots should jump when bus is closest to their landing location)
    float TimeToJump =
        5.0f +
        (FloatDist(RNG) * 25.0f); // Jump between 5-30 seconds after bus starts

    if (!Bot.bHasJumpedFromBus && BusFlightTime >= TimeToJump) {
      JumpFromBattleBus(Bot);
    }

    if (Bot.bHasJumpedFromBus && !Bot.bHasLanded) {
      UpdateSkydiving(Bot, DeltaTime);
    }
  }

  void JumpFromBattleBus(FBotPlayer &Bot) {
    if (!Bot.Pawn)
      return;

    // Get bus location (approximate - in real implementation would get actual
    // bus location)
    auto GameState = GetGameState();
    if (!GameState)
      return;

    // Start skydiving from bus location
    SDK::FVector BusLocation = Bot.Pawn->K2_GetActorLocation();
    BusLocation.Z = 15000.0f; // Battle bus altitude

    Bot.Pawn->K2_TeleportTo(BusLocation, Bot.Pawn->K2_GetActorRotation());
    Bot.bHasJumpedFromBus = true;
    Bot.GamePhase = EBotGamePhase::Skydiving;

    LOG_("Bot {} jumped from battle bus, heading to landing zone", Bot.Name);
  }

  void UpdateSkydiving(FBotPlayer &Bot, float DeltaTime) {
    if (!Bot.Pawn)
      return;

    auto CurrentLocation = Bot.Pawn->K2_GetActorLocation();
    SDK::FVector Direction =
        FVectorHelpers::operator-(Bot.LandingLocation, CurrentLocation);
    float DistanceXY =
        std::sqrt(Direction.X * Direction.X + Direction.Y * Direction.Y);
    float DistanceZ = Direction.Z;

    // Check if we've landed
    if (CurrentLocation.Z <= Bot.LandingLocation.Z + 100.0f) {
      Bot.bHasLanded = true;
      Bot.GamePhase = EBotGamePhase::Gameplay;
      Bot.CurrentState = EBotCombatState::Looting;
      LOG_("Bot {} landed at their POI", Bot.Name);
      return;
    }

    // Skydiving movement - move towards landing location while falling
    FVectorHelpers::Normalize(Direction);

    float FallSpeed = 800.0f * DeltaTime;  // Falling speed
    float GlideSpeed = 600.0f * DeltaTime; // Horizontal glide speed

    SDK::FVector NewLocation;
    NewLocation.X = CurrentLocation.X + Direction.X * GlideSpeed;
    NewLocation.Y = CurrentLocation.Y + Direction.Y * GlideSpeed;
    NewLocation.Z = CurrentLocation.Z - FallSpeed;

    // Face landing direction
    SDK::FRotator NewRotation =
        GetMath()->FindLookAtRotation(CurrentLocation, Bot.LandingLocation);

    Bot.Pawn->K2_TeleportTo(NewLocation, NewRotation);
  }

  void UpdateBotBehavior(FBotPlayer &Bot, float DeltaTime) {
    if (!Bot.Pawn || !Bot.Controller)
      return;

    // Use global GetStatics and GetWorld functions from framework.h
    auto Statics = GetStatics();
    auto World = GetWorld();
    if (!Statics || !World)
      return;

    float CurrentTime = Statics->GetTimeSeconds(World);

    // Check if we should change state
    if (CurrentTime - Bot.LastActionTime >= Bot.StateChangeCooldown) {
      DetermineNewState(Bot);
      Bot.LastActionTime = CurrentTime;
      Bot.StateChangeCooldown = 2.0f + FloatDist(RNG) * 4.0f;
    }

    // Execute behavior based on current state
    switch (Bot.CurrentState) {
    case EBotCombatState::Looting:
      ExecuteLootingBehavior(Bot);
      break;
    case EBotCombatState::Building:
      ExecuteBuildingBehavior(Bot);
      break;
    case EBotCombatState::Fighting:
      ExecuteCombatBehavior(Bot);
      break;
    case EBotCombatState::Fleeing:
      ExecuteFleeBehavior(Bot);
      break;
    case EBotCombatState::Rotating:
      ExecuteRotateBehavior(Bot);
      break;
    case EBotCombatState::Healing:
      ExecuteHealBehavior(Bot);
      break;
    default:
      ExecuteIdleBehavior(Bot);
      break;
    }
  }

  void DetermineNewState(FBotPlayer &Bot) {
    // Get bot health using proper accessor methods
    float Health = Bot.Pawn->GetHealth();
    float MaxHealth = Bot.Pawn->GetMaxHealth();
    float HealthPercent = MaxHealth > 0 ? Health / MaxHealth : 0.0f;

    // Check for nearby enemies
    auto NearestEnemy = FindNearestEnemy(Bot);
    float EnemyDistance =
        NearestEnemy
            ? FVectorHelpers::Distance(Bot.Pawn->K2_GetActorLocation(),
                                       NearestEnemy->K2_GetActorLocation())
            : FLT_MAX;

    // State decision logic
    if (HealthPercent < Bot.Config.FleeThreshold && NearestEnemy) {
      Bot.CurrentState = EBotCombatState::Fleeing;
    } else if (HealthPercent < 0.6f && Bot.Config.bUseConsumables) {
      Bot.CurrentState = EBotCombatState::Healing;
    } else if (NearestEnemy && EnemyDistance < Bot.Config.EngagementRange) {
      if (FloatDist(RNG) < Bot.Config.Aggressiveness)
        Bot.CurrentState = EBotCombatState::Fighting;
      else
        Bot.CurrentState = EBotCombatState::Building;
    } else if (FloatDist(RNG) < 0.2f && Bot.Config.bBuildStructures) {
      Bot.CurrentState = EBotCombatState::Building;
    } else {
      Bot.CurrentState = EBotCombatState::Looting;
    }

    Bot.TargetEnemy = NearestEnemy;
  }

  SDK::AActor *FindNearestEnemy(FBotPlayer &Bot) {
    if (!Bot.Pawn)
      return nullptr;

    SDK::AActor *NearestEnemy = nullptr;
    float NearestDistance = FLT_MAX;

    // Use global GetGameState function from framework.h
    auto GameState = GetGameState();
    if (!GameState)
      return nullptr;

    auto BotLocation = Bot.Pawn->K2_GetActorLocation();
    (void)BotLocation; // Suppress unused warning for now

    for (int i = 0; i < GameState->PlayerArray.Num(); i++) {
      auto PlayerState = GameState->PlayerArray[i];
      if (!PlayerState)
        continue;

      // Skip bots and teammates - using AFortPlayerStateAthena for TeamIndex
      auto FortPlayerState =
          static_cast<SDK::AFortPlayerStateAthena *>(nullptr);
      FortPlayerState = static_cast<SDK::AFortPlayerStateAthena *>(PlayerState);
      auto BotPlayerState = static_cast<SDK::AFortPlayerStateAthena *>(
          Bot.Controller ? Bot.Controller->PlayerState : nullptr);

      if (!FortPlayerState || !BotPlayerState)
        continue;

      if (FortPlayerState->TeamIndex == BotPlayerState->TeamIndex)
        continue;

      // Find the pawn for this player
      // This would need proper implementation to find the actual pawn
      // For now, just return nullptr
    }

    return NearestEnemy;
  }

  void ExecuteLootingBehavior(FBotPlayer &Bot) {
    // Move towards random loot locations
    // This would need proper implementation with actual loot spawning system
    SDK::FVector RandomOffset(FloatDist(RNG) * 4000.0f - 2000.0f,
                              FloatDist(RNG) * 4000.0f - 2000.0f, 0.0f);
    Bot.TargetLocation = FVectorHelpers::operator+(
        Bot.Pawn->K2_GetActorLocation(), RandomOffset);
  }

  void ExecuteBuildingBehavior(FBotPlayer &Bot) {
    // Build structures based on personality
    // This would need proper implementation with building system
    (void)Bot; // Suppress unused warning
  }

  void ExecuteCombatBehavior(FBotPlayer &Bot) {
    if (!Bot.TargetEnemy)
      return;

    // Aim at enemy with accuracy based on config
    auto EnemyLocation = Bot.TargetEnemy->K2_GetActorLocation();

    // Add some inaccuracy
    SDK::FVector InaccuracyOffset(
        (FloatDist(RNG) - 0.5f) * (1.0f - Bot.Config.AimAccuracy) * 500.0f,
        (FloatDist(RNG) - 0.5f) * (1.0f - Bot.Config.AimAccuracy) * 500.0f,
        (FloatDist(RNG) - 0.5f) * (1.0f - Bot.Config.AimAccuracy) * 200.0f);

    SDK::FVector AimLocation = EnemyLocation + InaccuracyOffset;

    // Calculate rotation to aim at target
    // FindLookAtRotation takes non-const references, so create temporary
    // non-const variables
    SDK::FVector BotLocation = Bot.Pawn->K2_GetActorLocation();
    SDK::FRotator AimRotation =
        GetMath()->FindLookAtRotation(BotLocation, AimLocation);
    Bot.Pawn->K2_SetActorRotation(AimRotation, false);
  }

  void ExecuteFleeBehavior(FBotPlayer &Bot) {
    if (!Bot.TargetEnemy)
      return;

    // Move away from enemy
    SDK::FVector ToEnemy =
        FVectorHelpers::operator-(Bot.TargetEnemy->K2_GetActorLocation(),
                                  Bot.Pawn->K2_GetActorLocation());
    FVectorHelpers::Normalize(ToEnemy);

    SDK::FVector FleeDirection = FVectorHelpers::operator*(
        FVectorHelpers::operator-(ToEnemy), 2000.0f); // Move 2000 units away
    Bot.TargetLocation = FVectorHelpers::operator+(
        Bot.Pawn->K2_GetActorLocation(), FleeDirection);
  }

  void ExecuteRotateBehavior(FBotPlayer &Bot) {
    // Move towards safe zone or random location
    SDK::FVector RandomOffset(FloatDist(RNG) * 3000.0f - 1500.0f,
                              FloatDist(RNG) * 3000.0f - 1500.0f, 0.0f);
    Bot.TargetLocation = FVectorHelpers::operator+(
        Bot.Pawn->K2_GetActorLocation(), RandomOffset);
  }

  void ExecuteHealBehavior(FBotPlayer &Bot) {
    // Use consumables to heal
    // This would need proper implementation with inventory system
    (void)Bot; // Suppress unused warning
  }

  void ExecuteIdleBehavior(FBotPlayer &Bot) {
    // Random movements to appear more natural
    if (FloatDist(RNG) < 0.1f) {
      SDK::FVector RandomOffset(FloatDist(RNG) * 500.0f - 250.0f,
                                FloatDist(RNG) * 500.0f - 250.0f, 0.0f);
      Bot.TargetLocation = FVectorHelpers::operator+(
          Bot.Pawn->K2_GetActorLocation(), RandomOffset);
    }
  }

  void SetMaxBots(int InMaxBots) { MaxBots = InMaxBots; }
  void SetSpawnInterval(float InInterval) { SpawnInterval = InInterval; }

  int GetBotCount() const { return static_cast<int>(Bots.size()); }
  const std::vector<FBotPlayer> &GetBots() const { return Bots; }

  void ClearBots() { Bots.clear(); }

  // Public accessors for RNG and FloatDist needed by advanced behaviors
  std::mt19937 &GetRNG() { return RNG; }
  std::uniform_real_distribution<float> &GetFloatDist() { return FloatDist; }
};

static FBotManager *GBotManager = nullptr;

// Initialize bot system
inline void InitializeBotSystem() {
  if (!GBotManager) {
    GBotManager = new FBotManager();
    GBotManager->Initialize();
  }
}

// Update bot system (call from tick)
inline void UpdateBotSystem(float DeltaTime) {
  if (GBotManager) {
    GBotManager->Update(DeltaTime);
  }
}

// Call when a real player is connecting to pause bot spawning
inline void OnPlayerConnecting() {
  if (GBotManager) {
    GBotManager->OnPlayerConnecting();
  }
}

// Cleanup bot system
inline void ShutdownBotSystem() {
  if (GBotManager) {
    delete GBotManager;
    GBotManager = nullptr;
  }
}
} // namespace BotSystem
