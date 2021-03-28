// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABatteryPickup::ABatteryPickup()
{
	// Keep movement synced from server to clients
	bReplicateMovement = true;

	// This pickup is physics enabled and should move
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

	// Set a base value for the power
	BatteryPower = 200.f;
}


void ABatteryPickup::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABatteryPickup, BatteryPower);
}

void ABatteryPickup::PickedUpBy( APawn* Pawn )
{
	Super::PickedUpBy(Pawn);

	if( Role == ROLE_Authority )
	{
		// to give client to play vfx, etc... before destroying the battery
		SetLifeSpan(2.0f);
	}
}

float ABatteryPickup::GetPower()
{
	return BatteryPower;
}
