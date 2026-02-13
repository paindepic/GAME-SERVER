#pragma once
#include "framework.h"
#include "BotSystem.h"
#include <cmath>

namespace BotSystem
{
    // Advanced Building System for Bots
    class FBotBuildingSystem
    {
    public:
        enum class EBuildType
        {
            Wall,
            Floor,
            Stairs,
            Roof,
            Pyramid,
            Tunnel,
            BoxFight,
            NinetySeconds,  // Renamed from "90s" which is invalid identifier
            CrashPad
        };

        struct FBuildRequest
        {
            SDK::FVector Location;
            FRotator Rotation;
            EBuildType Type;
            bool bImmediate;
            int Priority;
        };

        static void ExecuteBuild(FBotPlayer& Bot, const FBuildRequest& Request)
        {
            if (!Bot.Pawn || !Bot.Config.bBuildStructures)
                return;

            // Building would be implemented here using the actual building system
            // This is a placeholder for the actual building logic

            LOG_("Bot {} attempting to build at location", Bot.Name);
            (void)Request; // Suppress unused warning
        }

        static void BuildQuickBox(FBotPlayer& Bot, const SDK::FVector& Center)
        {
            // Build 4 walls around the bot
            std::vector<SDK::FVector> WallLocations = {
                FVectorHelpers::operator+(Center, SDK::FVector{300.0f, 0.0f, 0.0f}),
                FVectorHelpers::operator+(Center, SDK::FVector{-300.0f, 0.0f, 0.0f}),
                FVectorHelpers::operator+(Center, SDK::FVector{0.0f, 300.0f, 0.0f}),
                FVectorHelpers::operator+(Center, SDK::FVector{0.0f, -300.0f, 0.0f})
            };

            for (const auto& Loc : WallLocations)
            {
                FBuildRequest Request{};
                Request.Location = Loc;
                Request.Type = EBuildType::Wall;
                Request.bImmediate = true;
                Request.Priority = 10;
                ExecuteBuild(Bot, Request);
            }
        }

        static void BuildRampPush(FBotPlayer& Bot, const SDK::FVector& Target)
        {
            SDK::FVector Direction = FVectorHelpers::operator-(Target, Bot.Pawn->K2_GetActorLocation());
            Direction.Z = 0;
            FVectorHelpers::Normalize(Direction);

            SDK::FVector RampLocation = FVectorHelpers::operator+(Bot.Pawn->K2_GetActorLocation(), FVectorHelpers::operator*(Direction, 400.0f));

            FBuildRequest Request{};
            Request.Location = RampLocation;
            Request.Type = EBuildType::Stairs;
            Request.bImmediate = true;
            Request.Priority = 8;

            ExecuteBuild(Bot, Request);
        }

        static void BuildDefensiveFort(FBotPlayer& Bot)
        {
            auto Center = Bot.Pawn->K2_GetActorLocation();

            // Build a larger defensive structure
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    SDK::FVector WallLoc = Center;
                    if (j == 0) WallLoc.X += 300.0f * (i + 1);
                    else if (j == 1) WallLoc.X -= 300.0f * (i + 1);
                    else if (j == 2) WallLoc.Y += 300.0f * (i + 1);
                    else WallLoc.Y -= 300.0f * (i + 1);

                    FBuildRequest Request{};
                    Request.Location = WallLoc;
                    Request.Type = EBuildType::Wall;
                    Request.bImmediate = false;
                    Request.Priority = 7 - i;

                    ExecuteBuild(Bot, Request);
                }
            }
        }
    };

    // Advanced Combat System for Bots
    class FBotCombatSystem
    {
    public:
        enum class ECombatStyle
        {
            ShotgunRush,
            ARPeek,
            Snipe,
            Spray,
            EditTakes,
            BoxFight,
            HighGround
        };

        struct FCombatDecision
        {
            bool bShouldAttack;
            bool bShouldBuild;
            bool bShouldRetreat;
            SDK::FVector AimPosition;
            bool bShouldReload;
            bool bShouldSwapWeapon;
        };

        static FCombatDecision AnalyzeCombatSituation(FBotPlayer& Bot)
        {
            FCombatDecision Decision{};

            if (!Bot.TargetEnemy || !Bot.Pawn)
            {
                Decision.bShouldAttack = false;
                Decision.bShouldBuild = false;
                Decision.bShouldRetreat = false;
                return Decision;
            }

            auto EnemyLocation = Bot.TargetEnemy->K2_GetActorLocation();
            auto BotLocation = Bot.Pawn->K2_GetActorLocation();
            float Distance = FVectorHelpers::Distance(BotLocation, EnemyLocation);

            // Calculate aim position with prediction
            Decision.AimPosition = PredictEnemyPosition(Bot, Bot.TargetEnemy);

            // Decide based on distance and personality
            float BotHealth = Bot.Pawn->GetHealth();
            float BotMaxHealth = Bot.Pawn->GetMaxHealth();
            if (Distance < 500.0f && Bot.Config.Aggressiveness > 0.6f)
            {
                // Shotgun range - aggressive
                Decision.bShouldAttack = true;
                Decision.bShouldBuild = Bot.GetPersonality() == EBotPersonality::Builder;
                Decision.bShouldRetreat = BotHealth < BotMaxHealth * 0.3f;
            }
            else if (Distance < 1500.0f)
            {
                // AR range
                Decision.bShouldAttack = Bot.Config.Aggressiveness > 0.3f;
                Decision.bShouldBuild = true;
                Decision.bShouldRetreat = BotHealth < BotMaxHealth * 0.4f;
            }
            else if (Distance > 3000.0f)
            {
                // Long range
                Decision.bShouldAttack = Bot.Config.Personality == EBotPersonality::Sniper;
                Decision.bShouldBuild = false;
                Decision.bShouldRetreat = true;
            }
            else
            {
                Decision.bShouldAttack = true;
                Decision.bShouldBuild = GBotManager->GetFloatDist()(GBotManager->GetRNG()) < 0.5f;
                Decision.bShouldRetreat = false;
            }

            return Decision;
        }

        static SDK::FVector PredictEnemyPosition(FBotPlayer& Bot, SDK::AActor* Enemy)
        {
            if (!Enemy)
                return SDK::FVector{};

            auto EnemyLocation = Enemy->K2_GetActorLocation();
            auto BotLocation = Bot.Pawn->K2_GetActorLocation();

            // Calculate bullet travel time (approximate)
            float BulletSpeed = 30000.0f; // Default AR bullet speed
            float Distance = FVectorHelpers::Distance(BotLocation, EnemyLocation);
            float TravelTime = Distance / BulletSpeed;

            // Predict enemy movement (simple linear prediction)
            // In reality, this would need velocity information
            SDK::FVector PredictedOffset(
                (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * 200.0f * TravelTime,
                (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * 200.0f * TravelTime,
                (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * 50.0f * TravelTime
            );

            return EnemyLocation + PredictedOffset;
        }

        static void ExecuteCombat(FBotPlayer& Bot, const FCombatDecision& Decision)
        {
            if (!Bot.Pawn || !Bot.Controller)
                return;

            if (Decision.bShouldAttack && Bot.TargetEnemy)
            {
                // Aim at predicted position with accuracy
                SDK::FVector AimPos = Decision.AimPosition;

                // Add inaccuracy based on config
                float Inaccuracy = (1.0f - Bot.Config.AimAccuracy) * 300.0f;
                AimPos.X += (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * Inaccuracy;
                AimPos.Y += (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * Inaccuracy;
                AimPos.Z += (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * Inaccuracy * 0.3f;

                // FindLookAtRotation expects non-const references
                SDK::FVector BotLocation = Bot.Pawn->K2_GetActorLocation();
                FRotator AimRot = GetMath()->FindLookAtRotation(
                    BotLocation,
                    AimPos
                );

                // Smooth rotation for natural feel
                float RotationSpeed = 5.0f + static_cast<float>(static_cast<int>(Bot.Config.Difficulty)) * 5.0f;
                auto CurrentRot = Bot.Pawn->K2_GetActorRotation();
                AimRot = GetMath()->RInterpTo(CurrentRot, AimRot, 0.016f, RotationSpeed);

                Bot.Pawn->K2_SetActorRotation(AimRot, false);

                // Fire weapon (would need actual weapon system integration)
                LOG_("Bot {} firing at target", Bot.Name);
            }

            if (Decision.bShouldBuild)
            {
                // Build cover
                FBotBuildingSystem::BuildQuickBox(Bot, Bot.Pawn->K2_GetActorLocation());
            }
        }

        static void ExecuteEditTakes(FBotPlayer& Bot, SDK::AActor* TargetStructure)
        {
            if (!Bot.Pawn || !TargetStructure)
                return;

            // Edit takes logic - replace pieces quickly during combat
            // This would need actual editing system integration

            LOG_("Bot {} attempting edit takes", Bot.Name);
        }
    };

    // Advanced Looting System for Bots
    class FBotLootingSystem
    {
    public:
        struct FLootTarget
        {
            SDK::AActor* LootActor;
            SDK::FVector Location;
            float Priority;
            SDK::UFortItemDefinition* ItemDef;
        };

        static FLootTarget FindBestLoot(FBotPlayer& Bot, float SearchRadius)
        {
            FLootTarget BestTarget{};
            BestTarget.LootActor = nullptr;
            BestTarget.Priority = -1.0f;

            if (!Bot.Pawn)
                return BestTarget;

            auto BotLocation = Bot.Pawn->K2_GetActorLocation();
            (void)BotLocation; // Suppress unused warning
            (void)SearchRadius; // Suppress unused warning

            // Search for nearby loot
            // This would need actual loot scanning implementation
            // For now, return empty target

            return BestTarget;
        }

        static void MoveToLoot(FBotPlayer& Bot, const FLootTarget& Target)
        {
            if (!Bot.Pawn || !Target.LootActor)
                return;

            // Move towards loot
            // This would need actual movement system integration

            LOG_("Bot {} moving to loot", Bot.Name);
        }

        static float CalculateItemPriority(FBotPlayer& Bot, SDK::UFortItemDefinition* Item)
        {
            if (!Item)
                return 0.0f;

            // Calculate priority based on bot personality and needs
            float Priority = 0.5f;

            // Adjust based on personality
            switch (Bot.Config.Personality)
            {
                case EBotPersonality::Aggressive:
                    // Prefer shotguns and ARs
                    break;
                case EBotPersonality::Sniper:
                    // Prefer snipers
                    break;
                case EBotPersonality::Builder:
                    // Prefer mats and build weapons
                    break;
                case EBotPersonality::Looter:
                    // High priority for all loot
                    Priority += 0.3f;
                    break;
                default:
                    break;
            }

            return Priority;
        }

        static void SmartLoot(FBotPlayer& Bot)
        {
            if (!Bot.Pawn)
                return;

            // Find and move to best loot
            auto Target = FindBestLoot(Bot, 3000.0f);

            if (Target.LootActor)
            {
                MoveToLoot(Bot, Target);
            }
            else
            {
                // No loot nearby, rotate to new area
                Bot.CurrentState = EBotCombatState::Rotating;
            }
        }
    };

    // Advanced Movement System for Bots
    class FBotMovementSystem
    {
    public:
        enum class EMovementStyle
        {
            Walking,
            Running,
        };

        static void MoveToLocation(FBotPlayer& Bot, const SDK::FVector& Target, bool bSprint = true)
        {
            if (!Bot.Pawn || !Bot.Controller)
                return;

            auto CurrentLocation = Bot.Pawn->K2_GetActorLocation();
            SDK::FVector Direction = FVectorHelpers::operator-(Target, CurrentLocation);
            float Distance = std::sqrt(Direction.X * Direction.X + Direction.Y * Direction.Y + Direction.Z * Direction.Z);

            if (Distance < 50.0f)
                return; // Close enough

            FVectorHelpers::Normalize(Direction);

            // Add some randomness for natural movement
            SDK::FVector Noise(
                (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * 50.0f,
                (GBotManager->GetFloatDist()(GBotManager->GetRNG()) - 0.5f) * 50.0f,
                0.0f
            );

            SDK::FVector MoveDirection = Direction * 1.0f + Noise;
            FVectorHelpers::Normalize(MoveDirection);

            // Calculate rotation to face movement direction
            FRotator MoveRotation = GetMath()->FindLookAtRotation(CurrentLocation, Target);

            // Smooth rotation
            auto CurrentRot = Bot.Pawn->K2_GetActorRotation();
            float RotationSpeed = 10.0f;
            MoveRotation = GetMath()->RInterpTo(CurrentRot, MoveRotation, 0.016f, RotationSpeed);

            Bot.Pawn->K2_SetActorRotation(MoveRotation, false);

            // Move input (would need actual input system)
            // Bot.Controller->AddMovementInput(MoveDirection, 1.0f);
            (void)bSprint; // Suppress unused warning
        }

        static void StrafeMovement(FBotPlayer& Bot, const SDK::FVector& Target, float StrafeAmount)
        {
            if (!Bot.Pawn)
                return;

            auto CurrentLocation = Bot.Pawn->K2_GetActorLocation();
            SDK::FVector ToTarget = FVectorHelpers::operator-(Target, CurrentLocation);
            ToTarget.Z = 0;
            FVectorHelpers::Normalize(ToTarget);

            // Calculate perpendicular direction for strafing
            SDK::FVector StrafeDir(-ToTarget.Y, ToTarget.X, 0.0f);

            if (StrafeAmount != 0.0f)
            {
                SDK::FVector MoveDirection = FVectorHelpers::operator*(StrafeDir, StrafeAmount);
                // Bot.Controller->AddMovementInput(MoveDirection, 1.0f);
                (void)MoveDirection; // Suppress unused warning
            }
        }

        static void JumpToLocation(FBotPlayer& Bot, const SDK::FVector& Target)
        {
            if (!Bot.Pawn)
                return;

            auto CurrentLocation = Bot.Pawn->K2_GetActorLocation();
            float Distance = FVectorHelpers::Distance(CurrentLocation, Target);

            // Calculate jump trajectory
            float HeightDifference = Target.Z - CurrentLocation.Z;
            float HorizontalDistance = std::sqrt(
                std::pow(Target.X - CurrentLocation.X, 2) +
                std::pow(Target.Y - CurrentLocation.Y, 2)
            );

            // Simple jump if close
            if (HorizontalDistance < 500.0f && HeightDifference < 300.0f)
            {
                // Bot.Pawn->Jump();
            }
        }

        static void FollowPlayer(FBotPlayer& Bot, SDK::AActor* Player, float FollowDistance)
        {
            if (!Bot.Pawn || !Player)
                return;

            auto PlayerLocation = Player->K2_GetActorLocation();
            auto BotLocation = Bot.Pawn->K2_GetActorLocation();

            SDK::FVector ToPlayer = FVectorHelpers::operator-(PlayerLocation, BotLocation);
            float Distance = std::sqrt(ToPlayer.X * ToPlayer.X + ToPlayer.Y * ToPlayer.Y + ToPlayer.Z * ToPlayer.Z);

            if (Distance > FollowDistance)
            {
                FVectorHelpers::Normalize(ToPlayer);
                SDK::FVector TargetPos = FVectorHelpers::operator-(PlayerLocation, FVectorHelpers::operator*(ToPlayer, FollowDistance));
                MoveToLocation(Bot, TargetPos);
            }
        }

        static void RotateAroundPoint(FBotPlayer& Bot, const SDK::FVector& Center, float Radius, float Speed)
        {
            static float Angle = 0.0f;
            Angle += Speed * 0.016f;

            SDK::FVector TargetPos{};
            TargetPos.X = Center.X + std::cos(Angle) * Radius;
            TargetPos.Y = Center.Y + std::sin(Angle) * Radius;
            TargetPos.Z = Bot.Pawn->K2_GetActorLocation().Z;

            MoveToLocation(Bot, TargetPos);
        }
    };

    // Advanced Decision System for Bots
    class FBotDecisionSystem
    {
    public:
        struct FSituationAssessment
        {
            bool bInCombat;
            bool bHasHealthAdvantage;
            bool bHasWeaponAdvantage;
            bool bHasPositionAdvantage;
            bool bInSafeZone;
            float EnemyCount;
            float AllyCount;
            float LootQuality;
            float DangerLevel;
        };

        static FSituationAssessment AssessSituation(FBotPlayer& Bot)
        {
            FSituationAssessment Assessment{};

            // Assess current situation
            // This would need proper implementation with game state
            (void)Bot; // Suppress unused warning

            return Assessment;
        }

        static void MakeStrategicDecision(FBotPlayer& Bot)
        {
            auto Situation = AssessSituation(Bot);

            if (Situation.DangerLevel > 0.7f)
            {
                // High danger - be defensive
                Bot.CurrentState = EBotCombatState::Building;
            }
            else if (Situation.bInCombat)
            {
                // In combat - fight or build
                if (Bot.Config.Aggressiveness > 0.6f)
                    Bot.CurrentState = EBotCombatState::Fighting;
                else
                    Bot.CurrentState = EBotCombatState::Building;
            }
            else if (!Situation.bInSafeZone)
            {
                // Outside safe zone - rotate
                Bot.CurrentState = EBotCombatState::Rotating;
            }
            else
            {
                // Safe and not in combat - loot
                Bot.CurrentState = EBotCombatState::Looting;
            }
        }
    };
}
