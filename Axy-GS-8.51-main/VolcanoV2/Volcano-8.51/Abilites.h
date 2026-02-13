#pragma once
#include "framework.h"

static __int64 (*Fgameplauyabilirtyspecctor)(void*, void*, char, int, void*) = decltype(Fgameplauyabilirtyspecctor)(GetOffsetBRUH(0x868290));
static __int64* (*GiveAbility)(void*, void*, SDK::FGameplayAbilitySpec a3) = decltype(GiveAbility)(GetOffsetBRUH(0x843DF0));
static char (*InternalTryActivateAbility)(
	SDK::UAbilitySystemComponent* a1,
	SDK::FGameplayAbilitySpecHandle a2,
	SDK::FPredictionKey a3,
	SDK::UGameplayAbility** a4,
	void* a5,
	SDK::FGameplayEventData* a6) = decltype(InternalTryActivateAbility)(GetOffsetBRUH(0x8455D0));

// https://github.com/EpicGames/UnrealEngine/blob/463443057fb97f1af0d2951705324ce8818d2a55/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L250C1-L250C98
SDK::FGameplayAbilitySpec* GrantAbility(SDK::AFortPlayerStateAthena* PlayerState, SDK::UClass* AbilityClass, SDK::UObject* SourceObj = nullptr, bool ActivateOnce = false)
{
	if (!PlayerState || !AbilityClass)
		return nullptr;
	if (!PlayerState->AbilitySystemComponent)
		return nullptr;

	SDK::FGameplayAbilitySpec TEST{};
	Fgameplauyabilirtyspecctor(&TEST, AbilityClass->DefaultObject, 1, -1, SourceObj);
	TEST.RemoveAfterActivation = ActivateOnce;

	GiveAbility(PlayerState->AbilitySystemComponent, &TEST.Handle, TEST);

	return &TEST;
}

void GrantAbilitySet(SDK::AFortPlayerStateAthena* bbg, SDK::UFortAbilitySet* Set = nullptr)
{
	static auto aaaREAL = SDK::UObject::FindObject<SDK::UFortAbilitySet>("GAS_AthenaPlayer.GAS_AthenaPlayer");
	auto AbilitySet = Set ? Set : aaaREAL;
	if (AbilitySet)
	{
		for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
		{
			GrantAbility(bbg, AbilitySet->GameplayAbilities[i].Get());
		}
	}
}

// https://github.com/EpicGames/UnrealEngine/blob/463443057fb97f1af0d2951705324ce8818d2a55/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L737
SDK::FGameplayAbilitySpec* FindAbilitySpecFromHandle(SDK::UAbilitySystemComponent* AbilitySystemComponent, SDK::FGameplayAbilitySpecHandle Handle)
{
	for (int i = 0; i < AbilitySystemComponent->ActivatableAbilities.Items.Num(); i++)
	{
		if (AbilitySystemComponent->ActivatableAbilities.Items[i].Handle.Handle == Handle.Handle) 
		{
			return &AbilitySystemComponent->ActivatableAbilities.Items[i]; 
		}
	}

	return nullptr;
}

// https://github.com/EpicGames/UnrealEngine/blob/463443057fb97f1af0d2951705324ce8818d2a55/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1754
void InternalServerTryActivateAbilityHook(SDK::UAbilitySystemComponent* ASc, SDK::FGameplayAbilitySpecHandle Handle, bool InputPressed, const SDK::FPredictionKey& PredictionKey, SDK::FGameplayEventData* TriggerEventData)
{
	SDK::FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(ASc, Handle);
	if (!Spec)
	{
		// Can potentially happen in race conditions where client tries to activate ability that is removed server side before it is received.
		// ABILITY_LOG(Display, TEXT("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid SpecHandle!"));
		ASc->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	const SDK::UGameplayAbility* AbilityToActivate = Spec->Ability;

	// Consume any pending target info, to clear out cancels from old executions

	SDK::UGameplayAbility* InstancedAbility = nullptr;
	Spec->InputPressed = true;

	// Attempt to activate the ability (server side) and tell the client if it succeeded or failed.
	if (InternalTryActivateAbility(ASc, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
	{
		// TryActivateAbility handles notifying the client of success
	}
	else
	{
		// ABILITY_LOG(Display, TEXT("InternalServerTryActivateAbility. Rejecting ClientActivation of %s. InternalTryActivateAbility failed: %s"), *GetNameSafe(Spec->Ability), *InternalTryActivateAbilityFailureTags.ToStringSimple());
		ASc->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		Spec->InputPressed = false;

		ASc->ActivatableAbilities.MarkItemDirty(*Spec);
	}
}

void InitAbilities()
{
	// L skunky bozo if anyone wants abilities I need to get uabilitysystemcomponent start vtable index

	VirtualHook(GetDefObj<SDK::UFortAbilitySystemComponentAthena>(), 0xF4, InternalServerTryActivateAbilityHook);
}
