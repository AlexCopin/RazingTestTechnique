// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_GameMode.h"
#include "Game_GameState.h"

void AGame_GameMode::PostLogin(APlayerController* PC)
{
	Super::PostLogin(PC);
	if (auto GS = Cast<AGame_GameState>(GameState))
	{
		GS->PDA_GameRulesData = PDA_GameRulesData;
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("PostLogin"));
	}
}

void AGame_GameMode::BeginPlay()
{
}
