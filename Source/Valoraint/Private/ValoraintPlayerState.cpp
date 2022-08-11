// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


#include "ValoraintPlayerState.h"
#include "Net/UnrealNetwork.h"

void AValoraintPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Registering variable for networking
	DOREPLIFETIME(AValoraintPlayerState, PlayerWallet);
}

void AValoraintPlayerState::Purchase_Implementation(FCurrency Cost)
{
	PlayerWallet = PlayerWallet - Cost;
}
