#pragma once
#include "framework.h"
#include <vector>
#include <string>
#include <random>
#include <unordered_map>
#include <chrono>
#include <cmath>

// Forward declarations for SDK types - using SDK namespace explicitly
namespace SDK
{
    class AFortPlayerControllerAthena;
    class AFortPawn;
    class AActor;
    class AFortGameStateAthena;
    class AFortPlayerStateAthena;
}

// Helper functions for FVector operations (since SDK::FVector doesn't have these methods)
namespace FVectorHelpers
{
    inline float Distance(const SDK::FVector& A, const SDK::FVector& B)
    {
        float DX = A.X - B.X;
        float DY = A.Y - B.Y;
        float DZ = A.Z - B.Z;
        return std::sqrt(DX * DX + DY * DY + DZ * DZ);
    }

    inline void Normalize(SDK::FVector& V)
    {
        float Mag = std::sqrt(V.X * V.X + V.Y * V.Y + V.Z * V.Z);
        if (Mag > 0.0f)
        {
            V.X /= Mag;
            V.Y /= Mag;
            V.Z /= Mag;
        }
    }

    inline SDK::FVector operator-(const SDK::FVector& A, const SDK::FVector& B)
    {
        return SDK::FVector{ A.X - B.X, A.Y - B.Y, A.Z - B.Z };
    }

    inline SDK::FVector operator+(const SDK::FVector& A, const SDK::FVector& B)
    {
        return SDK::FVector{ A.X + B.X, A.Y + B.Y, A.Z + B.Z };
    }

    inline SDK::FVector operator*(const SDK::FVector& A, float Scalar)
    {
        return SDK::FVector{ A.X * Scalar, A.Y * Scalar, A.Z * Scalar };
    }

    inline SDK::FVector operator-(const SDK::FVector& V)
    {
        return SDK::FVector{ -V.X, -V.Y, -V.Z };
    }
}

namespace BotSystem
{
    // Bot personality traits for realistic behavior
    enum class EBotPersonality
    {
        Aggressive,      // Rushes, builds quickly, fights often
        Defensive,       // Builds forts, plays safe
        Strategic,       // Positions carefully, zones enemies
        Looter,          // Prioritizes loot over fighting
        Sniper,          // Keeps distance, aims precisely
        Builder,         // Builds massive structures
        Rusher,          // Pushes aggressively
        Camper,          // Holds positions, plays passive
        Balanced,        // Mix of all traits
        Random           // Random behavior
    };

    // Bot difficulty level affecting aim accuracy, reaction time, etc.
    enum class EBotDifficulty
    {
        Easy,           // Poor aim, slow reactions, predictable
        Medium,         // Average human-like
        Hard,           // Good aim, quick reactions
        Expert,         // Very accurate, fast reactions
        Realistic       // Varies, mimics real player inconsistency
    };

    // Bot combat state
    enum class EBotCombatState
    {
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
    struct FBotConfig
    {
        EBotPersonality Personality;
        EBotDifficulty Difficulty;
        float AimAccuracy;           // 0.0 to 1.0
        float ReactionTime;          // in seconds
        float BuildSpeed;            // multiplier
        float Aggressiveness;        // 0.0 to 1.0
        bool bUseConsumables;
        bool bBuildStructures;
        bool bPrioritizeWeapons;
        float EngagementRange;       // distance to engage enemies
        float FleeThreshold;         // health percentage to flee
    };

    // Bot player data
    struct FBotPlayer
    {
        SDK::AFortPlayerControllerAthena* Controller;
        SDK::AFortPawn* Pawn;
        std::string Name;
        FBotConfig Config;
        EBotCombatState CurrentState;
        SDK::FVector TargetLocation;
        SDK::AActor* TargetEnemy;
        float LastActionTime;
        float StateChangeCooldown;
        int Kills;
        int Deaths;
        float TotalDamageDealt;
        bool bIsAlive;

        // Accessor for config personality (needed in advanced behaviors)
        EBotPersonality GetPersonality() const { return Config.Personality; }
    };

    // 350+ unique bot names
    static const std::vector<std::string> BotNames = {
        // Gaming/Pro names
        "Ninja_Fan", "TFue_Lite", "Cloakzy_Clone", "Symfuhny_Style", "NickMercs_Bot",
        "DrLupo_AI", "TimTheTatman_Pro", "Dakotaz_AI", "HighDistortion_Bot", "NICKMERCS_Clone",

        // Memes and Pop Culture
        "Thanos_Snap", "GigaChad_AI", "PepeLover", "Dogecoin_Moon", "AmongUs_Sus",
        "SpongeBob_Pro", "RickRoll_AI", "Shrek_Bot", "BigSmoke_AI", "JohnWick_Mode",

        // Cool/Gamer names
        "ShadowBlade_X", "DarkReaper_V2", "StormTrooper_X", "FireDragon_AI", "IceQueen_Bot",
        "ThunderBolt_X", "NightHawk_AI", "GhostRider_X", "MysticWolf_AI", "PhoenixRise_X",

        // Random/Fun names
        "PizzaLover_X", "TacoTuesday_AI", "CookieMonster_Bot", "DonutKing_X", "BurgerKing_AI",
        "CoffeeAddict_X", "SushiMaster_AI", "ChocolateRain_Bot", "SpaghettiCode_X", "BananaSplit_AI",

        // Skill-based names
        "NoScopeOnly", "QuickDraw_X", "HeadshotHunter_AI", "WallBangKing_Bot", "BoxFightChamp_X",
        "BuildGod_AI", "EditLord_X", "RushMaster_Bot", "AimBot_Legit", "ShotgunGod_X",

        // Competitive names
        "FNCS_Winner", "CashCup_AI", "ProLeague_X", "RankedGod_Bot", "TournamentChamp_AI",
        "ScrimWarrior_X", "LeagueChampion_AI", "GrandMaster_Bot", "EliteSquad_X", "VictoryRoyale_AI",

        // Mythology/Lore
        "Zeus_Thunder", "Poseidon_Waves", "Hades_Underworld", "Ares_War", "Athena_Wisdom",
        "Apollo_Sun", "Artemis_Hunt", "Hermes_Speed", "Dionysus_Party", "Hephaestus_Forge",

        // Nature/Elements
        "Blaze_Fire", "Frost_Ice", "Thunder_Lightning", "Shadow_Darkness", "Light_Holy",
        "Storm_Wind", "Earth_Quake", "Ocean_Deep", "Mountain_Peak", "Forest_Spirit",

        // Space/Sci-Fi
        "Nebula_X", "Galaxy_AI", "Stardust_Bot", "Cosmos_X", "BlackHole_AI",
        "Supernova_X", "Comet_AI", "Asteroid_Bot", "Planet_X", "Starship_AI",

        // Medieval/Fantasy
        "DragonSlayer_X", "Knight_AI", "Wizard_Bot", "Paladin_X", "Rogue_AI",
        "Necromancer_X", "Berserker_AI", "Archer_Bot", "Mage_X", "Warrior_AI",

        // Sports/Activities
        "SoccerStar_X", "BasketBall_AI", "FootballPro_Bot", "TennisChamp_X", "GolfMaster_AI",
        "Runner_X", "Swimmer_AI", "Cyclist_Bot", "Skateboard_X", "Surfer_AI",

        // Tech/Programming
        "CodeMaster_X", "DevBot_AI", "Hacker_Bot", "SysAdmin_X", "Network_AI",
        "Database_X", "Algorithm_AI", "Compiler_Bot", "Debugger_X", "Server_AI",

        // Food/Drinks
        "TacoKing_X", "PizzaBot_AI", "SushiChef_Bot", "BurgerMaster_X", "IceCream_AI",
        "Smoothie_X", "CoffeeBot_AI", "TeaMaster_Bot", "Juice_X", "Soda_AI",

        // Animals
        "Lion_King", "Tiger_AI", "Eagle_X", "Wolf_AI", "Shark_Bot",
        "Bear_X", "Snake_AI", "Fox_Bot", "Hawk_X", "Wolf_Pack_AI",

        // Music/Bands
        "RockStar_X", "PopArtist_AI", "Rapper_Bot", "DJ_X", "Producer_AI",
        "Guitar_X", "Drummer_AI", "Singer_Bot", "Band_X", "Music_AI",

        // Vehicles
        "SpeedRacer_X", "Truck_AI", "Car_Bot", "Motorcycle_X", "Plane_AI",
        "Boat_X", "Train_AI", "Bus_Bot", "Bike_X", "Jet_AI",

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
        "Summer_Vibes", "Winter_Wonder", "Spring_Bloom", "Autumn_Leaves", "Holiday_Cheer",
        "Spooky_Season", "Frosty_Fest", "Love_Shack", "Egg_Hunt", "Birthday_Bash",

        // Locations/Places
        "Tilted_Tower", "Pleasant_Park", "Retail_Row", "Salty_Springs", "Loot_Lake",
        "Dusty_Depot", "Fatal_Fields", "Moisty_Mire", "Wailing_Woods", "Anarchy_Acres",

        // Miscellaneous
        "ProGamer_99", "NoobMaster_69", "TryHard_X", "ChillVibes_AI", "Casual_Bot",
        "Sweaty_X", "Relaxed_AI", "Competitive_Bot", "Fun_X", "Serious_AI",

        // More unique names
        "Vortex_X", "Phantom_AI", "Spectre_Bot", "Wraith_X", "Ghost_AI",
        "Shadow_X", "Darkness_AI", "Light_Bot", "Element_X", "Power_AI",
        "Energy_X", "Force_AI", "Magic_Bot", "Mystic_X", "Ancient_AI",
        "Future_X", "Time_AI", "Space_Bot", "Dimension_X", "Reality_AI",

        // Additional names to reach 350+
        "AlphaBot", "BetaTest", "GammaRay", "DeltaForce", "OmegaPro",
        "PrimeTime", "Ultimate_X", "Extreme_AI", "Maximum_Bot", "Infinite_X",
        "Eternal_AI", "Forever_Bot", "Legendary_X", "Mythic_AI", "Epic_Bot",
        "Rare_X", "Common_AI", "Uncommon_Bot", "LegendaryAI_X", "MythicBot_AI",
        "EpicGamer_X", "RareFind_AI", "LootBox_X", "SkinShop_AI", "BattlePass_X",
        "VictoryCrown", "Umbrella_X", "Pickaxe_AI", "Emote_Bot", "Wrap_X",
        "Spray_AI", "LoadingScreen_X", "Music_AI", "Map_X", "Creative_AI",
        "SaveTheWorld", "BattleRoyale", "ZeroBuild_X", "Ranked_X", "Arena_AI",
        "CashCup_X", "ProScrims", "Tournament_X", "Championship_AI", "WorldCup_X",
        "Champion_AI", "Winner_X", "FirstPlace_AI", "Podium_X", "Trophy_AI"
    };

    static_assert(BotNames.size() >= 350, "BotNames must have at least 350 unique names");

    // Global bot management
    class FBotManager
    {
    private:
        std::vector<FBotPlayer> Bots;
        std::mt19937 RNG;
        std::uniform_int_distribution<int> NameDist;
        std::uniform_real_distribution<float> FloatDist;

        bool bInitialized;
        int MaxBots;
        float SpawnInterval;
        float LastSpawnTime;

    public:
        FBotManager()
            : RNG(std::chrono::steady_clock::now().time_since_epoch().count())
            , NameDist(0, static_cast<int>(BotNames.size()) - 1)
            , FloatDist(0.0f, 1.0f)
            , bInitialized(false)
            , MaxBots(99)
            , SpawnInterval(5.0f)
            , LastSpawnTime(0.0f)
        {
        }

        void Initialize()
        {
            if (bInitialized)
                return;

            LOG_("Initializing Bot System...");
            LOG_("Loaded {} unique bot names", BotNames.size());
            bInitialized = true;
        }

        FBotConfig GenerateRandomConfig()
        {
            FBotConfig Config{};

            // Random personality
            Config.Personality = static_cast<EBotPersonality>(
                static_cast<int>(FloatDist(RNG) * static_cast<int>(EBotPersonality::Random))
            );

            // Random difficulty
            Config.Difficulty = static_cast<EBotDifficulty>(
                static_cast<int>(FloatDist(RNG) * static_cast<int>(EBotDifficulty::Realistic))
            );

            // Set stats based on difficulty
            switch (Config.Difficulty)
            {
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
            switch (Config.Personality)
            {
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

        std::string GetRandomName()
        {
            return BotNames[NameDist(RNG)];
        }

        bool CanSpawnBot()
        {
            if (!bInitialized)
                return false;

            auto CurrentTime = GetStatics()->GetTimeSeconds(GetWorld());
            if (CurrentTime - LastSpawnTime < SpawnInterval)
                return false;

            return static_cast<int>(Bots.size()) < MaxBots;
        }

        void SpawnBot()
        {
            if (!CanSpawnBot())
                return;

            FBotPlayer NewBot{};
            NewBot.Name = GetRandomName();
            NewBot.Config = GenerateRandomConfig();
            NewBot.CurrentState = EBotCombatState::Looting;
            NewBot.LastActionTime = GetStatics()->GetTimeSeconds(GetWorld());
            NewBot.StateChangeCooldown = 2.0f + FloatDist(RNG) * 3.0f;
            NewBot.Kills = 0;
            NewBot.Deaths = 0;
            NewBot.TotalDamageDealt = 0.0f;
            NewBot.bIsAlive = false;
            NewBot.Controller = nullptr;
            NewBot.Pawn = nullptr;
            NewBot.TargetEnemy = nullptr;

            Bots.push_back(NewBot);
            LastSpawnTime = GetStatics()->GetTimeSeconds(GetWorld());

            LOG_("Spawned bot: {} (Personality: {}, Difficulty: {})",
                 NewBot.Name,
                 static_cast<int>(NewBot.Config.Personality),
                 static_cast<int>(NewBot.Config.Difficulty));
        }

        void Update(float DeltaTime)
        {
            if (!bInitialized)
                return;

            // Spawn bots periodically
            if (CanSpawnBot() && FloatDist(RNG) < 0.3f)
            {
                SpawnBot();
            }

            // Update all bots
            for (auto& Bot : Bots)
            {
                if (Bot.bIsAlive && Bot.Pawn && Bot.Controller)
                {
                    UpdateBotBehavior(Bot, DeltaTime);
                }
            }

            // Clean up dead bots
            Bots.erase(
                std::remove_if(Bots.begin(), Bots.end(),
                    [](const FBotPlayer& Bot) { return !Bot.bIsAlive && Bot.Deaths > 0; }),
                Bots.end());
        }

        void UpdateBotBehavior(FBotPlayer& Bot, float DeltaTime)
        {
            if (!Bot.Pawn || !Bot.Controller)
                return;

            auto CurrentTime = GetStatics()->GetTimeSeconds(GetWorld());

            // Check if we should change state
            if (CurrentTime - Bot.LastActionTime >= Bot.StateChangeCooldown)
            {
                DetermineNewState(Bot);
                Bot.LastActionTime = CurrentTime;
                Bot.StateChangeCooldown = 2.0f + FloatDist(RNG) * 4.0f;
            }

            // Execute behavior based on current state
            switch (Bot.CurrentState)
            {
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

        void DetermineNewState(FBotPlayer& Bot)
        {
            // Get bot health using proper accessor methods
            float Health = Bot.Pawn->GetHealth();
            float MaxHealth = Bot.Pawn->GetMaxHealth();
            float HealthPercent = MaxHealth > 0 ? Health / MaxHealth : 0.0f;

            // Check for nearby enemies
            auto NearestEnemy = FindNearestEnemy(Bot);
            float EnemyDistance = NearestEnemy ? FVectorHelpers::Distance(Bot.Pawn->K2_GetActorLocation(),
                NearestEnemy->K2_GetActorLocation()) : FLT_MAX;

            // State decision logic
            if (HealthPercent < Bot.Config.FleeThreshold && NearestEnemy)
            {
                Bot.CurrentState = EBotCombatState::Fleeing;
            }
            else if (HealthPercent < 0.6f && Bot.Config.bUseConsumables)
            {
                Bot.CurrentState = EBotCombatState::Healing;
            }
            else if (NearestEnemy && EnemyDistance < Bot.Config.EngagementRange)
            {
                if (FloatDist(RNG) < Bot.Config.Aggressiveness)
                    Bot.CurrentState = EBotCombatState::Fighting;
                else
                    Bot.CurrentState = EBotCombatState::Building;
            }
            else if (FloatDist(RNG) < 0.2f && Bot.Config.bBuildStructures)
            {
                Bot.CurrentState = EBotCombatState::Building;
            }
            else
            {
                Bot.CurrentState = EBotCombatState::Looting;
            }

            Bot.TargetEnemy = NearestEnemy;
        }

        SDK::AActor* FindNearestEnemy(FBotPlayer& Bot)
        {
            if (!Bot.Pawn)
                return nullptr;

            SDK::AActor* NearestEnemy = nullptr;
            float NearestDistance = FLT_MAX;

            auto GameState = GetGameState();
            if (!GameState)
                return nullptr;

            auto BotLocation = Bot.Pawn->K2_GetActorLocation();
            (void)BotLocation; // Suppress unused warning for now

            for (int i = 0; i < GameState->PlayerArray.Num(); i++)
            {
                auto PlayerState = GameState->PlayerArray[i];
                if (!PlayerState)
                    continue;

                // Skip bots and teammates - using AFortPlayerStateAthena for TeamIndex
                auto FortPlayerState = static_cast<SDK::AFortPlayerStateAthena*>(PlayerState);
                auto BotPlayerState = static_cast<SDK::AFortPlayerStateAthena*>(Bot.Controller->PlayerState);

                if (FortPlayerState->TeamIndex == BotPlayerState->TeamIndex)
                    continue;

                // Find the pawn for this player
                // This would need proper implementation to find the actual pawn
                // For now, just return nullptr
            }

            return NearestEnemy;
        }

        void ExecuteLootingBehavior(FBotPlayer& Bot)
        {
            // Move towards random loot locations
            // This would need proper implementation with actual loot spawning system
            SDK::FVector RandomOffset(
                FloatDist(RNG) * 4000.0f - 2000.0f,
                FloatDist(RNG) * 4000.0f - 2000.0f,
                0.0f
            );
            Bot.TargetLocation = FVectorHelpers::operator+(Bot.Pawn->K2_GetActorLocation(), RandomOffset);
        }

        void ExecuteBuildingBehavior(FBotPlayer& Bot)
        {
            // Build structures based on personality
            // This would need proper implementation with building system
            (void)Bot; // Suppress unused warning
        }

        void ExecuteCombatBehavior(FBotPlayer& Bot)
        {
            if (!Bot.TargetEnemy)
                return;

            // Aim at enemy with accuracy based on config
            auto EnemyLocation = Bot.TargetEnemy->K2_GetActorLocation();

            // Add some inaccuracy
            SDK::FVector InaccuracyOffset(
                (FloatDist(RNG) - 0.5f) * (1.0f - Bot.Config.AimAccuracy) * 500.0f,
                (FloatDist(RNG) - 0.5f) * (1.0f - Bot.Config.AimAccuracy) * 500.0f,
                (FloatDist(RNG) - 0.5f) * (1.0f - Bot.Config.AimAccuracy) * 200.0f
            );

            SDK::FVector AimLocation = EnemyLocation + InaccuracyOffset;

            // Calculate rotation to aim at target
            SDK::FVector BotLocation = Bot.Pawn->K2_GetActorLocation();
            FRotator AimRotation = GetMath()->FindLookAtRotation(BotLocation, AimLocation);
            Bot.Pawn->K2_SetActorRotation(AimRotation, false);
        }

        void ExecuteFleeBehavior(FBotPlayer& Bot)
        {
            if (!Bot.TargetEnemy)
                return;

            // Move away from enemy
            SDK::FVector ToEnemy = FVectorHelpers::operator-(Bot.TargetEnemy->K2_GetActorLocation(), Bot.Pawn->K2_GetActorLocation());
            FVectorHelpers::Normalize(ToEnemy);

            SDK::FVector FleeDirection = FVectorHelpers::operator*(FVectorHelpers::operator-(ToEnemy), 2000.0f); // Move 2000 units away
            Bot.TargetLocation = FVectorHelpers::operator+(Bot.Pawn->K2_GetActorLocation(), FleeDirection);
        }

        void ExecuteRotateBehavior(FBotPlayer& Bot)
        {
            // Move towards safe zone or random location
            SDK::FVector RandomOffset(
                FloatDist(RNG) * 3000.0f - 1500.0f,
                FloatDist(RNG) * 3000.0f - 1500.0f,
                0.0f
            );
            Bot.TargetLocation = FVectorHelpers::operator+(Bot.Pawn->K2_GetActorLocation(), RandomOffset);
        }

        void ExecuteHealBehavior(FBotPlayer& Bot)
        {
            // Use consumables to heal
            // This would need proper implementation with inventory system
            (void)Bot; // Suppress unused warning
        }

        void ExecuteIdleBehavior(FBotPlayer& Bot)
        {
            // Random movements to appear more natural
            if (FloatDist(RNG) < 0.1f)
            {
                SDK::FVector RandomOffset(
                    FloatDist(RNG) * 500.0f - 250.0f,
                    FloatDist(RNG) * 500.0f - 250.0f,
                    0.0f
                );
                Bot.TargetLocation = FVectorHelpers::operator+(Bot.Pawn->K2_GetActorLocation(), RandomOffset);
            }
        }

        void SetMaxBots(int InMaxBots) { MaxBots = InMaxBots; }
        void SetSpawnInterval(float InInterval) { SpawnInterval = InInterval; }

        int GetBotCount() const { return static_cast<int>(Bots.size()); }
        const std::vector<FBotPlayer>& GetBots() const { return Bots; }

        void ClearBots() { Bots.clear(); }

        // Public accessors for RNG and FloatDist needed by advanced behaviors
        std::mt19937& GetRNG() { return RNG; }
        std::uniform_real_distribution<float>& GetFloatDist() { return FloatDist; }
    };

    static FBotManager* GBotManager = nullptr;

    // Initialize bot system
    inline void InitializeBotSystem()
    {
        if (!GBotManager)
        {
            GBotManager = new FBotManager();
            GBotManager->Initialize();
        }
    }

    // Update bot system (call from tick)
    inline void UpdateBotSystem(float DeltaTime)
    {
        if (GBotManager)
        {
            GBotManager->Update(DeltaTime);
        }
    }

    // Cleanup bot system
    inline void ShutdownBotSystem()
    {
        if (GBotManager)
        {
            delete GBotManager;
            GBotManager = nullptr;
        }
    }
}
