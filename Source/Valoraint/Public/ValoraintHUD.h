// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ValoraintHUD.generated.h"

class UShopScreen;
UCLASS()
class AValoraintHUD : public AHUD
{
	GENERATED_BODY()

public:
	AValoraintHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UFUNCTION()
	void ToggleShop();

	UPROPERTY(BlueprintReadOnly)
	UShopScreen* ShopScreen = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UShopScreen> ShopScreenClass;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

