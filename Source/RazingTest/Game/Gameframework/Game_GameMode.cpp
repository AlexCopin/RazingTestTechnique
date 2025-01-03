// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_GameMode.h"
#include "Game_GameState.h"

void AGame_GameMode::PostLogin(APlayerController* PC)
{
	Super::PostLogin(PC);
	if (auto GS = Cast<AGame_GameState>(GameState))
	{

		uint32 uniqueId = PC->GetUniqueID();
		if(GetNumPlayers() >= 2)
		{
			GS->PDA_GameRulesData = PDA_GameRulesData;
			GS->bGameBegun = true;
			GS->OnBeginGame.Broadcast();
		}
	}
}

void AGame_GameMode::BeginPlay()
{
	Super::BeginPlay();
}
