// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "Currency.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"

class AValoraintProjectile;
/**
 * 
 */
UENUM(BlueprintType)
enum class EGunType : uint8
{
	PrimaryWeapon,
	SecondaryWeapon
};

UCLASS(BlueprintType, Blueprintable)
class VALORAINT_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FText GunName;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float DamageAmount = 10.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int32 MagazineSize = 5;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float FireRate = 0.2f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FCurrency Cost;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	USkeletalMesh* GunMesh;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	TSubclassOf<AValoraintProjectile> AmmoType;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	EGunType GunType;
};
