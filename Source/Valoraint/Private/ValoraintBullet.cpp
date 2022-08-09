// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


#include "ValoraintBullet.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AValoraintBullet::AValoraintBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	SetRootComponent(Collider);
	Mesh->SetupAttachment(Collider);

	ProjMoveComp->SetUpdatedComponent(Collider);
	bReplicates = true;
}

// Called when the game starts or when spawned
void AValoraintBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AValoraintBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

