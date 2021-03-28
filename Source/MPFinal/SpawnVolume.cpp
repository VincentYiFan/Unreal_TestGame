// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Pickup.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// only do on sever
	if( Role == ROLE_Authority )
	{
		WhereToSpawn = CreateDefaultSubobject<UBoxComponent>( TEXT( "SpawnVolume" ) );
		RootComponent = WhereToSpawn;

		// Set some base value for range
		SpawnDelayRangeLow = 1.0f;
		SpawnDelayRangeHigh = 4.5f;
	}
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	// Set the timer to start spawning pickups
	//SpawnDelay = FMath::FRandRange( SpawnDelayRangeLow, SpawnDelayRangeHigh );
	//GetWorldTimerManager().SetTimer( SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false );
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASpawnVolume::GetRandomPointInVolume()
{
	if( WhereToSpawn != NULL )
	{
		FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
		FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
		return UKismetMathLibrary::RandomPointInBoundingBox( SpawnOrigin, SpawnExtent );
	}

	return FVector();
}

void ASpawnVolume::SetSpawningActive( bool bShouldSpawn )
{
	if( Role == ROLE_Authority )
	{
		if( bShouldSpawn )
		{
			// Start spawning
			SpawnDelay = FMath::FRandRange( SpawnDelayRangeLow, SpawnDelayRangeHigh );
			GetWorldTimerManager().SetTimer( SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false );
		}
		else
		{
			// Stop spawning
			GetWorldTimerManager().ClearTimer(SpawnTimer);
		}
	}
}

void ASpawnVolume::SpawnPickup()
{
	// if we are the sever and have something to spawn
	if( Role == ROLE_Authority && WhatToSpawn != NULL )
	{
		// check for valid world
		if( UWorld* const World = GetWorld() )
		{
			// setup any required params/info
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			// where shall we put the pickup
			FVector SpawnLocation = GetRandomPointInVolume();

			// set the random rotation for the spawned pickup
			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.f;
			SpawnRotation.Pitch = FMath::FRand() * 360.f;
			SpawnRotation.Roll = FMath::FRand() * 360.f;

			// drop the new pickup into the world
			APickup* const SpawnedPickup = World->SpawnActor<APickup>( WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams );

			// Delay for a bit before spawning the next pickup
			SetSpawningActive(true);
		}
	}
}

