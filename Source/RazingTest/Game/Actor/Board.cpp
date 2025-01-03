// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
#include <RazingTest/Game/Gameframework/Game_GameState.h>
#include <RazingTest/Game/Data/PDA_GameRulesData.h>
#include "DrawDebugHelpers.h"
#include <RazingTest/Game/Gameframework/Game_PlayerController.h>

ABoard::ABoard()
{
	RootComp = CreateDefaultSubobject<UArrowComponent>("RootComponent");
	SetRootComponent(RootComp);

	FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);

	DeckPlacement = CreateDefaultSubobject<UArrowComponent>("DeckPlacement");
	DeckPlacement->AttachToComponent(RootComp, rules);
	HandPlacement = CreateDefaultSubobject<UArrowComponent>("HandPlacement");
	HandPlacement->AttachToComponent(RootComp, rules);
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();
	FVector location = GetActorLocation();
	for(int i = 0; i < 9; i++)
	{
		float offsetX = Offset * FMath::DivideAndRoundDown(i, 3) + Offset;
		float offsetY = i % 3 * Offset - Offset;
		FVector newLocation(location.X - offsetX, location.Y + offsetY, location.Z);

		// Symetry with 0.0
		FVector newLocationMirrored {-newLocation.X, newLocation.Y, newLocation.Z};
		//FVector newLocationMirrored = newLocation.MirrorByVector(FVector::Zero());
		FActorSpawnParameters spawnParam;
		spawnParam.Owner = GetOwner();
		spawnParam.Instigator = GetInstigator();
		auto allySlot = GetWorld()->SpawnActor<ABoardSlot>(SlotClass, newLocation, GetActorRotation(), spawnParam);
		AllySlots.AddUnique(allySlot);
		allySlot->NumSlot = i;
		allySlot->bOwned = true;
		auto enemySlot = GetWorld()->SpawnActor<ABoardSlot>(SlotClass, newLocationMirrored, GetActorRotation(), spawnParam);
		EnemySlots.AddUnique(enemySlot);
		enemySlot->NumSlot = i;
	}
}

void ABoard::NotifyActorOnClicked(FKey ButtonPressed)
{
	if(ButtonPressed == EKeys::LeftMouseButton)
	{
		if(auto PC = Cast<AGame_PlayerController>(GetOwner()))
		{
			PC->S_TryDrawCard();
		}
	}
}
