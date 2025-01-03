// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_PlayerController.h"
#include "Game_PlayerState.h"
#include "Game_GameState.h"
#include <Kismet/KismetArrayLibrary.h>
#include "RazingTest/Game/Actor/Card.h"
#include <Net/UnrealNetwork.h>
#include "Game_GameMode.h"
#include <Kismet/GameplayStatics.h>
#include "RazingTest/Game/Actor/Board.h"
#include "Algo/RandomShuffle.h"

AGame_PlayerController::AGame_PlayerController()
{
	//For interaction with the deck
	bEnableClickEvents = true;
}

void AGame_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGame_PlayerController, CardHandValueDrawn);
	DOREPLIFETIME(AGame_PlayerController, CardDeckValues);
}

void AGame_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetShowMouseCursor(true);
	//We know there's only one, not useful to Spawn it inside the PC, GetAllActorsOfClass when used very little isn't cost important.
	//More flexible for the Board placements
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoard::StaticClass(), OutActors);
	if (auto board = Cast<ABoard>(OutActors[0]))
	{
		Board = board;
		//Set the Owner/Instigator correctly nonetheless, allows RPC calls in Board.cpp
		if(IsLocalPlayerController())
		{
			Board->SetInstigator(GetPawn());
			Board->SetOwner(this);
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Board set owner"));
		}
	}

	//Bind to OnBeginGame when it's the player's PC (Host or Client)
	if(IsLocalPlayerController())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = GetPawn();
		spawnParams.Owner = this; 

		if(auto GS = GetWorld()->GetGameState<AGame_GameState>())
		{
			if (!GS->bGameBegun)
			{
				GS->OnBeginGame.AddDynamic(this, &AGame_PlayerController::S_CreateDeck);
				uint32 uniqueId = GetUniqueID();
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString("OnBeginGameBound " + FString::FromInt(uniqueId)));
			}
			else
				S_CreateDeck();
		}
	}
}
void AGame_PlayerController::OnRep_CardDeckValues()
{
	if(IsLocalPlayerController() && !bDeckCreated)
	{
		//Test with values
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = GetPawn();
		spawnParams.Owner = this;
		for (int i = 0; i < CardDeckValues.Num(); i++)
		{
			auto card = GetWorld()->SpawnActor<ACard>(CardClass, Board->DeckPlacement->GetComponentLocation(), FRotator::ZeroRotator, spawnParams);
			CardDeck.Add(card);
			card->SetValue(CardDeckValues[i]);
		}
		bDeckCreated = true;	
	}
}
void AGame_PlayerController::InitCardDecks()
{
	TArray<int> tempIntArray;
	//Always valid on server
	int deckSize = GetWorld()->GetAuthGameMode<AGame_GameMode>()->PDA_GameRulesData->MaxDeckSize;
	for (int i = 1; i <= deckSize; i++)
	{
		tempIntArray.Add(i);
	}
	Algo::RandomShuffle(tempIntArray);
	CardDeckValues = tempIntArray;

	//Test with values
	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = GetPawn();
	spawnParams.Owner = this;

	if (IsNetMode(NM_ListenServer) && IsLocalPlayerController() && !bDeckCreated)
	{
		for (int i = 0; i < CardDeckValues.Num(); i++)
		{
			auto card = GetWorld()->SpawnActor<ACard>(CardClass, Board->DeckPlacement->GetComponentLocation(), FRotator::ZeroRotator, spawnParams);
			CardDeck.Add(card);
			card->SetValue(CardDeckValues[i]);
		}
		bDeckCreated = true;
	}
}


void AGame_PlayerController::S_CreateDeck_Implementation()
{
	InitCardDecks();
}

void AGame_PlayerController::S_TryDrawCard_Implementation()
{
	int maxHandSize = GetWorld()->GetAuthGameMode<AGame_GameMode>()->PDA_GameRulesData->MaxHandSize;
	if (CardHand.Num() <= maxHandSize)
	{
		CardHandValueDrawn = CardDeckValues[0];
		CardDeckValues.RemoveAt(0);
		if(IsNetMode(NM_ListenServer) && IsLocalPlayerController())
		{
			PlaceCardInHand();
		}
	}
}

void AGame_PlayerController::TryDrawCard()
{
	S_TryDrawCard();
}

void AGame_PlayerController::OnRep_CardHandValueDrawn()
{
	if (IsLocalPlayerController())
		PlaceCardInHand();
}


void AGame_PlayerController::PlaceCardInHand()
{
	FVector handLoc = Board->HandPlacement->GetComponentLocation();
	CardHand.Add(CardDeck[0]);
	FRotator rotation = CardDeck[0]->GetActorRotation();
	rotation.Pitch += 180;
	CardDeck.RemoveAt(0);
	//Arbitrary offset == 150
	float leftPointOffset = handLoc.Y - (CardHand.Num() * 150 / 2);
	for(int i = 0; i < CardHand.Num(); i++)
	{
		CardHand[i]->SetActorLocation(FVector(handLoc.X, leftPointOffset + i * 150, handLoc.Z));
		CardHand[i]->SetActorRotation(rotation);
	}
	
}
