// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Game_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RAZINGTEST_API AGame_PlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	UFUNCTION()
	virtual void BeginPlay() override;
};
