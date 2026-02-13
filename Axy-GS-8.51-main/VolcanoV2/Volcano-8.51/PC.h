#pragma once
#include "Abilites.h"
#include "Inventory.h"


#include <windows.h>
#include <tlhelp32.h>
#include <string>


#include <windows.h>
#include <cstring>


static void* (*sub_7FF6B99CFE30)(void*, void*) = decltype(sub_7FF6B99CFE30)(GetOffsetBRUH(0x175FE30));
void ServerAcknowledgePossessionHook(SDK::AFortPlayerController* PC, SDK::APawn* P)
{
	PC->AcknowledgedPawn = P;

	auto PlayerState = (SDK::AFortPlayerStateAthena*)PC->PlayerState;
	auto FortPawn = PC->MyFortPawn;
	if (PlayerState && FortPawn)
	{
		auto& CosmeticLoadout = PC->CosmeticLoadoutPC;
		if (CosmeticLoadout.Character)
		{
			if (CosmeticLoadout.Character->HeroDefinition)
			{
				FortPawn->CosmeticLoadout = CosmeticLoadout;
				FortPawn->OnRep_CosmeticLoadout();

				PlayerState->HeroType = CosmeticLoadout.Character->HeroDefinition;
				PlayerState->OnRep_HeroType(); // yay

				sub_7FF6B99CFE30(PlayerState, FortPawn); // test idk
			}
		}
	}
}

// TODO: check the original
void (*ServerReadyToStartMatchOG)(SDK::AController*);
void ServerReadyToStartMatchHook(SDK::AFortPlayerController* PC)
{
	if (PC)
	{
		auto PlayerState = (SDK::AFortPlayerStateAthena*)PC->PlayerState;
		if (PlayerState)
		{
			GrantAbilitySet(PlayerState);
		}

		  // Inventory::Setup(PC); // YOU FKN STUPID NAX!

		for (int i = 0; i < GetGameMode()->StartingItems.Num(); i++)
		{
			Inventory::AddItem(PC, GetGameMode()->StartingItems[i].Item, GetGameMode()->StartingItems[i].Count);
		}

		static auto YAYA = SDK::UObject::FindObject<SDK::UAthenaPickaxeItemDefinition>("DefaultPickaxe.DefaultPickaxe");
		auto& test = PC->CosmeticLoadoutPC;
		if (test.Pickaxe)
		{
			LOG_("valid picakxe!!!");
			LOG_("aa: {}", test.Pickaxe->GetName());
		}

		Inventory::AddItem(PC, test.Pickaxe ? test.Pickaxe->WeaponDefinition : YAYA->WeaponDefinition, 1);
		/*
		static auto PUMP = UObject::FindObject<UFortItemDefinition>("WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03");
		static auto AR = UObject::FindObject<UFortItemDefinition>("WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03");
		static auto Grap = UObject::FindObject<UFortItemDefinition>("WID_Hook_Gun_VR_Ore_T03.WID_Hook_Gun_VR_Ore_T03");

		static auto aaa = UObject::FindObject<UFortItemDefinition>("WID_Sniper_Heavy_Athena_VR_Ore_T03.WID_Sniper_Heavy_Athena_VR_Ore_T03");
		static auto DRUMGUN = UObject::FindObject<UFortItemDefinition>("WID_Assault_AutoDrum_Athena_R_Ore_T03.WID_Assault_AutoDrum_Athena_R_Ore_T03");
		static auto bbbc = UObject::FindObject<UFortItemDefinition>("WID_Pistol_Flintlock_Athena_UC.WID_Pistol_Flintlock_Athena_UC");

		Inventory::AddItem(PC, bbbc, 1, 5);
		Inventory::AddItem(PC, aaa, 1, 30);
		Inventory::AddItem(PC, DRUMGUN, 1, 5);
		Inventory::AddItem(PC, AR, 1, 30);
		Inventory::AddItem(PC, PUMP, 1, 5);
		Inventory::AddItem(PC, ((UFortWeaponItemDefinition*)aaa)->GetAmmoWorldItemDefinition_BP(), 9999);
		Inventory::AddItem(PC, ((UFortWeaponItemDefinition*)DRUMGUN)->GetAmmoWorldItemDefinition_BP(), 9999);
		Inventory::AddItem(PC, ((UFortWeaponItemDefinition*)AR)->GetAmmoWorldItemDefinition_BP(), 9999);
		Inventory::AddItem(PC, ((UFortWeaponItemDefinition*)PUMP)->GetAmmoWorldItemDefinition_BP(), 9999);
		Inventory::AddItem(PC, Grap, 1, 10);*/
		
		//Inventory::AddItem(PC, shells, 30);
		if (Globals::bPlayground)
		{
			PC->bInfiniteAmmo = true;
			PC->bBuildFree = true;
			PC->bInfiniteMagazine = true; // TEST
		}
		
		// TODO: GameplayModifiers implementatipons */

		LOG_("TeamIndex: {}", PlayerState->TeamIndex); // pickteam nvm im high
		PlayerState->SquadId = PlayerState->TeamIndex - 2;
		PlayerState->OnRep_PlayerTeam();
		PlayerState->OnRep_PlayerTeamPrivate();
		PlayerState->OnRep_TeamIndex(0);
		PlayerState->OnRep_SquadId();

		/*FGameMemberInfo test{-1,-1,-1};  // DONT USE IT SKIDDAS
		test.TeamIndex = PlayerState->TeamIndex;
		test.SquadId = PlayerState->SquadId;
		test.MemberUniqueId = PlayerState->UniqueId;

		GetGameState()->GameMemberInfoArray.Members.Add(test);*/
		GetGameState()->GameMemberInfoArray.MarkArrayDirty();

		static bool bTest = false;
		if (!bTest)
		{
			// GetGameState()->TotalPlayers++;
			// AFortPlayerController::OnReadyToStartMatch: Marking player MCP:<Redacted> ready to start
			// ^ heres where this stuff stops, gamemode doesn't return true at ReadyToStartMatch, no idea why yet
		}
	}

	return ServerReadyToStartMatchOG(PC);
}

void ServerExecuteInventoryItem(SDK::AFortPlayerController* PC, SDK::FGuid& ItemGuid)
{
	if (auto Pawn = (SDK::AFortPlayerPawn*)PC->Pawn)
	{
		if (auto ItemEntry = Inventory::FindItemEntry(PC, ItemGuid))
		{
			Pawn->EquipWeaponDefinition((SDK::UFortWeaponItemDefinition*)ItemEntry->ItemDefinition, ItemEntry->ItemGuid);
		}
	}
}

static bool (*CantBuild)(SDK::UWorld*, SDK::UObject*, SDK::FVector, SDK::FRotator, char, void*, char*) = decltype(CantBuild)(GetOffsetBRUH(0x1330D70)); // 0x1330D70
void ServerCreateBuildingActorHook(SDK::AFortPlayerControllerAthena* PC, SDK::FCreateBuildingActorData& CreateBuildingData)
{
	// auto Class = PC->BroadcastRemoteClientInfo->RemoteBuildableClass.Get(); // 0x28D8
	auto Class = (*(SDK::AFortBroadcastRemoteClientInfo**)(__int64(PC) + 0x28D8))->RemoteBuildableClass.Get();
	SDK::TArray<SDK::AActor*> BuildingActorsToDestroy;
	char Result;
	if (!CantBuild(GetWorld(), Class, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &BuildingActorsToDestroy, &Result))
	{
		for (int i = 0; i < BuildingActorsToDestroy.Num(); i++)
		{
			BuildingActorsToDestroy[i]->K2_DestroyActor();
		}
		BuildingActorsToDestroy.Free();

		if (auto NewBuilding = SpawnActor<SDK::ABuildingSMActor>(Class, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot))
		{
			NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, PC, true);
			NewBuilding->bPlayerPlaced = true;
			// *(uint8*)(__int64(NewBuilding) + 0x403) = ((AFortPlayerStateAthena*)PC->PlayerState)->TeamIndex;
			NewBuilding->Team = SDK::EFortTeam(((SDK::AFortPlayerStateAthena*)PC->PlayerState)->TeamIndex);
			NewBuilding->TeamIndex = ((SDK::AFortPlayerStateAthena*)PC->PlayerState)->TeamIndex;
			NewBuilding->OnRep_Team();

			if(!PC->bBuildFree)
				Inventory::RemoveItem(PC, GetFortKismet()->K2_GetResourceItemDefinition(NewBuilding->ResourceType), 10);
		}
	}
}

void ServerBeginEditingBuildingActorHook(SDK::AFortPlayerController* PC, SDK::ABuildingSMActor* BuildingActorToEdit)
{
	auto Pawn = (SDK::AFortPlayerPawnAthena*)PC->Pawn;
	if (Pawn && BuildingActorToEdit)
	{
		static auto EditToolDef = StaticFindObject<SDK::UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
		if (Pawn->CurrentWeapon->WeaponData != EditToolDef)
		{
			if (auto EditToolEntry = Inventory::FindItemEntry(PC, EditToolDef))
				PC->ServerExecuteInventoryItem(EditToolEntry->ItemGuid);
		}

		auto EditTool = (SDK::AFortWeap_EditingTool*)Pawn->CurrentWeapon;
		EditTool->EditActor = BuildingActorToEdit;
		EditTool->OnRep_EditActor();
		BuildingActorToEdit->EditingPlayer = (SDK::AFortPlayerStateAthena*)PC->PlayerState;
		BuildingActorToEdit->OnRep_EditingPlayer();
	}
}

// Idk about this offset theres 2 refs again but I think its the right one
static SDK::ABuildingSMActor* (*ReplaceBuildingActorOG)(SDK::ABuildingSMActor*, char, SDK::UClass*, int, uint8, bool, SDK::AController*) = decltype(ReplaceBuildingActorOG)(GetOffsetBRUH(0x11252B0));
void ServerEditBuildingActorHook(SDK::AFortPlayerController* PC, SDK::ABuildingSMActor* BuildingActorToEdit, SDK::UClass* NewBuildingClass, uint8 RotationIterations, bool bMirrored)
{
	if (PC && BuildingActorToEdit && NewBuildingClass)
	{
		if (auto NewBuilding = ReplaceBuildingActorOG(BuildingActorToEdit, 1, NewBuildingClass, BuildingActorToEdit->CurrentBuildingLevel, RotationIterations, bMirrored, PC))
		{
			NewBuilding->bPlayerPlaced = true;

			BuildingActorToEdit->EditingPlayer = nullptr;
			BuildingActorToEdit->OnRep_EditingPlayer();
		}
	}
}

void ServerEndEditingBuildingActorHook(SDK::AFortPlayerController* PC, SDK::ABuildingSMActor* BuildingActorToStopEditing)
{
	if (PC && PC->Pawn && BuildingActorToStopEditing)
	{
		BuildingActorToStopEditing->EditingPlayer = nullptr;
		BuildingActorToStopEditing->OnRep_EditingPlayer();

		SDK::AFortWeap_EditingTool* EditTool = (SDK::AFortWeap_EditingTool*)((SDK::APlayerPawn_Athena_C*)PC->Pawn)->CurrentWeapon;
		if (EditTool)
		{
			EditTool->bEditConfirmed = true;
			EditTool->EditActor = nullptr;
			EditTool->OnRep_EditActor();
		}
	}
}

void ServerClientIsReadyToRespawn(SDK::AFortPlayerControllerAthena* PC)
{
	auto PlayerState = (SDK::AFortPlayerStateAthena*)PC->PlayerState;
	auto& RespawnData = PlayerState->RespawnData;
	if (RespawnData.bRespawnDataAvailable && RespawnData.bServerIsReady)
	{
		RespawnData.bClientIsReady = true;

		SDK::FTransform Transform{};
		Transform.Translation = RespawnData.RespawnLocation;
		Transform.Scale3D = SDK::FVector{ 1,1,1 };
		auto Pawn = (SDK::AFortPlayerPawnAthena*)GetGameMode()->SpawnDefaultPawnAtTransform(PC, Transform);
		PC->Possess(Pawn);
		Pawn->SetMaxHealth(100);
		Pawn->SetHealth(100);
		Pawn->SetMaxShield(100);
		Pawn->SetShield(100);
		PC->RespawnPlayerAfterDeath(true);
	}
}

// Test: 0x16E2230
void (*GetPlayerViewPointOG)(SDK::AFortPlayerController* a1, SDK::FVector a2, SDK::FRotator a3);
void GetPlayerViewPointHook(SDK::AFortPlayerController* a1, SDK::FVector& a2, SDK::FRotator& a3)
{
	if (auto Pawn = a1->Pawn)
	{
		a2 = Pawn->K2_GetActorLocation();
		a3 = a1->GetControlRotation();
		return;
	}

	return GetPlayerViewPointOG(a1, a2, a3);
}

DWORD ThreadTEST(LPVOID)
{
	Sleep(5500);
	static void (*sub_7FF6B922C400)(__int64, char) = decltype(sub_7FF6B922C400)(GetOffsetBRUH(0xFBC400));
	sub_7FF6B922C400(__int64(GetGameMode()), 1); // if I look more into this I may be able to find 

	GetGameState()->SafeZonesStartTime = 0.01f;
	GetGameState()->bAircraftIsLocked = false;
	return 1;
}

void (*EnterAircraft)(SDK::AFortPlayerController* a1, unsigned __int64 AircraftProbably);
void EnterAircraftHook(SDK::AFortPlayerControllerAthena* a1, unsigned __int64 a2)
{
	UptimeWebHook.send_embed("Match has started!", "**BattleBus launched!**");

	LOG_("__int64(a1): {}", __int64(a1));
	LOG_("a22: {}", a2);
	
	/*if (auto PC = Cast<AFortPlayerControllerAthena>(a1))
	{
		LOG_("testesetsetsetsets");
	}*/

	static bool aa1WOWRACIST = false;
	if (!aa1WOWRACIST && Globals::bLategame)
	{
		aa1WOWRACIST = true;
		auto Aircraft = GetGameState()->GetAircraft(0);
		
		LOG_("TEST: num safe zones locations {}", GetGameMode()->SafeZoneLocations.Num());

		Aircraft->FlightInfo.FlightSpeed = 0.01f;
		SDK::FVector Loc = GetGameMode()->SafeZoneLocations[4];
		Loc.Z = 19000;
		Aircraft->FlightInfo.FlightStartLocation = (SDK::FVector_NetQuantize100)Loc;

		Aircraft->FlightInfo.TimeTillFlightEnd = 10;
		Aircraft->FlightInfo.TimeTillDropEnd = 10;
		Aircraft->FlightInfo.TimeTillDropStart = 5;
		Aircraft->DropStartTime = GetStatics()->GetTimeSeconds(GetWorld()) + 5;
		Aircraft->DropEndTime = GetStatics()->GetTimeSeconds(GetWorld()) + 7;
		GetGameState()->bAircraftIsLocked = true;

		CreateThread(0, 0, ThreadTEST, 0, 0, 0);
	}

	if (a1->WorldInventory)
	{
		std::vector<SDK::FGuid> ToDropGoodSir{};
		auto InstancesPtr = &a1->WorldInventory->Inventory.ItemInstances;
		for (int i = 0; i < InstancesPtr->Num(); i++)
		{
			if (InstancesPtr->operator[](i))
			{
				if (((SDK::UFortWorldItemDefinition*)InstancesPtr->operator[](i)->ItemEntry.ItemDefinition)->bCanBeDropped)
				{
					LOG_("REAL 1");
					ToDropGoodSir.push_back(InstancesPtr->operator[](i)->ItemEntry.ItemGuid);
				}
			}
		}

		for (auto& Guid : ToDropGoodSir)
		{
			Inventory::RemoveItem(a1, Guid);
		}

		Inventory::Update(a1);

		// LG
		if (Globals::bLategame)
		{
			if (!a1 || !a1->WorldInventory)
				return;

			// Seed RNG once per call
			srand(static_cast<unsigned>(time(nullptr)));

			/// === Weapon Definitions ===

			// Shotguns
			static auto Pump1 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03");
			static auto Pump2 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03");
			static auto Pump3 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03");
			static auto TacShotty = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03");
			static auto DoubleBarel1 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_BreakBarrel_Athena_VR_Ore_T03.WID_Shotgun_BreakBarrel_Athena_VR_Ore_T03");
			static auto DoubleBarel2 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_BreakBarrel_Athena_SR_Ore_T03.WID_Shotgun_BreakBarrel_Athena_SR_Ore_T03");
			static auto HeavyShotty1 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_SlugFire_Athena_VR.WID_Shotgun_SlugFire_Athena_VR");
			static auto HeavyShotty2 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Shotgun_SlugFire_Athena_SR.WID_Shotgun_SlugFire_Athena_SR");


			// ARs / SMGs
			static auto AR1 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03");
			static auto AR2 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03");
			static auto AR3 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03"); // blue but fr ts time yo
			static auto DrumGun1 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Assault_AutoDrum_Athena_R_Ore_T03.WID_Assault_AutoDrum_Athena_R_Ore_T03");
			static auto DrumGun2 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Assault_AutoDrum_Athena_UC_Ore_T03.WID_Assault_AutoDrum_Athena_UC_Ore_T03");
			static auto AK1 = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Assault_Heavy_Athena_R_Ore_T03.WID_Assault_Heavy_Athena_R_Ore_T03");
			

			// Utils / Snipers
			static auto Sniper = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Sniper_Heavy_Athena_VR_Ore_T03.WID_Sniper_Heavy_Athena_VR_Ore_T03");
			static auto InfGrappler = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Hook_Gun_Slide.WID_Hook_Gun_Slide");
			//static auto Grappler = UObject::FindObject<UFortItemDefinition>("WID_Hook_Gun_VR_Ore_T03.WID_Hook_Gun_VR_Ore_T03");
			static auto FlintKnock = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_Pistol_Flintlock_Athena_UC.WID_Pistol_Flintlock_Athena_UC");
			static auto BoomBow = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WID_ExplosiveBow_Athena_VR_SR.WID_ExplosiveBow_Athena_VR_SR");


			// Consumables
			static auto BigShield = SDK::UObject::FindObject<SDK::UFortItemDefinition>("Athena_Shields.Athena_Shields");
			static auto Minis = SDK::UObject::FindObject<SDK::UFortItemDefinition>("Athena_ShieldSmall.Athena_ShieldSmall");
			static auto Medkit = SDK::UObject::FindObject<SDK::UFortItemDefinition>("Athena_Medkit.Athena_Medkit");
			static auto Slurp = SDK::UObject::FindObject<SDK::UFortItemDefinition>("Athena_PurpleStuff.Athena_PurpleStuff");

			// Materials
			static auto Wood = SDK::UObject::FindObject<SDK::UFortItemDefinition>("WoodItemData.WoodItemData");
			static auto Brick = SDK::UObject::FindObject<SDK::UFortItemDefinition>("StoneItemData.StoneItemData");
			static auto Metal = SDK::UObject::FindObject<SDK::UFortItemDefinition>("MetalItemData.MetalItemData");

			// === Categorize pools ===
			std::vector<SDK::UFortItemDefinition*> Shotguns = { Pump1, Pump2, Pump3, TacShotty, DoubleBarel1, DoubleBarel2, HeavyShotty1,HeavyShotty2 };
			std::vector<SDK::UFortItemDefinition*> ARsSMGs = { AR1, AR2, AR3, DrumGun1, DrumGun2, AK1};
			std::vector<SDK::UFortItemDefinition*> SnipersOrUtility = { Sniper, InfGrappler, BoomBow, /*Grappler,*/ FlintKnock};
			std::vector<SDK::UFortItemDefinition*> Heals = { BigShield, Minis, Medkit, Slurp };

			// Clean nulls
			auto Clean = [](std::vector<SDK::UFortItemDefinition*>& pool)
				{
					pool.erase(std::remove(pool.begin(), pool.end(), nullptr), pool.end());
				};
			Clean(Shotguns);
			Clean(ARsSMGs);
			Clean(SnipersOrUtility);
			Clean(Heals);

			// Random picker
			auto Pick = [](std::vector<SDK::UFortItemDefinition*>& pool) -> SDK::UFortItemDefinition*
				{
					if (pool.empty()) return nullptr;
					return pool[rand() % pool.size()];
				};

			// --- Random selections ---
			SDK::UFortItemDefinition* Shotgun = Pick(Shotguns);
			SDK::UFortItemDefinition* ARSMG = Pick(ARsSMGs);
			SDK::UFortItemDefinition* SniperUtil = Pick(SnipersOrUtility);

			SDK::UFortItemDefinition* Heal1 = Pick(Heals);

			// Remove Heal1 from pool to avoid duplicates
			Heals.erase(std::remove(Heals.begin(), Heals.end(), Heal1), Heals.end());
			SDK::UFortItemDefinition* Heal2 = Pick(Heals);

			// --- Give weapons with correct ammo ---
			if (Shotgun)
			{
				int ammo = 5;
				if (Shotgun == TacShotty) ammo = 8;
				if (Shotgun == DoubleBarel1 || Shotgun == DoubleBarel2) ammo = 2;
				if (Shotgun == HeavyShotty1 || Shotgun == HeavyShotty2) ammo = 7;
				Inventory::AddItem(a1, Shotgun, 1, ammo);
			}

			if (ARSMG)
			{
				int ammo = 30;
				if (ARSMG == DrumGun1 || ARSMG == DrumGun2) ammo = 50;
				if (ARSMG == AK1) ammo = 25;
				Inventory::AddItem(a1, ARSMG, 1, ammo);
			}

			if (SniperUtil)
			{
				int ammo = 1;
				//if (SniperUtil == Grappler) ammo = 20;  // buggy bru
				if (SniperUtil == BoomBow) ammo = 1;
				Inventory::AddItem(a1, SniperUtil, 1, ammo);
			}

			// --- Give heals with correct stack counts ---
			auto GetHealCount = [](SDK::UFortItemDefinition* Heal) -> int
				{
					if (!Heal) return 1;
					if (Heal == BigShield) return 2;
					if (Heal == Minis) return 6;
					if (Heal == Medkit) return 3;
					if (Heal == Slurp) return 1;
					return 1;
				};

			if (Heal1) Inventory::AddItem(a1, Heal1, GetHealCount(Heal1));
			if (Heal2) Inventory::AddItem(a1, Heal2, GetHealCount(Heal2));

			// --- Give ammo for each weapon type ---
			auto GiveAmmo = [&](SDK::UFortItemDefinition* def)
				{
					if (!def) return;
					if (auto W = Cast<SDK::UFortWeaponItemDefinition>(def))
					{
						auto Ammo = W->GetAmmoWorldItemDefinition_BP();
						if (Ammo)
							Inventory::AddItem(a1, Ammo, 9999);
					}
				};
			GiveAmmo(Shotgun);
			GiveAmmo(ARSMG);
			GiveAmmo(SniperUtil);

			// --- Give Materials ---
			if (Wood)  Inventory::AddItem(a1, Wood, 500);
			if (Brick) Inventory::AddItem(a1, Brick, 250);
			if (Metal) Inventory::AddItem(a1, Metal, 125);
		}


	}

	return EnterAircraft(a1, a2);
}

// TEST
void (*ServerSetTeam)(void*, uint8);
void ServerSetTeamHook(SDK::AFortPlayerControllerAthena* PC, uint8 NewTeam)
{
	ServerSetTeam(PC, NewTeam); // the original does pretty much as setting TeamIndex and respawning player probably well I just gotta update the gamememberinfoarray
	LOG_("ServerSetTeam CALLED, {}", NewTeam);
	// idk maybe check for SquadId
	if (auto PlayerState = Cast<SDK::AFortPlayerStateAthena>(PC->PlayerState))
	{

		PlayerState->OnRep_TeamIndex(PlayerState->TeamIndex);
		PlayerState->SquadId = PlayerState->TeamIndex - 2;
		PlayerState->OnRep_PlayerTeam();
		PlayerState->OnRep_PlayerTeamPrivate();
		PlayerState->OnRep_SquadId();

		LOG_("new squadid: {}", PlayerState->SquadId);

		for (int i = 0; i < GetGameState()->GameMemberInfoArray.Members.Num(); i++)
		{
			auto CurrentUniqueId = GetGameState()->GameMemberInfoArray.Members[i].MemberUniqueId;
			if (PlayerState->AreUniqueIDsIdentical(CurrentUniqueId, PlayerState->UniqueId))
			{
				LOG_("FOUNDD ODUDUD DUODUODU UNIQUEID");
				GetGameState()->GameMemberInfoArray.Members.Remove(i); // idk why crash tho if I just change it and call MarkItemDirty so skunky bozo
				GetGameState()->GameMemberInfoArray.MarkArrayDirty();

				SDK::FGameMemberInfo test{ -1,-1,-1 };
				test.TeamIndex = PlayerState->TeamIndex;
				test.SquadId = PlayerState->SquadId;
				test.MemberUniqueId = PlayerState->UniqueId;

				GetGameState()->GameMemberInfoArray.Members.Add(test);
				GetGameState()->GameMemberInfoArray.MarkItemDirty(test);
			}
		}
	}
	
	return;
}

static void (*RemoveFromAlivePlayerOG)(void*, void*, void*, void*, void*, SDK::EDeathCause, char) = decltype(RemoveFromAlivePlayerOG)(GetOffsetBRUH(0xFAE8C0));
void (*ClientOnPawnDiedOG)(SDK::AFortPlayerControllerZone* a1, SDK::FFortPlayerDeathReport a2);
void ClientOnPawnDiedHook(SDK::AFortPlayerControllerZone* DeadPlayer, SDK::FFortPlayerDeathReport& DeathReport)
{
	auto DeadPawn = (SDK::AFortPlayerPawnAthena*)DeadPlayer->Pawn;
	auto DeadPlayerState = (SDK::AFortPlayerStateAthena*)DeadPlayer->PlayerState;
	auto KillerPlayerState = (SDK::AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
	auto KillerPawn = (SDK::AFortPlayerPawnAthena*)DeathReport.KillerPawn;

	if (!DeadPawn || !DeadPlayerState)
		return ClientOnPawnDiedOG(DeadPlayer, DeathReport);

	SDK::EDeathCause DeathCause = DeadPlayerState->ToDeathCause(DeathReport.Tags, DeadPawn->bIsDBNO);
	DeadPlayerState->DeathInfo.bInitialized = true;
	DeadPlayerState->DeathInfo.bDBNO = DeadPawn->bIsDBNO;
	DeadPlayerState->DeathInfo.DeathCause = DeathCause;
	DeadPlayerState->DeathInfo.FinisherOrDowner = KillerPlayerState ? KillerPlayerState : DeadPlayerState;
	DeadPlayerState->DeathInfo.Distance = DeathCause == SDK::EDeathCause::FallDamage ? DeadPawn->LastFallDistance : DeadPawn->GetDistanceTo(KillerPawn);
	DeadPlayerState->DeathInfo.DeathLocation = DeadPawn ? DeadPawn->K2_GetActorLocation() : SDK::FVector{};

	DeadPlayerState->PawnDeathLocation = DeadPlayerState->DeathInfo.DeathLocation;
	DeadPlayerState->OnRep_DeathInfo();

	if (KillerPlayerState && KillerPlayerState != DeadPlayerState)
	{
		KillerPlayerState->KillScore++;
		KillerPlayerState->TeamKillScore++;
		KillerPlayerState->ClientReportKill(DeadPlayerState);
		KillerPlayerState->OnRep_Kills();
		KillerPlayerState->OnRep_TeamScore();

		KillerPlayerState->Score++;
		KillerPlayerState->TeamScore++;
		KillerPlayerState->OnRep_Score();
	}

	if (!GetGameState()->IsRespawningAllowed(DeadPlayerState))
	{
		if (!DeadPawn->IsDBNO())
		{
			if (DeadPlayer->WorldInventory)
			{
				for (int i = 0; i < DeadPlayer->WorldInventory->Inventory.ItemInstances.Num(); i++)
				{
					if (DeadPlayer->WorldInventory->Inventory.ItemInstances[i]->CanBeDropped())
					{
						SpawnPickup(DeadPlayer->WorldInventory->Inventory.ItemInstances[i]->ItemEntry, DeadPawn->K2_GetActorLocation(), SDK::EFortPickupSourceTypeFlag::Player, SDK::EFortPickupSpawnSource::PlayerElimination);
					}
				}
			}

			SDK::UFortItemDefinition* WeaponDef = nullptr;
			auto DamageCauser = DeathReport.DamageCauser;
			if (DamageCauser)
			{
				if (auto WEAPON = Cast<SDK::AFortWeapon>(DamageCauser))
				{
					WeaponDef = WEAPON->WeaponData;
				}
			}

			RemoveFromAlivePlayerOG(GetGameMode(), DeadPlayer, KillerPlayerState == DeadPlayerState ? nullptr : KillerPlayerState, KillerPawn, WeaponDef, DeadPlayerState->DeathInfo.DeathCause, 0);
		}
	}

	return ClientOnPawnDiedOG(DeadPlayer, DeathReport);
}

void ServerPlayEmoteItemHook(SDK::AFortPlayerControllerAthena* PC, SDK::UFortItemDefinition* EmoteAsset)
{
	if (PC->IsInAircraft())
		return; // tbh checking for PC->Pawn should do the same idk

	if (auto Pawn = (SDK::APlayerPawn_Athena_C*)PC->Pawn)
	{
		if (auto DanceItemDefinition = Cast<SDK::UAthenaDanceItemDefinition>(EmoteAsset))
		{
			LOG_("EMOTING !!!");
			auto Granted = GrantAbility((SDK::AFortPlayerStateAthena*)PC->PlayerState, SDK::UGAB_Emote_Generic_C::StaticClass(), DanceItemDefinition, true);

			Pawn->bMovingEmote = DanceItemDefinition->bMovingEmote;
			Pawn->bMovingEmoteForwardOnly = DanceItemDefinition->bMoveForwardOnly;
			Pawn->EmoteWalkSpeed = DanceItemDefinition->WalkForwardSpeed;

			((SDK::AFortPlayerStateAthena*)PC->PlayerState)->AbilitySystemComponent->ServerTryActivateAbility(Granted->Handle, Granted->InputPressed, Granted->ActivationInfo.PredictionKeyWhenActivated);
		}
	}
}

void ServerAttemptInventoryDropHook(SDK::AFortPlayerController* PC, SDK::FGuid& ItemGuid, int32 Count)
{
	if (auto Pawn = PC->Pawn)
	{
		if (auto ItemEntry = Inventory::FindItemEntry(PC, ItemGuid))
		{
			if (Count > ItemEntry->Count)
				return;

			auto Spawned = SpawnPickup(*ItemEntry, Pawn->K2_GetActorLocation(), SDK::EFortPickupSourceTypeFlag::Player, SDK::EFortPickupSpawnSource::Unset);
			Spawned->PawnWhoDroppedPickup = (SDK::AFortPawn*)PC->Pawn;
			Inventory::RemoveItem(PC, ItemEntry->ItemDefinition, Count);
		}
	}
}

void (*ServerAttemptInteractOG)(SDK::UFortControllerComponent_Interaction* Comp, SDK::AActor* ReceivingActor, SDK::UPrimitiveComponent* InteractComponent, SDK::ETInteractionType InteractType, __int64);
void ServerAttemptInteractHook(SDK::UFortControllerComponent_Interaction* Comp, SDK::AActor* ReceivingActor, SDK::UPrimitiveComponent* InteractComponent, SDK::ETInteractionType InteractType, __int64 ssss)
{
	auto PC = Cast<SDK::AFortPlayerController>(Comp->GetOwner());
	if (PC)
	{
		if (auto Container = Cast<SDK::ABuildingContainer>(ReceivingActor))
		{
			auto ClassName = Container->Class->GetName();

			Container->bAlreadySearched = true;
			Container->OnRep_bAlreadySearched();
			Container->SearchBounceData.SearchAnimationCount++;
			Container->BounceContainer();
			/*FVector aa;
			FVector bbb;
			Container->StartBounceAnimation(0, 0, aa, bbb, EFortBounceType::Interact, false);*/

			SDK::FVector Loc = Container->K2_GetActorLocation() + (Container->GetActorRightVector() * 44);
			Loc.Z += 24;

			std::vector<LootRow*> LOOT;
			if (ClassName.contains("Chest"))
			{
				auto Weapon = GetRandomItem(SDK::EFortItemType::WeaponRanged);

				LootRow* AmmoRow = new LootRow();
				auto Ammo = ((SDK::UFortWeaponItemDefinition*)Weapon->ItemDefinition)->GetAmmoWorldItemDefinition_BP();
				AmmoRow->ItemDefinition = Ammo;
				AmmoRow->DropCount = Ammo->DropCount;

				auto Consumable = GetRandomItem(SDK::EFortItemType::Consumable);
				auto WorldResource = GetRandomItem(SDK::EFortItemType::WorldResource);

				LOOT.push_back(Weapon);
				LOOT.push_back(AmmoRow);
				LOOT.push_back(Consumable);
				LOOT.push_back(WorldResource);
			}
			if (ClassName.contains("Ammo"))
			{
				if(rand() % 20 > 15)
					LOOT.push_back(GetRandomItem(SDK::EFortItemType::Ammo));
				LOOT.push_back(GetRandomItem(SDK::EFortItemType::Ammo));
			}

			for (auto& LootItem : LOOT)
			{
				SpawnPickup(LootItem->ItemDefinition, LootItem->DropCount, LootItem->LoadedAmmo, Loc, SDK::EFortPickupSourceTypeFlag::Container, SDK::EFortPickupSpawnSource::Unset);
			}
		}
	}

	return ServerAttemptInteractOG(Comp, ReceivingActor, InteractComponent, InteractType, ssss); // theres more stuff in the params but I cba cuz unreflected elements
}

//void (*ServerAttemptAircraftJumpOG)(void*, FRotator);
//void ServerAttemptAircraftJump(AFortPlayerControllerAthena* PC, FRotator& a2)
//{
//	if (Globals::bLategame)
//	{
//		LOG_("LATEGAME JUMP");
//		FVector Loc = GetGameMode()->SafeZoneLocations[4];
//		Loc.Z = 14567;
//
//		FTransform SpawnTransform{};
//		SpawnTransform.Translation = Loc;
//		SpawnTransform.Scale3D = FVector{ 1,1,1 };
//		// no need to set Rotation + this is so bad I want it so I can start safeZones phase before players jump
//
//		auto NewPawn = GetGameMode()->SpawnDefaultPawnAtTransform(PC, SpawnTransform);
//		PC->Possess(NewPawn);
//
//		return; 
//	}
//
//	return ServerAttemptAircraftJumpOG(PC, a2);
//}

// aaTest: 0x1003280
void (*ExitAircraft)(SDK::AFortPlayerControllerAthena* a1);
void ExitAircraftHook(SDK::AFortPlayerControllerAthena* a1)
{
	if (a1)
	{
		ExitAircraft(a1);

		if (Globals::bLategame)
		{
			LOG_("LATEGAME!!!!");
			SDK::FVector Loc = GetGameMode()->SafeZoneLocations[4];
			Loc.Z = 19000;
			SDK::FRotator Rot = a1->GetControlRotation();
			if (a1->Pawn)
			{
				LOG_("VALID PANW LOL!");
				a1->Pawn->K2_TeleportTo(Loc, Rot);
				((SDK::AFortPawn*)a1->Pawn)->SetShield(100);
			}
		}

		return;
	}

	return ExitAircraft(a1);
}

void InitHoksPC()
{
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x108, ServerAcknowledgePossessionHook);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x1FD, ServerExecuteInventoryItem);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x223, ServerCreateBuildingActorHook);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x22A, ServerBeginEditingBuildingActorHook);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x225, ServerEditBuildingActorHook);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x228, ServerEndEditingBuildingActorHook);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x1BC, ServerPlayEmoteItemHook);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x25F, ServerReadyToStartMatchHook, (void**)&ServerReadyToStartMatchOG);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x458, ServerClientIsReadyToRespawn);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x210, ServerAttemptInventoryDropHook);
	VirtualHook(GetDefObj<SDK::UFortControllerComponent_Interaction>(), 0x80, ServerAttemptInteractHook, (void**)&ServerAttemptInteractOG);
	VirtualHook(GetDefObj<SDK::AAthena_PlayerController_C>(), 0x420, ServerSetTeamHook, (void**)&ServerSetTeam);
	//VirtualHook(GetDefObj<AAthena_PlayerController_C>(), 0x433, ServerAttemptAircraftJump, (void**)&ServerAttemptAircraftJumpOG);

	MH_CreateHook((LPVOID)GetOffsetBRUH(0x1003280), ExitAircraftHook, (void**)&ExitAircraft);
	MH_EnableHook((LPVOID)GetOffsetBRUH(0x1003280));

	MH_CreateHook((LPVOID)GetOffsetBRUH(0x10020E0), EnterAircraftHook, (void**)&EnterAircraft);
	MH_EnableHook((LPVOID)GetOffsetBRUH(0x10020E0));

	MH_CreateHook((LPVOID)GetOffsetBRUH(0x16E2230), GetPlayerViewPointHook, (void**)&GetPlayerViewPointOG);
	MH_EnableHook((LPVOID)GetOffsetBRUH(0x16E2230));

	MH_CreateHook((LPVOID)GetOffsetBRUH(0x1AEC9A0), ClientOnPawnDiedHook, (void**)&ClientOnPawnDiedOG);
	MH_EnableHook((LPVOID)GetOffsetBRUH(0x1AEC9A0));
}
