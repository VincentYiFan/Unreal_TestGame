// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

APickup::APickup()
{
	bReplicates = true;
	
	// do not need to tick every frame
	PrimaryActorTick.bCanEverTick = false;

	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);

	if( Role == ROLE_Authority )
	{
		bIsActive = true;
	}
}

void APickup::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, bIsActive);
}

bool APickup::IsActive()
{
	return bIsActive;
}

void APickup::SetActive( bool NewPickupState )
{
	if( Role == ROLE_Authority )
	{
		bIsActive = NewPickupState;
	}
}

void APickup::WasCollected_Implementation()
{
	// Log a debug message
	UE_LOG( LogClass, Log, TEXT( "APickup::WasCollected_Implemention %s"), *GetName() );
}

void APickup::OnRep_IsActive()
{

}

void APickup::PickedUpBy( APawn* Pawn )
{
	if( Role == ROLE_Authority )
	{
		PickupInstigator = Pawn;
		// Notify clients of the picked up action
		ClientOnPickedUpBy(Pawn);
	}
}


void APickup::ClientOnPickedUpBy_Implementation( APawn* Pawn )
{
	PickupInstigator = Pawn;
	// Fire the Blueprint Native Event, which itself cannot be replicated
	WasCollected();
}
