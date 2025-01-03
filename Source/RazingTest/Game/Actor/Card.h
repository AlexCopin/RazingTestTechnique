// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardValueSet, int, Value);

UCLASS()
class RAZINGTEST_API ACard : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ACard();
	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Value)
	int Value;
	UFUNCTION()
	void OnRep_Value();

	UFUNCTION(Server, Reliable)
	void S_AddToValue(int Added);
	UFUNCTION(Server, Reliable)
	void S_SetValue(int NewValue);
	UFUNCTION()
	void SetValue(int NewValue);

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnCardValueSet OnCardValueSet;
};
