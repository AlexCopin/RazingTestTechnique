// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardSlot.h"

#include "Board.generated.h"

UCLASS()
class RAZINGTEST_API ABoard : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ABoardSlot*> AllySlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ABoardSlot*> EnemySlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Offset = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABoardSlot> SlotClass;
};
