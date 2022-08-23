// Copyright Epic Games, Inc. All Rights Reserved.

#include "ValoraintCharacter.h"

#include "SkeletalRenderPublic.h"
#include "ValoraintProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Data/WeaponData.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AValoraintCharacter

AValoraintCharacter::AValoraintCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	NetworkedCharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Networked Mesh"));
	NetworkedCharacterMesh->SetOwnerNoSee(true);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FP_Gun->GetSocketLocation("Muzzle"));
	
	// Create secondary gun mesh component
	SecondaryGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SecondaryWeapon"));
	SecondaryGun->bCastDynamicShadow = false;
	SecondaryGun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	SecondaryGun->SetupAttachment(RootComponent);
	SecondaryGun->SetOwnerNoSee(true);
	
	SecondaryMuzzle = CreateDefaultSubobject<USceneComponent>(TEXT("SecondaryMuzzle"));
	SecondaryMuzzle->SetupAttachment(SecondaryGun);
	SecondaryMuzzle->SetRelativeLocation(SecondaryGun->GetSocketLocation("Muzzle"));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	NetworkedCharacterMesh->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AValoraintCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true), TEXT("GripPoint"));
	SecondaryGun->AttachToComponent(NetworkedCharacterMesh, FAttachmentTransformRules(EAttachmentRule::KeepRelative,true), TEXT("HolsterPoint"));
	bIsPrimaryEquipped = true;
	SetupWeapons();
	Mesh1P->SetHiddenInGame(false, true);
}


void AValoraintCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Returning replicated individual bools back to individual AValoraintCharacter
	DOREPLIFETIME_CONDITION(AValoraintCharacter, bCanFirstAbility, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AValoraintCharacter, bCanSecondAbility, COND_OwnerOnly);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AValoraintCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AValoraintCharacter::Shoot);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AValoraintCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AValoraintCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Fire Abilities
	//PlayerInputComponent->BindAction("FirstAbility", IE_Pressed, this, &AValoraintCharacter::FirstAbilityServer);
	PlayerInputComponent->BindAction("FirstAbility", IE_Pressed, this, &AValoraintCharacter::FireFirstAbility);
	PlayerInputComponent->BindAction("SecondAbility", IE_Pressed, this, &AValoraintCharacter::FireSecondAbility);
	
	// Swap Weapons
	PlayerInputComponent->BindAction("SwapWeapon", IE_Pressed, this, &AValoraintCharacter::SwapWeapons);
}

void AValoraintCharacter::Shoot_Implementation()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			const USceneComponent* Muzzle = (bIsPrimaryEquipped ? FP_MuzzleLocation : SecondaryMuzzle);
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((Muzzle != nullptr) ? Muzzle->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AValoraintProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// First Ability

void AValoraintCharacter::FireFirstAbility()
{
	// check
	CheckCanFirstAbility();

	// if bool false, return
	if(!CanFirstAbility()) return;

	// fire server first ability function
	FirstAbilityServer();
}

void AValoraintCharacter::CheckCanFirstAbility_Implementation()
{
	// if this actor has no network authority, or world reference is nullptr
	if (!HasAuthority()) return;

	const UWorld* world = GetWorld();
	if (world == nullptr) return;
		
	// if dash time is in cooldown, flip bool to true
	if(LastDashTime + DashCoolDown < UKismetSystemLibrary::GetGameTimeInSeconds(world))
	{
		bCanFirstAbility = true;
	}
	
}

bool AValoraintCharacter::CanFirstAbility() const
{
	return bCanFirstAbility;
}

void AValoraintCharacter::FirstAbilityServer_Implementation()
{
	// do first ability function from ability interface
	Execute_FirstAbility(this);

	// grab world
	const UWorld* world = GetWorld();

	// Set up last dash time in server
	LastDashTime = UKismetSystemLibrary::GetGameTimeInSeconds(world);

	// Make dash in cooldown
	bCanFirstAbility = false;
	
	// do first ability particle effect
	FirstAbilityNetMulticast();
}

bool AValoraintCharacter::FirstAbilityServer_Validate()
{
	return true;
}

void AValoraintCharacter::FirstAbility_Implementation()
{
	// implementation of first ability from ability interface
	IAbilityInterface::FirstAbility_Implementation();
	
	// Launch character to perform dashing 
	LaunchCharacter(GetActorForwardVector() * DashDistance, true, false);
}

void AValoraintCharacter::FirstAbilityNetMulticast_Implementation()
{
	//TODO Try to run particle effects/VFX
}

//////////////////////////////////////////////////////////////////////////
// Second Ability

void AValoraintCharacter::FireSecondAbility()
{
	// check
	CheckCanSecondAbility();

	// if check is true, fire Second ability server function
	if(!CanSecondAbility()) return;
		
	SecondAbilityServer();
}

bool AValoraintCharacter::CanSecondAbility() const
{
	return bCanSecondAbility;
}

void AValoraintCharacter::CheckCanSecondAbility_Implementation()
{
	// if this actor has no authority, or world reference is nullptr, return
	if (!HasAuthority()) return;

	const UWorld* world = GetWorld();
	if (world == nullptr) return;
		
	// if number of emitter spawner is bigger, player still can fire, else, false	
	if(NumOfEmiSpawner > 0)
		bCanSecondAbility = true;
	else
		bCanSecondAbility = false;
}


void AValoraintCharacter::SecondAbilityServer_Implementation()
{
	// do second ability function from ability interface
	Execute_SecondAbility(this);

	// Server side reduce 1 emitter projectile 
	NumOfEmiSpawner--;
	
	// do second ability particle effect
	SecondAbilityNetMulticast();
}

void AValoraintCharacter::SecondAbility_Implementation()
{
	UWorld* const world = GetWorld();

	// try and fire a projectile
	if (EmitterSpawnProjectile != nullptr)
	{
		if (world != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			const USceneComponent* Muzzle = (bIsPrimaryEquipped ? FP_MuzzleLocation : SecondaryMuzzle);
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((Muzzle != nullptr) ? Muzzle->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the emitter Spawner/ at the muzzle
			world->SpawnActor<AEmittSpawnerProjectile>(EmitterSpawnProjectile, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
}

bool AValoraintCharacter::SecondAbilityServer_Validate()
{
	return true;
}

void AValoraintCharacter::SecondAbilityNetMulticast_Implementation()
{
	
}

//////////////////////////////////////////////////////////////////////////
// Swap Weapon

void AValoraintCharacter::SwapWeapons_Implementation()
{
	if(!PrimaryWeapon || !SecondaryWeapon) return;

	if(bIsPrimaryEquipped)
	{
		SwapWeaponsInternal(FP_Gun, SecondaryGun);
	} else
	{
		SwapWeaponsInternal(SecondaryGun, FP_Gun);
	}
	
	bIsPrimaryEquipped = !bIsPrimaryEquipped;
}



void AValoraintCharacter::SetupWeapons() const
{
	if(PrimaryWeapon) {
		FP_Gun->SetSkeletalMesh(PrimaryWeapon->GunMesh);
	}
	
	if(!SecondaryWeapon) return;
	SecondaryGun->SetSkeletalMesh(SecondaryWeapon->GunMesh);
}

void AValoraintCharacter::SwapWeaponsInternal(USkeletalMeshComponent* Primary, USkeletalMeshComponent* Secondary)
{
	Secondary->DetachFromComponent({EDetachmentRule::KeepRelative, false});
	Secondary->SetOwnerNoSee(false);
	Secondary->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true), TEXT("GripPoint"));
	Primary->DetachFromComponent({EDetachmentRule::KeepRelative, false});
	Primary->SetOwnerNoSee(true);
	Primary->AttachToComponent(NetworkedCharacterMesh, FAttachmentTransformRules(EAttachmentRule::KeepRelative,true), TEXT("HolsterPoint"));
}

void AValoraintCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AValoraintCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}
