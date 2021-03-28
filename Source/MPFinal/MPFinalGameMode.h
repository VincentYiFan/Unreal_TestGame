// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPFinalGameState.h"
#include "MPFinalGameMode.generated.h"

UCLASS(MinimalAPI)
class AMPFinalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMPFinalGameMode();

	// Start the power drain timer
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetDecayRate() const;

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWinMultiplier();

protected:
	// How many times per seconds to update characters power and check game rules
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
	float PowerDrainDelay;

	// Access the timer for recurring power draining
	FTimerHandle PowerDrainTimer;

	// The rate at which characters lose power (% of InitialPower per second)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float DecayRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float PowerToWinMultiplier;

	// Track the number of players who have run out of energy
	int32 DeadPlaterCount;

private:
	void DrainPowerOverTime();

	TArray<class ASpawnVolume*> SpawnVolumeActors;

	void HandleNewState(EBatteryPlayState NewState);
};



