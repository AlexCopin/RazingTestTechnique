// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <RazingTest/Game/Actor/Board.h>

#include "Game_PlayerController.generated.h"
/**
 * 
 */
UCLASS()
class RAZINGTEST_API AGame_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AGame_PlayerController();
	//Replication
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void BeginPlay() override;

	//------------------Variables-----------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ACard> CardClass;
	UPROPERTY()
	TObjectPtr<ABoard> Board;

	bool bDeckCreated = false;
	//--------------------------------

	//I chose to replicate only the values (Simpler in Peer-to-peer, but not my choice if it was a DedicatedServer)
	//so the cards themselves are only locales, but the values they are created from are replicated to assure the same behavior 
	UPROPERTY()
	TArray<ACard*> CardDeck;
	UPROPERTY(ReplicatedUsing = OnRep_CardDeckValues)
	TArray<int> CardDeckValues;
	UFUNCTION()
	void OnRep_CardDeckValues();

	UPROPERTY()
	TArray<ACard*> CardHand;
	UPROPERTY(ReplicatedUsing = OnRep_CardHandValueDrawn)
	int CardHandValueDrawn = 0;
	UFUNCTION()
	void OnRep_CardHandValueDrawn();

	//-----------Deck functions-----------
	UFUNCTION(Server, Reliable)
	void S_CreateDeck();
	UFUNCTION()
	void InitCardDecks();
	UFUNCTION()
	void TryDrawCard();
	UFUNCTION(Server, Reliable)
	void S_TryDrawCard();

	void PlaceCardInHand();
	//--------------------------------

};
