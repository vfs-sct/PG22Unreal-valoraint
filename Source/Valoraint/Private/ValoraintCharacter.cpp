// Copyright (C) Shatrujit Aditya Kumar, Jim Chen, Andre Dupuis 2022, All Rights Reserved

#include "ValoraintCharacter.h"

#include "ValoraintProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Data/WeaponData.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "MotionControllerComponent.h"
#include "ValoraintPlayerState.h"

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
	Health = 100.0f;

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
	PrimaryGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	PrimaryGun->bCastDynamicShadow = false;
	PrimaryGun->CastShadow = false;
	PrimaryGun->SetIsReplicated(true);
	PrimaryGun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(PrimaryGun);
	FP_MuzzleLocation->SetRelativeLocation(PrimaryGun->GetSocketLocation("Muzzle"));
	
	// Create secondary gun mesh component
	SecondaryGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SecondaryWeapon"));
	SecondaryGun->bCastDynamicShadow = false;
	SecondaryGun->CastShadow = false;
	SecondaryGun->SetupAttachment(RootComponent);
	SecondaryGun->SetIsReplicated(true);
	SecondaryGun->SetOwnerNoSee(true);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	NetworkedCharacterMesh->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AValoraintCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	LastFireTime = NAN;
	PrimaryMagazineUse = 0;
	SecondaryMagazineUse = 0;
	
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	PrimaryGun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true), TEXT("GripPoint"));
	SecondaryGun->AttachToComponent(NetworkedCharacterMesh, FAttachmentTransformRules(EAttachmentRule::KeepRelative,true), TEXT("HolsterPoint"));
	bIsPrimaryEquipped = true;
	Mesh1P->SetHiddenInGame(false, true);

	FTimerHandle SetupTimer;
	GetWorldTimerManager().SetTimer(SetupTimer, this, &AValoraintCharacter::ServerSetupWeapons, 2.0f, false);
	ServerSetupWeapons();
}

void AValoraintCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Couldn't get IE_Repeat working so turned a bool on/off on IE_Pressed and IE_Released
	if(bIsShooting) Shoot();
}


void AValoraintCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Returning replicated individual bools back to individual AValoraintCharacter
	DOREPLIFETIME_CONDITION(AValoraintCharacter, bCanFirstAbility, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AValoraintCharacter, bCanSecondAbility, COND_OwnerOnly);

	// Replicate magazine size back to the individual client
	DOREPLIFETIME_CONDITION(AValoraintCharacter, PrimaryMagazineUse, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AValoraintCharacter, SecondaryMagazineUse, COND_OwnerOnly);
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
	// Couldn't get IE_Repeat working so turned a bool on/off on IE_Pressed and IE_Released
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AValoraintCharacter::StartShooting);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AValoraintCharacter::StopShooting);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AValoraintCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AValoraintCharacter::MoveRight);

	// Bind Rotation
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Fire Abilities
	//PlayerInputComponent->BindAction("FirstAbility", IE_Pressed, this, &AValoraintCharacter::FirstAbilityServer);
	PlayerInputComponent->BindAction("FirstAbility", IE_Pressed, this, &AValoraintCharacter::FireFirstAbility);
	PlayerInputComponent->BindAction("SecondAbility", IE_Pressed, this, &AValoraintCharacter::FireSecondAbility);
	
	// Swap Weapons
	PlayerInputComponent->BindAction("SwapWeapon", IE_Pressed, this, &AValoraintCharacter::ServerSwapWeapons);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AValoraintCharacter::Reload);
}

void AValoraintCharacter::Shoot_Implementation()
{
	// Get the currently equipped weapon
	const UWeaponData* Equipped = bIsPrimaryEquipped ? PrimaryWeapon : SecondaryWeapon;
	const int32 magUse = bIsPrimaryEquipped ? PrimaryMagazineUse : SecondaryMagazineUse;
	const TSubclassOf<AValoraintProjectile> ProjectileClass = Equipped->AmmoType;
	
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();

		// Return if fire rate hasn't elapsed yet
		if(!isnan(LastFireTime) && UKismetSystemLibrary::GetGameTimeInSeconds(World) - LastFireTime < Equipped->FireRate) return;

		// If we're out of ammo, print a message and stop shooting
		if(magUse >= Equipped->MagazineSize)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, TEXT("Out of Ammo, 'R' to reload"));
			StopShooting();
			return;
		}
		
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			const FVector SpawnLocation = (FP_MuzzleLocation != nullptr ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + GetActorForwardVector() * 50;

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			AValoraintProjectile* Projectile = World->SpawnActor<AValoraintProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if(Projectile)
			{
				// Init damage and instigator, then adjust last fire time and relevant magazine size
				Projectile->Initialize(Equipped->DamageAmount, this);
				LastFireTime = UKismetSystemLibrary::GetGameTimeInSeconds(World);
				if(bIsPrimaryEquipped) ++PrimaryMagazineUse;
				else ++SecondaryMagazineUse;
			}
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
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = (FP_MuzzleLocation != nullptr ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

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

void AValoraintCharacter::ServerSwapWeapons_Implementation()
{
	MulticastSwapWeapons();
}

void AValoraintCharacter::MulticastSwapWeapons_Implementation()
{
	if(!PrimaryWeapon || !SecondaryWeapon) return;

	if(bIsPrimaryEquipped)
	{
		SwapWeaponsInternal(PrimaryGun, SecondaryGun);
	} else
	{
		SwapWeaponsInternal(SecondaryGun, PrimaryGun);
	}
	LastFireTime = NAN;
	bIsPrimaryEquipped = !bIsPrimaryEquipped;
}

// Switches the positions of primary and secondary weapons, changes visibility rules and resets the muzzle position
void AValoraintCharacter::SwapWeaponsInternal(USkeletalMeshComponent* Primary, USkeletalMeshComponent* Secondary)
{
	Secondary->DetachFromComponent({EDetachmentRule::KeepRelative, false});
	Secondary->SetOwnerNoSee(false);
	Secondary->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true), TEXT("GripPoint"));
	
	SetMuzzle(Secondary);
	
	Primary->DetachFromComponent({EDetachmentRule::KeepRelative, false});
	Primary->SetOwnerNoSee(true);
	Primary->AttachToComponent(NetworkedCharacterMesh, FAttachmentTransformRules(EAttachmentRule::KeepRelative,true), TEXT("HolsterPoint"));
}

// Sets the muzzle location to the new SK component's Muzzle socket
void AValoraintCharacter::SetMuzzle(USkeletalMeshComponent* Gun) const
{
	FP_MuzzleLocation->DetachFromComponent({EDetachmentRule::KeepRelative, false});
	FP_MuzzleLocation->AttachToComponent(Gun, FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true), TEXT("Muzzle"));
}

// Resets magazine use for active weapon
void AValoraintCharacter::Reload_Implementation()
{
	if(bIsPrimaryEquipped)
	{
		PrimaryMagazineUse = 0;
		return;
	}

	SecondaryMagazineUse = 0;
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

// Toggles for start and stop shooting
void AValoraintCharacter::StartShooting()
{
	bIsShooting = true;
}

void AValoraintCharacter::StopShooting()
{
	bIsShooting = false;
}

void AValoraintCharacter::Hit_Implementation(AValoraintProjectile* Projectile)
{
	Health -= Projectile->Damage;
	if(Health <= 0)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Green, "DIED" );
		AValoraintPlayerState* InstigatorState = Cast<AValoraintPlayerState>(Projectile->Instigator->GetPlayerState());
		if(InstigatorState) InstigatorState->PlayerWallet = InstigatorState->PlayerWallet + Bounty;
		Health = 100;
	}
	FString TheFloatStr = FString::SanitizeFloat(Health);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Green, *TheFloatStr );
	
}

// Wrapper for the multicast
void AValoraintCharacter::ServerSetupWeapons_Implementation() const
{
	MulticastSetupWeapons();
}

// Sets the skeletal mesh if the Weapon Data asset exists
void AValoraintCharacter::MulticastSetupWeapons_Implementation() const
{
	if(PrimaryWeapon) {
		PrimaryGun->SetSkeletalMesh(PrimaryWeapon->GunMesh);
	}

	if(!SecondaryWeapon) return;
	SecondaryGun->SetSkeletalMesh(SecondaryWeapon->GunMesh);
}

// Wrapper for the multicast
void AValoraintCharacter::ServerAddPurchasedWeapons_Implementation(UWeaponData* Primary, UWeaponData* Secondary)
{
	MulticastAddPurchasedWeapons(Primary, Secondary);
}

// Changes equipped primary and secondary weapons and resets the magazine count if valid
// Resets Muzzle location for the active weapon
void AValoraintCharacter::MulticastAddPurchasedWeapons_Implementation(UWeaponData* Primary,
	UWeaponData* Secondary)
{
	if(Primary)
	{
		PrimaryWeapon = Primary;
		PrimaryMagazineUse = 0;
		if(bIsPrimaryEquipped) SetMuzzle(PrimaryGun);
	}
	if(Secondary)
	{
		SecondaryWeapon = Secondary;
		SecondaryMagazineUse = 0;
		if(!bIsPrimaryEquipped) SetMuzzle(SecondaryGun);
	}

	ServerSetupWeapons();
}
