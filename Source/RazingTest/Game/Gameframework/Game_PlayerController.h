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
	void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABoard> BoardClass;

	UPROPERTY()
	TObjectPtr<ABoard> Board;
};
