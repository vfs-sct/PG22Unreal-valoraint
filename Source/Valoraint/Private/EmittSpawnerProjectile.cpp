// Copyright (C) Jim Chen 2022, All Rights Reserved


#include "EmittSpawnerProjectile.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AEmittSpawnerProjectile::AEmittSpawnerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	SetRootComponent(Collider);
	Mesh->SetupAttachment(Collider);

	ProjMoveComp->SetUpdatedComponent(Collider);
	ProjMoveComp->bShouldBounce = true;
	bReplicates = true;
	// initial life span to prevent projectile not destroying itself
	InitialLifeSpan = 5.0f;
}


void AEmittSpawnerProjectile::SpawnEmittServer_Implementation(FVector Location)
{
	// Spawn Emitter by multicast
	SpawnEmittMulticast(Location);

	// decrease bounce time, if bounce time reach to zero, destroy self
	TotalBounceTime--;
	if(TotalBounceTime <= 0) Destroy();
}
// validate function to prevent player cheat
bool AEmittSpawnerProjectile::SpawnEmittServer_Validate(FVector Location)
{
	return true;
}

void AEmittSpawnerProjectile::SpawnEmittMulticast_Implementation(FVector Location)
{
	// get world reference
	const UWorld* world = GetWorld();

	// if world or blockingParticle is nullptr, return without SpawnEmitter 
	if(BlockingParticle == nullptr) return;
	if(world == nullptr) return;

	// Spawn Emitter At Location
	UParticleSystemComponent* SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(world, BlockingParticle, Location);

}

// Called when the game starts or when spawned
void AEmittSpawnerProjectile::BeginPlay()
{
	Super::BeginPlay();
	// Delegate OnHit function
	Collider->OnComponentHit.AddDynamic(this, &AEmittSpawnerProjectile::OnHit);
}

// Called every frame
void AEmittSpawnerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEmittSpawnerProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// grab Hit location as emitter spawning location
	FVector SpawnPos = Hit.Location;
	
	// Server function to make Emitter Spawn, pass spawn location as parameter
	SpawnEmittServer(SpawnPos);
}


