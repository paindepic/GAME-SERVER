#pragma once
#include "Inventory.h"

void ServerHandlePickupHook(SDK::AFortPlayerPawn* Pawn, SDK::AFortPickup* Pickup, float InFlyTime, SDK::FVector InStartDirection, bool bPlayPickupSound)
{
	LOG_("InStartDirection: [{},{},{}]", InStartDirection.X, InStartDirection.Y, InStartDirection.Z);
	
	if (Pickup->bPickedUp)
		return;
	
	if (Pawn && Pawn->Controller)
	{
		if (auto PC = (SDK::AFortPlayerController*)Pawn->Controller)
		{
			// swapping
			if (Inventory::GetQuickBars(Pickup->PrimaryPickupItemEntry.ItemDefinition) == SDK::EFortQuickBars::Primary && Inventory::IsInventoryFull(PC)) // the inventory is full
			{
				LOG_("Swapping moment");

				if (auto CurrentWeapon = Pawn->CurrentWeapon)
				{
					if (auto CurrentWeaponItemEntry = Inventory::FindItemEntry(PC, CurrentWeapon->WeaponData))
					{
						if (!CurrentWeapon->WeaponData->bCanBeDropped) // BECAUSE IT WILL JUST SWAP WITH PICKAXE IF I DON4T PREVENT THIS!!!
						{
							return;
						}

						if (CurrentWeaponItemEntry->ItemDefinition == Pickup->PrimaryPickupItemEntry.ItemDefinition)
						{
							// check if stackable
							if (CurrentWeaponItemEntry->ItemDefinition->MaxStackSize <= 1)
							{
								PC->ServerAttemptInventoryDrop(CurrentWeaponItemEntry->ItemGuid, CurrentWeaponItemEntry->Count);
								auto NewItem = Inventory::AddItem(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo, false);
								PC->ClientEquipItem(NewItem->ItemGuid, true);
							}
							else
							{
								// stackable
								// AddItem will drop if its above max stack size
								Inventory::AddItem(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo, false);
							}
						}
						else
						{
							if (CurrentWeaponItemEntry->ItemDefinition->MaxStackSize <= 1)
							{
								PC->ServerAttemptInventoryDrop(CurrentWeaponItemEntry->ItemGuid, CurrentWeaponItemEntry->Count);
								auto NewItem = Inventory::AddItem(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo, false);
								PC->ClientEquipItem(NewItem->ItemGuid, true);
							}
							else
							{
								// stackable
								// AddItem will drop if its above max stack size
								Inventory::AddItem(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo, false);
							}
						}
						
					}
				}
			}
			else
			{
				Inventory::AddItem(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo, false);
			}

		}

		Pickup->bPickedUp = true;
		Pickup->OnRep_bPickedUp();

		Pawn->IncomingPickups.Add(Pickup);
		auto& LocData = Pickup->PickupLocationData;
		LocData.StartDirection = (SDK::FVector_NetQuantizeNormal)InStartDirection;
		LocData.FlyTime = 0.40f;
		LocData.PickupTarget = Pawn;
		LocData.ItemOwner = Pawn;
		LocData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
		Pickup->OnRep_PickupLocationData();

	}
}

static double (*OnRep_ZiplineState)(void* FortPawn) = decltype(OnRep_ZiplineState)(GetOffsetBRUH(0x16A2800));
void ServerSendZiplineState(SDK::AFortPlayerPawn* Pawn, SDK::FZiplinePawnState& InState) // maybe call original too?
{
	if (Pawn && Pawn->Controller)
	{
		// IDK WHY CHECK FOR CONTROLLER BECAUSE IF I DONT IT WILL JUST CRASH

		Pawn->ZiplineState = InState;
		
		// PROPER iirc: theres a curve table for zipline jump strenghttththt but I cba to look at it 
		if (InState.bJumped)
		{
			LOG_("ZIPLINES JUMP LOLOOLOLO");
			Pawn->LaunchCharacter(SDK::FVector{ 0,0,1200 }, false, true);
		}
		OnRep_ZiplineState(Pawn);
	}
}

// 0x16A94D0: OnCapsuleBeginOverlap TEST idk
void (*OnCapsuleBeginOverlapOG)(SDK::AFortPawn* FortPawn, SDK::UPrimitiveComponent* OverlappedComp, SDK::AActor* OtherActor, SDK::UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, SDK::FHitResult SweepResult);
void OnCapsuleBeginOverlapHook(SDK::AFortPawn* FortPawn, SDK::UPrimitiveComponent* OverlappedComp, SDK::AActor* OtherActor, SDK::UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, SDK::FHitResult SweepResult)
{
	LOG_("CapsuleBeginOVerlapTEST: 0x{:x}", __int64(_ReturnAddress()) - __int64(GetModuleHandleW(0)));
	if (auto Pawn = Cast<SDK::AFortPlayerPawn>(FortPawn))
	{
		LOG_("PANW VLIAD!!!");
		if (auto Pickup = Cast<SDK::AFortPickup>(OtherActor))
		{
			LOG_("PICKUP VALID!!");

			if (!Pickup->bPickedUp)
			{
				if (Pickup->PawnWhoDroppedPickup != Pawn)
				{
					if(Pickup->PrimaryPickupItemEntry.ItemDefinition->IsA(SDK::UFortAmmoItemDefinition::StaticClass())) // for now
						Pawn->ServerHandlePickup(Pickup, 0.40f, { 0,0,1 }, true);
				}
			}

		}
	}
	return OnCapsuleBeginOverlapOG(FortPawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}


void InitPawnHooks()
{
	VirtualHook(GetDefObj<SDK::APlayerPawn_Athena_C>(), 0x1BA, ServerHandlePickupHook);
	// TODO ziplines
	VirtualHook(GetDefObj<SDK::APlayerPawn_Athena_C>(), 0x1C5, ServerSendZiplineState);

	/*MH_CreateHook((LPVOID)GetOffsetBRUH(0x16A94D0), OnCapsuleBeginOverlapHook, (void**)&OnCapsuleBeginOverlapOG);
	MH_EnableHook((LPVOID)GetOffsetBRUH(0x16A94D0));*/
}
