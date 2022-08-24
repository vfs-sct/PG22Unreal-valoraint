// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Data/Currency.h"
#include "ValoraintPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class VALORAINT_API AValoraintPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// Money player has to purchase weapons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FCurrency PlayerWallet;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Purchase a weapon that the pc can afford by subtracting the money from their wallet
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Purchase(FCurrency Cost);
};
