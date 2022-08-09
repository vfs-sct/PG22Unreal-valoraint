// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


#include "HideSpotsGenerator.h"
#include "EngineUtils.h"
#include "HideSpots.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

UHideSpotsGenerator::UHideSpotsGenerator()
{
	ItemType = UEnvQueryItemType_Actor::StaticClass();
}

void UHideSpotsGenerator::GenerateItems(FEnvQueryInstance & QueryInstance) const
{
	Super::GenerateItems(QueryInstance);

	UWorld* World = GetWorld();

	if(!World) return;
	TArray<AActor*> MatchingActors;
	for(TActorIterator<AHideSpots> Iter(World); Iter; ++Iter)
	{
	    AActor* GeneratorActor = *Iter;
		MatchingActors.Add(GeneratorActor);
	}
	
	QueryInstance.AddItemData<UEnvQueryItemType_Actor>(MatchingActors);
}
