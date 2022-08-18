// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


#include "ValoraintPlayerController.h"

#include "ValoraintHUD.h"

void AValoraintPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(!InputComponent) return;

	UWorld* world = GetWorld();
	if(!world) return;
	
	InputComponent->BindAction("Shop", IE_Pressed, this, &AValoraintPlayerController::ToggleShop);
}

void AValoraintPlayerController::ToggleShop()
{
	if(AValoraintHUD* hud = GetHUD<AValoraintHUD>())
	{
		hud->ToggleShop();
	}
}
