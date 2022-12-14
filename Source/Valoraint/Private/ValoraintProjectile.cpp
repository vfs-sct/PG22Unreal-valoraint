// Copyright (C) Shatrujit Aditya Kumar, Andre Dupuis 2022, All Rights Reserved


#include "ValoraintProjectile.h"

#include "ValoraintCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AValoraintProjectile::AValoraintProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	SetRootComponent(Collider);
	Mesh->SetupAttachment(Collider);
	Collider->OnComponentHit.AddDynamic(this, &AValoraintProjectile::OnHit);

	ProjMoveComp->SetUpdatedComponent(Collider);

	ProjMoveComp->InitialSpeed = 3000;
	ProjMoveComp->MaxSpeed = 3000;
	ProjMoveComp->ProjectileGravityScale = 0.0f;
	bReplicates = true;

	hasHit = false;
	Damage = 0.0f;
	Instigator = nullptr;
}


// Called when the game starts or when spawned
void AValoraintProjectile::BeginPlay()
{
	Super::BeginPlay();
	Collider->OnComponentHit.AddDynamic(this, &AValoraintProjectile::OnHit);
}

// Set it's damage and instigator and ignore the instifator
void AValoraintProjectile::Initialize(float damage, AValoraintCharacter* instigator)
{
	this->Damage = damage;
	this->Instigator = instigator;
	Collider->IgnoreActorWhenMoving(Instigator, true);
}

// Damage target if it's a player, destroy regardless
void AValoraintProjectile::OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	AValoraintCharacter* Player = Cast<AValoraintCharacter>(OtherActor);
	if(Player && hasHit == false)
	{
		Player->Hit(this);
		hasHit = true;
	}
	
	Destroy();
}
