// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
//#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ShopScreen.generated.h"

/**
 * 
 */
class UListView;
UCLASS()
class VALORAINT_API UShopScreen : public UUserWidget // public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UListView* PrimaryWeaponsLayout;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UListView* SecondaryWeaponsLayout;

	virtual void NativeOnInitialized() override;
};
