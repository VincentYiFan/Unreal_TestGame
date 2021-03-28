// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class MPFINAL_API ABatteryPickup : public APickup
{
	GENERATED_BODY()
	
public:
	ABatteryPickup();

	// Required network setup
	void GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

	// Server side handling of being picked up by the child class
	//UFUNCTION( BlueprintAuthorityOnly, Category = "Pickup" )
	void PickedUpBy( APawn* Pawn ) override;

	// Public method to access the battery's power level
	float GetPower();

protected:
	// Set the amount of power the battery holds
	UPROPERTY(Replicated, EditAnywhere, Category = "Power", Meta = (BlueprintProtected = "true"))
	float BatteryPower;
};
