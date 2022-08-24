// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ValoraintGameState.generated.h"

class UWeaponData;
/**
 * 
 */
UCLASS()
class VALORAINT_API AValoraintGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	// Array containing static weapon data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UWeaponData*> AllWeapons;
};
