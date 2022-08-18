// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


#include "UI/ShopScreen.h"

#include "ValoraintGameState.h"
#include "Valoraint/Public/Data/WeaponData.h"
#include "Components/TileView.h"

void UShopScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UWorld* world = GetWorld();

	if(!world) return;

	if(AValoraintGameState* GameState = world->GetGameState<AValoraintGameState>())
	{
		TArray<UWeaponData*> PrimaryWeapons = GameState->AllWeapons.FilterByPredicate([](const UWeaponData* weapon)->bool
		{
			return weapon->GunType == EGunType::PrimaryWeapon;
		});
		TArray<UWeaponData*> SecondaryWeapons = GameState->AllWeapons.FilterByPredicate([](const UWeaponData* weapon)->bool
		{
			return weapon->GunType == EGunType::SecondaryWeapon;
		});
		PrimaryWeaponsLayout->SetListItems<UWeaponData*>(PrimaryWeapons);
		SecondaryWeaponsLayout->SetListItems<UWeaponData*>(SecondaryWeapons);
	}
	
}
