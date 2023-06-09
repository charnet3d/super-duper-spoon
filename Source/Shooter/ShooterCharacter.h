// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "AmmoType.h"

#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USoundCue;
class UParticleSystem;
class UAnimMontage;
class AItem;
class AWeapon;
class AAmmo;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	
	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	// Scene Component to use for its location for inteping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	// Number of items interping to/at this scene comp location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Called for forwards/backwards input */
	void MoveForward(float Value);

	/* Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired rate
	 */
	void LookUpAtRate(float Rate);

	/**
	 * Rotate controller based on mouse X movement
	 *
	 * @param Value The input value from mouse movement
	 */
	void Turn(float Value);

	/**
	 * Rotate controller based on mouse Y movement
	 *
	 * @param Value	The input value from mouse movement
	 */
	void LookUp(float Value);

	/**
	 * Called when the Fire button is pressed
	 */
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/* Set bAiming to true or false with button press */
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	/* Set BaseTurnRate and BaseLookUpRate based on aiming */
	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/** Line trace for items under the crosshairs */
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/* Trace for items if OverlappedItemCount > 0 */
	void TraceForItems();

	void SetItemPickupWidgetVisibility(AItem* Item, bool visibility);

	/* Spawns a default weapon and equips it */
	AWeapon* SpawnDefaultWeapon();

	/* Takes a weapon and attaches it to the mesh */
	void EquipWeapon(AWeapon* WeaponToEquip);

	/** Detach weapon and let it fall to the ground */
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	/** Drops currently equipped weapon and equips TraceHitItem */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/** Initialize the ammo map with ammo values */
	void InitializeAmmoMap();

	/** Check to make sure this weapon has ammo */
	bool WeaponHasAmmo();

	/** FireWeapon functions */
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	/** For reloading the weapon */
	void ReloadButtonPressed();
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	
	/** Check to see if we have ammo of the EquippedWeapon's ammo type */
	bool CarryingAmmo();

	/** Called from Animation Blueprint with GrabClip notify */
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	
	/** Called from Animation Blueprint with ReleaseClip notify */
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

	virtual void Jump() override;

	/** Interps capsule half height when crouching / standing */
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();

	void StopAiming();
	
	void PickupAmmo(AAmmo* Ammo);

	void InitializeInterpLocations();

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	/* Base turn rate, in deg/sec. Other scaling may affect final rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float BaseTurnRate;

	/* Base look up/down rate, in deg/sec. Other scaling may affect final rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float BaseLookUpRate;

	/* Turn rate while not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float HipTurnRate;

	/* Look up rate while not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float HipLookUpRate;

	/* Turn rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float AimingTurnRate;

	/* Look up rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float AimingLookUpRate;

	/* Scale factor for mouse look sensitivity. Turn rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true),
		meta = (ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
	float MouseHipTurnRate;

	/* Scale factor for mouse look sensitivity. Look up rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true),
		meta = (ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
	float MouseHipLookUpRate;

	/* Scale factor for mouse look sensitivity. Turn rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true),
		meta = (ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
	float MouseAimingTurnRate;

	/* Scale factor for mouse look sensitivity. Look up rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true),
		meta = (ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
	float MouseAimingLookUpRate;

	/* Randomized gunshot sound cue*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	USoundCue* FireSound;

	/* Flash spawned at BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	UParticleSystem* MuzzleFlash;

	/* Montage for firing the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	UAnimMontage* HipFireMontage;

	/* Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	UParticleSystem* ImpactParticles;

	/* Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	UParticleSystem* BeamParticles;

	/* True when aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	bool bAiming;

	/* Default camera field of view value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	float CameraDefaultFOV;

	/* Field of view value when camera is zoomed in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	float CameraZoomedFOV;

	/* Current field of view this frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	float CameraCurrentFOV;

	/* Interp speed for zooming when aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	float ZoomInterpSpeed;

	/* Determines the spread of the crosshairs */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairSpreadMultiplier;

	/* Velocity component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairVelocityFactor;

	/* In air component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairInAirFactor;

	/* Aim component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairAimFactor;

	/* Shooting component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairShootingFactor;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	bool bFireButtonPressed;

	/* True when we cane fire. False when waiting for the timer */
	bool bShouldFire;

	/* Rate of automatic gun fire */
	float AutomaticFireRate;

	/* Sets a timer between gunshots */
	FTimerHandle AutoFireTimer;

	/* True if we should trace every frame for items */
	bool bShouldTraceForItems;

	/* Number of overlapped AItems */
	int8 OverlappedItemCount;

	/* Currently equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	AWeapon * EquippedWeapon;

	/* Set this in blueprints for the default weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/** Item currently hit by our trace in TraceForItems() (could be null) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	AItem* TraceHitItem;

	/** Distance outward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	float CameraInterpDistance;

	/** Distance upward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	float CameraInterpElevation;

	/** Map to keep track of ammo of the different weapon types */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	TMap<EAmmoType, int32> AmmoMap;

	/** Staring amount of 9mm ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = true))
	int32 Starting9mmAmmo;
	
	/** Staring amount of AR ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = true))
	int32 StartingARAmmo;

	/** Combat State can only fire or reload if Unoccupied */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	ECombatState CombatState;

	/* Montage for reloading the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadMontage;

	/* Transform of the clip when we first grab the clip during reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	FTransform ClipTransform;

	/* Scene component to attach to the Character's hand when reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	USceneComponent* HandSceneComponent;

	/** True when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bCrouching;

	/** Regular movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float BaseMovementSpeed;

	/** Crouch movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float CrouchMovementSpeed;

	/** Current half height of the capsule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	float CurrentCapsuleHalfHeight;
	
	/** When not crouching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	float StandingCapsuleHalfHeight;
	
	/** When crouching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	float CrouchingCapsuleHalfHeight;

	/** Ground friction while not crouching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	float BaseGroundFriction;
	
	/** Ground friction while crouching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	float CrouchingGroundFriction;

	/** Used for knowing when aiming button was pressed */
	bool bAimingButtonPressed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp6;
	
	/** Array of interp location structs */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;
	
	/** Time to wait before we can play another Pickup sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	float PickupSoundResetTime;
	
	/** Time to wait before we can play another Equip sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	float EquipSoundResetTime;

public:
	/* Returns CameraBoom SubObject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/* Returns FollowCamera SubObject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/* Adds/Subtracts to/from OverlappedItemCount and updates bShouldTraceForItems */
	void IncrementOverlappedItemCount(int8 Amount);

	// No longer needed; AItem has GetInterpLocation
	//FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	
	FORCEINLINE bool IsCrouching() const { return bCrouching; }

	FInterpLocation GetInterpLocation(int32 Index);
	
	// Returns the index in InterpLocations array with the lowest ItemCount
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();
};
