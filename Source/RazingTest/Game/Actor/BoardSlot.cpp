// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardSlot.h"
#include "Card.h"

// Called when the game starts or when spawned
void ABoardSlot::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoardSlot::PutCard(ACard* Card)
{
	if(!bFull && bOwned)
	{

	}
}

