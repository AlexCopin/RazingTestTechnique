// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_PlayerController.h"

void AGame_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	if(IsLocalController())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = GetPawn();
		spawnParams.Owner = this;
		Board = GetWorld()->SpawnActor<ABoard>(BoardClass, FVector::Zero(), FRotator::ZeroRotator, spawnParams);
	}
}
