// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardSlot.generated.h"

class ACard;
UCLASS()
class RAZINGTEST_API ABoardSlot : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int NumSlot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOwned;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFull;

	UFUNCTION(BlueprintCallable)
	void PutCard(ACard* Card);
};
