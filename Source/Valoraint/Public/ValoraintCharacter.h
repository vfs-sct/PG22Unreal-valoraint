// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilityInterface.h"
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
	
	UFUNCTION(NetMulticast, Unreliable)
	void SwapWeapons();

	UFUNCTION(BlueprintCallable)
	bool CanDash() const;

	// Server function with validation to prevent cheating
	UFUNCTION(Server, Reliable, WithValidation)
	void FirstAbilityServer();
	
	UFUNCTION(NetMulticast, Unreliable)
	void FirstAbilityNetMulticast();

	virtual void FirstAbility_Implementation() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void SecondAbilityServer();
	
	UFUNCTION(NetMulticast, Unreliable)
	void SecondAbilityNetMulticast();

	virtual void SecondAbility_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AValoraintProjectile> FlashbangProjectile;

protected:

	void SetupWeapons() const;

	void SwapWeaponsInternal(USkeletalMeshComponent* Primary, USkeletalMeshComponent* Secondary);

	UFUNCTION(Server, Reliable)
	void Shoot();
	
	/** Fires a projectile. */
	void OnFire();

	// Fire First Ability 
	void FireFirstAbility();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	bool bIsPrimaryEquipped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dash)
	float DashCoolDown = 6;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Dash)
	bool bCanDash = true;

	UPROPERTY(BlueprintReadWrite, Category = Dash)
	float LastDashTime = 0;	

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Dash)
	float DashDistance = 200;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

