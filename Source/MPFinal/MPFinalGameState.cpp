// Fill out your copyright notice in the Description page of Project Settings.


#include "MPFinalGameState.h"
#include "Net/UnrealNetwork.h"

AMPFinalGameState::AMPFinalGameState()
{
	CurrentState = EBatteryPlayState::EUnknown;
}

void AMPFinalGameState::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPFinalGameState, PowerToWin);
	DOREPLIFETIME(AMPFinalGameState, CurrentState);
}

EBatteryPlayState AMPFinalGameState::GetCurrentState() const
{
	return CurrentState;
}

void AMPFinalGameState::SetCurrentState( EBatteryPlayState NewState )
{
	if( Role == ROLE_Authority )
	{
		CurrentState = NewState;
	}
}

void AMPFinalGameState::OnRep_CurrentState()
{

}
