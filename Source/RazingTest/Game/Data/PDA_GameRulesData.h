// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PDA_GameRulesData.generated.h"


//Préférence pour des règles tweakables et centralisées pour le test et les changements d'avis.
UCLASS()
class RAZINGTEST_API UPDA_GameRulesData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxPlayerHealth = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxDeckSize = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxHandSize = 4;
};
