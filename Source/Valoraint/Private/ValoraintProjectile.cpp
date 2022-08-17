// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


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
	bReplicates = true;

	hasHit = false;
}

// Called when the game starts or when spawned
void AValoraintProjectile::BeginPlay()
{
	Super::BeginPlay();
	Collider->OnComponentHit.AddDynamic(this, &AValoraintProjectile::OnHit);
}

// Called every frame
void AValoraintProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AValoraintProjectile::OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	AValoraintCharacter* Player = Cast<AValoraintCharacter>(OtherActor);
	if(Player && hasHit == false)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Green, "Hit");
		Player->Hit();
		hasHit = true;
		Destroy();
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Green, "Not Hit");
		Destroy();
	}
	
	
}