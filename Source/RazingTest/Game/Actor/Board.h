// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardSlot.h"
#include "Components/ArrowComponent.h"

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

	//To make something working fast for the prototype, use normal EnhancedInputs for inputs in correct case
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

public:
	ABoard();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> RootComp;
	//Board placements
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> DeckPlacement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> HandPlacement;
};
