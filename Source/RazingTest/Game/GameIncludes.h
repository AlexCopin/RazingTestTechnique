#pragma once

#include "GameIncludes.generated.h"


USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Health;
};

USTRUCT(BlueprintType)
struct FCardStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Value;
};