// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MPFinalGameMode.h"
#include "MPFinalPawn.h"
#include "MPFinalHud.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "MPFinalGameState.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"

AMPFinalGameMode::AMPFinalGameMode()
{
	//DefaultPawnClass = AMPFinalPawn::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_MPFInalPawn"));
	if( PlayerPawnBPClass.Class != NULL )
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	//HUDClass = AMPFinalHud::StaticClass();

	//  Set the type of HUD used in the game
	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUDClass( TEXT("/Game/Blueprints/BP_MPFinalGameHUD"));
	if( PlayerHUDClass.Class != NULL )
	{
		HUDClass = PlayerHUDClass.Class;
	}

	// Set the type of game state used in the game
	GameStateClass = AMPFinalGameState::StaticClass();

	// Base decay rate
	DecayRate = 0.02f;

	// Base value for how frequently to drain power 
	PowerDrainDelay = 0.25f;

	// Set base value for power multiplier
	PowerToWinMultiplier = 999.f;

	// Reset stats
	DeadPlaterCount = 0;
}

void AMPFinalGameMode::BeginPlay()
{
	GetWorldTimerManager().SetTimer( PowerDrainTimer, this, &AMPFinalGameMode::DrainPowerOverTime, PowerDrainDelay, true );

	UWorld* World = GetWorld();
	check( World );
	AMPFinalGameState* MyGameState = Cast<AMPFinalGameState>(GameState);
	check( MyGameState );

	// Reset stats
	DeadPlaterCount = 0;


	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), FoundActors);
	for( auto Actor : FoundActors )
	{
		if( ASpawnVolume* TestSpawnVol = Cast<ASpawnVolume>( Actor ) )
		{
			// Add the volume to the array and ensure it only exists in the array once
			SpawnVolumeActors.AddUnique(TestSpawnVol);
		}
	}
	
	
	// Transition the game to playing state
	//MyGameState->SetCurrentState(EBatteryPlayState::EPlaying);
	HandleNewState(EBatteryPlayState::EPlaying);

	for( FConstControllerIterator It = World->GetControllerIterator(); It; ++It )
	{
		if( APlayerController* PlayerController = Cast<APlayerController>( *It ) )
		{
			if( AMPFinalPawn* BatteryCharacter = Cast<AMPFinalPawn>( PlayerController->GetPawn() ) )
			{
				MyGameState->PowerToWin = BatteryCharacter->GetInitialPower() * PowerToWinMultiplier;
				break;
			}
		}
	}

}

float AMPFinalGameMode::GetDecayRate() const
{
	return DecayRate;
}

float AMPFinalGameMode::GetPowerToWinMultiplier()
{
	return PowerToWinMultiplier;
}

void AMPFinalGameMode::DrainPowerOverTime()
{
	UWorld* World = GetWorld();
	check(World);
	AMPFinalGameState* MyGameState = Cast<AMPFinalGameState>( GameState );
	check( MyGameState );

	// Reset stats
	DeadPlaterCount = 0;

	for( FConstControllerIterator It = World->GetControllerIterator(); It; ++It )
	{
		if( APlayerController* PlayerController = Cast<APlayerController>( *It ) )
		{
			if( AMPFinalPawn* BatteryCharacter = Cast<AMPFinalPawn>( PlayerController->GetPawn() ) )
			{
				if( BatteryCharacter->GetCurrentPower() > MyGameState->PowerToWin )
				{
					//MyGameState->SetCurrentState(EBatteryPlayState::EWon);
					HandleNewState(EBatteryPlayState::EWon);
				}
				else if( BatteryCharacter->GetCurrentPower() > 0 )
				{
					BatteryCharacter->UpdatePower(-PowerDrainDelay*DecayRate*(BatteryCharacter->GetInitialPower()));
				}
				else
				{
					// poor player died.
					BatteryCharacter->DetachFromControllerPendingDestroy();

					// Destroy the character
					//BatteryCharacter->Destroy();
					BatteryCharacter->OnPlayerDeath();
					
					// See if this is the last player to die, and end the game if so
					++DeadPlaterCount;
					if( DeadPlaterCount >= GetNumPlayers() )
					{
						//MyGameState->SetCurrentState(EBatteryPlayState::EGameOver);
						HandleNewState(EBatteryPlayState::EGameOver);
					}
				}
			}
		}
	}
}

void AMPFinalGameMode::HandleNewState( EBatteryPlayState NewState )
{
	UWorld* World = GetWorld();
	check( World );
	AMPFinalGameState* MyGameState = Cast<AMPFinalGameState>( GameState );
	check( MyGameState );

	if( NewState != MyGameState->GetCurrentState() )
	{
		// Update the state, so clients know about the transition
		MyGameState->SetCurrentState(NewState);

		switch( NewState )
		{
		case EBatteryPlayState::EPlaying:
			// Activate the spawn volumes
			for( ASpawnVolume* SpawnVol : SpawnVolumeActors )
			{
				SpawnVol->SetSpawningActive(true);
			}
			break;

		case EBatteryPlayState::EGameOver:
			for( ASpawnVolume* SpawnVol : SpawnVolumeActors )
			{
				SpawnVol->SetSpawningActive( false );
			}
			break;

		case EBatteryPlayState::EWon:
			for( ASpawnVolume* SpawnVol : SpawnVolumeActors )
			{
				SpawnVol->SetSpawningActive( false );
			}
			break;

		default:
		case EBatteryPlayState::EUnknown:
			break;
		}
	}
}

