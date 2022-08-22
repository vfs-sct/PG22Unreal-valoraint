// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ValoraintFlashbang.generated.h"

UCLASS()
class VALORAINT_API AValoraintFlashbang : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AValoraintFlashbang();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* Collider;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProjectileMovementComponent* ProjMoveComp;

	UFUNCTION()
	void OnDestroy(AActor* DestroiedActor);

	UFUNCTION(Server, Reliable, WithValidation)
	void FlashbangServer();

	UFUNCTION(NetMulticast, Reliable)
	void FlashbangMulticast();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	

};
