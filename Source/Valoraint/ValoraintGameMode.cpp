// Copyright Epic Games, Inc. All Rights Reserved.

#include "ValoraintGameMode.h"
#include "ValoraintHUD.h"
#include "ValoraintCharacter.h"
#include "UObject/ConstructorHelpers.h"

AValoraintGameMode::AValoraintGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AValoraintHUD::StaticClass();
}
