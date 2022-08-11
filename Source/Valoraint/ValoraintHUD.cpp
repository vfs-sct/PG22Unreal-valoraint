// Copyright Epic Games, Inc. All Rights Reserved.

#include "ValoraintHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "CanvasItem.h"
#include "Blueprint/UserWidget.h"
#include "Data/WeaponData.h"
#include "UI/ShopScreen.h"
#include "UObject/ConstructorHelpers.h"

AValoraintHUD::AValoraintHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void AValoraintHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void AValoraintHUD::ToggleShop()
{
	UWorld* world = GetWorld();
	if(!world) return;

	if(!ShopScreen)
	{
		ShopScreen = CreateWidget<UShopScreen>(GetOwningPlayerController(), ShopScreenClass, TEXT("Shop Screen"));
		ShopScreen->AddToViewport();
		return;
	}

	ShopScreen->RemoveFromParent();
	ShopScreen = nullptr;
}
