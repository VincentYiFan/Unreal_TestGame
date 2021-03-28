// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class MPFINAL_API APickup : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	// Set default values
	APickup();

	// Required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsActive();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActive(bool NewPickupState);

	// Function to call when the pickup is collected
	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void WasCollected(); 
	virtual void WasCollected_Implementation();

	// Server side handling of being picked up
	UFUNCTION(BlueprintAuthorityOnly, Category = "Pickup")
	virtual void PickedUpBy( APawn* Pawn );

protected:
	// true when the pickup can be used, false when the pickup is deactivated
	UPROPERTY(Replicated = OnRep_IsActive)
	bool bIsActive = false;

	// this is called whenever bIsActive is updated
	UFUNCTION()
	virtual void OnRep_IsActive();

	// The pawn who picked up the pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	APawn* PickupInstigator;

private:
	// Client-side handling of being picked up
	UFUNCTION(NetMulticast, Unreliable)
	void ClientOnPickedUpBy(APawn* Pawn);
};
