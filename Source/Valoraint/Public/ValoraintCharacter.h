// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilityInterface.h"
#include "EmittSpawnerProjectile.h"
#include "Data/WeaponData.h"
#include "GameFramework/Character.h"
#include "ValoraintCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class UWeaponData;
class AValoraintProjectile;

UCLASS(config=Game)
class AValoraintCharacter : public ACharacter, public IAbilityInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* NetworkedCharacterMesh;

	/** Primary Gun mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on primary gun where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;
	
	/** Secondary weapon mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* SecondaryGun;

	/** Location on secondary gun where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* SecondaryMuzzle;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	AValoraintCharacter();

protected:
	virtual void BeginPlay();

	
	// Function to return properties that are used for network replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeaponData* PrimaryWeapon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeaponData* SecondaryWeapon = nullptr;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	float Health;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<AValoraintProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	// Proxy for multicast swap, to ensure replication to all clients
	UFUNCTION(Server, Reliable)
	void ServerSwapWeapons();
	
	//////////////////////// First Ability /////////////////////////////////

	// Fire First Ability 
	void FireFirstAbility();
	
	// Server function with validation to prevent cheating
	UFUNCTION(Server, Reliable, WithValidation)
	void FirstAbilityServer();

	// Server function to check if player can fire first ability
	UFUNCTION(Server,Reliable)
	void CheckCanFirstAbility();
	
	// const function to return bCanDash
	bool CanFirstAbility() const;
	
	virtual void FirstAbility_Implementation() override;

	// Multicast function to spawn particle effects
	UFUNCTION(Server, Unreliable)
	void FirstAbilityNetMulticast();
	
	//////////////////////// Second Ability /////////////////////////////////

	// Fire Second Ability
	void FireSecondAbility();

	// Server function to check if player can fire second ability
	UFUNCTION(Server,Reliable)
	void CheckCanSecondAbility();

	// const function to return bCanSecondAbility
	bool CanSecondAbility() const;
	
	// Server function with validation to prevent cheating
	UFUNCTION(Server, Reliable, WithValidation)
	void SecondAbilityServer();
	
	virtual void SecondAbility_Implementation() override;

	// Multicast function to spawn particle effects
	UFUNCTION(NetMulticast, Unreliable)
	void SecondAbilityNetMulticast();

	UFUNCTION(Server, Reliable)
	void Hit();

	// Proxy for multicast set up, so it replicates to clients
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSetupWeapons() const;
	
	// Proxy for multicast set up, so it replicates to clients
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerAddPurchasedWeapons(UWeaponData* Primary, UWeaponData* Secondary);

protected:

	void SwapWeaponsInternal(USkeletalMeshComponent* Primary, USkeletalMeshComponent* Secondary);

	UFUNCTION(Server, Reliable)
	void Shoot();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	bool bIsPrimaryEquipped;

	// Property for first ability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dash)
	float DashCoolDown = 6;

	// Replicated property for network to replicate back to owner 
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Dash)
	bool bCanFirstAbility = true;

	UPROPERTY(BlueprintReadWrite, Category = Dash)
	float LastDashTime = 0;	

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Dash)
	float DashDistance = 200;

	
	// Property for Second ability
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=SecondAbility)
	TSubclassOf<AEmittSpawnerProjectile> EmitterSpawnProjectile;

	// Replicated properties for network to replicate back to owner 
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category=SecondAbility)
	int32 NumOfEmiSpawner = 3;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = SecondAbility)
	bool bCanSecondAbility = true;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwapWeapons();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddPurchasedWeapons(UWeaponData* Primary, UWeaponData* Secondary);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetupWeapons() const;
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

