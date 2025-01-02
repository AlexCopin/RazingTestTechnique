// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_GameState.h"
#include <Net/UnrealNetwork.h>
#include "Game_GameMode.h"

void AGame_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGame_GameState, PDA_GameRulesData);
}
void AGame_GameState::OnRep_PDA_GameRuleData()
{
	OnRulesSet.Broadcast(PDA_GameRulesData);
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("OnRep_PDA_GameRuleData"));
}
