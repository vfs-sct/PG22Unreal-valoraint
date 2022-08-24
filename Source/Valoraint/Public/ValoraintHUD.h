// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

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

	// Reference to the drawn widget
	UPROPERTY(BlueprintReadOnly)
	UShopScreen* ShopScreen = nullptr;

	// Class of widget to draw
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UShopScreen> ShopScreenClass;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

