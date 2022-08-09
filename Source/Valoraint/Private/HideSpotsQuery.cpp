// Fill out your copyright notice in the Description page of Project Settings.


#include "HideSpotsQuery.h"

#include "EngineUtils.h"
#include "HideSpots.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UHideSpotsQuery::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	UWorld* World = GetWorld();

	if(!World) return;

	for(TActorIterator<AHideSpots> Iter(World); Iter; ++Iter)
	{
		QueryInstance.AddItemData<UEnvQueryItemType_Actor>(*Iter);
	}
}
