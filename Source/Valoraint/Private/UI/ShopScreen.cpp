// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


#include "UI/ShopScreen.h"
#include "Valoraint/Public/Data/WeaponData.h"
#include "Components/TileView.h"
#include "Valoraint/ValoraintGameMode.h"

void UShopScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UWorld* world = GetWorld();

	if(!world) return;

	if(AValoraintGameMode* GameMode = world->GetAuthGameMode<AValoraintGameMode>())
	{
		TArray<UWeaponData*> PrimaryWeapons = GameMode->AllWeapons.FilterByPredicate([](const UWeaponData* weapon)->bool
		{
			return weapon->GunType == EGunType::PrimaryWeapon;
		});
		TArray<UWeaponData*> SecondaryWeapons = GameMode->AllWeapons.FilterByPredicate([](const UWeaponData* weapon)->bool
		{
			return weapon->GunType == EGunType::SecondaryWeapon;
		});
		PrimaryWeaponsGrid->SetListItems<UWeaponData*>(PrimaryWeapons);
		SecondaryWeaponsGrid->SetListItems<UWeaponData*>(SecondaryWeapons);
	}
	
}
