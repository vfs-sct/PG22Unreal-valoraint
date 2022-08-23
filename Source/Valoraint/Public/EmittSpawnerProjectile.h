// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EmittSpawnerProjectile.generated.h"

UCLASS()
class VALORAINT_API AEmittSpawnerProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEmittSpawnerProjectile();

	// collider
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* Collider;

	// mesh component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Mesh;

	// projectile movement for emitter spawner projectile
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProjectileMovementComponent* ProjMoveComp;

	// blocking particle to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* BlockingParticle;

	// total bounce time
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalBounceTime = 3;

	// server and net multicast functions
	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnEmittServer(FVector Location);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnEmittMulticast(FVector Location);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Delegate UFunction
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
	
};
