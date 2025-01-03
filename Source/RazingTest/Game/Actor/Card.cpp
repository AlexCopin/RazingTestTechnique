// Fill out your copyright notice in the Description page of Project Settings.


#include "Card.h"
#include <Net/UnrealNetwork.h>

ACard::ACard()
{
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACard::BeginPlay()
{
	Super::BeginPlay();
}

void ACard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACard, Value);
}

void ACard::OnRep_Value()
{
	OnCardValueSet.Broadcast(Value);
}

void ACard::SetValue(int NewValue)
{
	Value = NewValue;
	OnCardValueSet.Broadcast(Value);
}

void ACard::S_AddToValue_Implementation(int Added)
{
	Value += Added;
}

void ACard::S_SetValue_Implementation(int NewValue)
{
	Value = NewValue;
	OnCardValueSet.Broadcast(Value);
}

