// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ValoraintProjectile.generated.h"

class AValoraintCharacter;
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class VALORAINT_API AValoraintProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AValoraintProjectile();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* Collider;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProjectileMovementComponent* ProjMoveComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	bool hasHit;

	float Damage;
	AValoraintCharacter* Instigator;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	UFUNCTION(Server, Reliable)
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	void Initialize(float damage, AValoraintCharacter* instigator);
};
