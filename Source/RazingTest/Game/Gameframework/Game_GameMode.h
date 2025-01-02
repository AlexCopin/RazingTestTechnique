// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include <RazingTest/Game/Data/PDA_GameRulesData.h>
#include "Game_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class RAZINGTEST_API AGame_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UPDA_GameRulesData> PDA_GameRulesData;

	UFUNCTION()
	virtual void PostLogin(APlayerController* PC) override;

	virtual void BeginPlay() override;
};
