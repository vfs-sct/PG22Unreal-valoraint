// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved
#pragma once

#include "Currency.generated.h"

USTRUCT(BlueprintType)
struct FCurrency
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float InGameCurrent;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float PremiumCurrency;

	// Override addition/subtraction for easy transactions
	FCurrency operator+(const FCurrency& Other) const
	{
		return FCurrency{(this->InGameCurrent + Other.InGameCurrent), (this->PremiumCurrency + Other.PremiumCurrency)};
	}

	FCurrency operator-(const FCurrency& Other) const
	{
		return FCurrency{(this->InGameCurrent - Other.InGameCurrent), (this->PremiumCurrency - Other.PremiumCurrency)};
	}
	
};
