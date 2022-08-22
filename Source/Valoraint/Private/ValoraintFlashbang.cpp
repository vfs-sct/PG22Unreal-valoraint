// Copyright (C) Shatrujit Aditya Kumar 2022, All Rights Reserved


#include "ValoraintFlashbang.h"

#include "Interfaces/Interface_PostProcessVolume.h"

// Sets default values
AValoraintFlashbang::AValoraintFlashbang()
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
	
}

void AValoraintFlashbang::OnDestroy(AActor* DestroiedActor)
{
	
}


void AValoraintFlashbang::FlashbangServer_Implementation()
{
	const UWorld* world = GetWorld();

	FPostProcessVolumeProperties volume = world->PostProcessVolumes[0]->GetProperties();
	volume.Settings
	
	
	//FlashbangMulticast();
}
bool AValoraintFlashbang::FlashbangServer_Validate()
{
	return true;
}

void AValoraintFlashbang::FlashbangMulticast_Implementation()
{
	
}

// Called when the game starts or when spawned
void AValoraintFlashbang::BeginPlay()
{
	Super::BeginPlay();
	OnDestroyed.AddDynamic(this, &AValoraintFlashbang::OnDestroy);
}

// Called every frame
void AValoraintFlashbang::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

