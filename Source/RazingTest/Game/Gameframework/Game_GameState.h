// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include <RazingTest/Game/Data/PDA_GameRulesData.h>

#include "Game_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRulesSet, UPDA_GameRulesData*, Rules);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginGame);


UCLASS()
class RAZINGTEST_API AGame_GameState : public AGameState
{
	GENERATED_BODY()
private:
public:
	//Replication
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//Bonne pratique de copier les règles du GameMode (GameMode présent seulement sur le serveur)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_PDA_GameRuleData)
	TObjectPtr<UPDA_GameRulesData> PDA_GameRulesData;

	UFUNCTION()
	void OnRep_PDA_GameRuleData();

	UPROPERTY(BlueprintAssignable)
	FOnRulesSet OnRulesSet;

	UPROPERTY(ReplicatedUsing=OnRep_bGameBegun)
	bool bGameBegun = false;
	UFUNCTION()
	void OnRep_bGameBegun();

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnBeginGame OnBeginGame;
};
