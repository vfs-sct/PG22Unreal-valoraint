// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "Currency.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class VALORAINT_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float DamageAmount = 10.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int32 MagazineSize = 5;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FCurrency Cost;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UStaticMesh* GunMesh;
};
