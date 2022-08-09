// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "HideSpotsGenerator.generated.h"

/**
 * 
 */
UCLASS()
class VALORAINT_API UHideSpotsGenerator : public UEnvQueryGenerator
{
	GENERATED_BODY()
public:
	UHideSpotsGenerator();
	
protected:
	virtual void GenerateItems(FEnvQueryInstance & QueryInstance) const override;
};
