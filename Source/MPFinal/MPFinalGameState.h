// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MPFinalGameState.generated.h"

UENUM(BlueprintType)
enum EBatteryPlayState
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};

/**
 * 
 */
UCLASS()
class MPFINAL_API AMPFinalGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AMPFinalGameState();

	// Required network scaffolding
	void GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Power")
	float PowerToWin;

	// Returns the current state of gameplay
	UFUNCTION(BlueprintPure, Category = "Power")
	EBatteryPlayState GetCurrentState() const;

	//Transition the game to a new play state, Only usable on server.
	void SetCurrentState(EBatteryPlayState NewState);

	UFUNCTION()
	void OnRep_CurrentState();

private:
	// Track the current playing state
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	TEnumAsByte<enum EBatteryPlayState> CurrentState;
};
