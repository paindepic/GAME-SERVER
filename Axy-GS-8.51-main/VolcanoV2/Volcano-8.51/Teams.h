#pragma once
#include "framework.h"

static int TeamIndex = 0;
static int NumPlayerPerTeam = 0;
static int CurrentTeamMembers = 0;

// I would use EFortTeam enum but I think Its broken but idk If that would matter
uint8 PickTeamHook(SDK::AFortGameModeAthena* GameMode, uint8 PreferredTeam, SDK::AFortPlayerControllerAthena* NewPlayer)
{
	if (auto PlayerState = (SDK::AFortPlayerStateAthena*)NewPlayer->PlayerState)
	{/*
		auto TeamTEST = GetGameState()->Teams[TeamIndex == 3 ? TeamIndex : TeamIndex - 1];
		LOG_("LAST TEAM MEMBERSTUFF: {}", TeamTEST->TeamMembers.Num());*/

		int RetValue = TeamIndex;
		CurrentTeamMembers++;
		if (CurrentTeamMembers == NumPlayerPerTeam)
		{
			LOG_("aaa test ong fr !!");
			CurrentTeamMembers = 0;
			TeamIndex++;
		}

		return RetValue;
	}
	return 0;
}

uint8 GetSquadIdForCurrentPlayerHook(SDK::AFortGameSessionDedicatedAthena* GameSession, SDK::FUniqueNetIdRepl& InUniqueId)
{
	LOG_("GetSquadIdForCurrentPlayer called!");
	return 1;
}
