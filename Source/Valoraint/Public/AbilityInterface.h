// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VALORAINT_API IAbilityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FirstAbility();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SecondAbility();
};
