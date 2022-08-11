// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ValoraintPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VALORAINT_API AValoraintPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;

private:
	UFUNCTION()
	void ToggleShop();
};
