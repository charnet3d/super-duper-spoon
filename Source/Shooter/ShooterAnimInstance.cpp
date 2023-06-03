// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip),
	CharacterRotation(FRotator(0)),
	CharacterRotationLastFrame(FRotator(0)),
	YawDelta(0.f),
	RecoilWeight(1.f),
	bTurningInPlace(false)
{
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!ShooterCharacter)
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

	if (!ShooterCharacter)	return;

	
	// Get the lateral speed of the character from velocity
	FVector Velocity{ ShooterCharacter->GetVelocity() };
	Velocity.Z = 0;
	Speed = Velocity.Size();

	// Is the character in the air
	bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

	// Is the character accelerating
	bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;

	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	if (ShooterCharacter->GetVelocity().Size() > 0)
	{
		LastMovementOffsetYaw = MovementOffsetYaw;
	}

	bAiming = ShooterCharacter->GetAiming();
	bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
	bCrouching = ShooterCharacter->IsCrouching();
	
	if (bReloading)
	{
		OffsetState = EOffsetState::EOS_Reloading;
	}
	else if (bIsInAir)
	{
		OffsetState = EOffsetState::EOS_InAir;
	}
	else if (bAiming)
	{
		OffsetState = EOffsetState::EOS_Aiming;
	}
	else
	{
		OffsetState = EOffsetState::EOS_Hip;
	}
		
	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (!ShooterCharacter)	return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch; 
	
	if (Speed > 0 || bIsInAir)
	{
		// Don't want to turn in place; Character is moving
		RootYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurve = 0.f;
		RotationCurveLastFrame = 0.f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta { TIPCharacterYaw - TIPCharacterYawLastFrame };
		
		// Root Yaw Offset, updated and clamped to [-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		// 1.0 if turning, 0.0 if not
		const float Turning { GetCurveValue(TEXT("Turning")) };
		if (Turning)
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation { RotationCurve = RotationCurveLastFrame };

			// If RootYawOffset > 0, -> Turning Left, If RootYawOffset < 0, -> Turning Right
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset { FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset)
			{
				const float YawExcess { ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;	
			}
		}
		else
		{
			bTurningInPlace = false;
		}
	}

	// Set the recoil weight
	if (bTurningInPlace)
	{
		if (bReloading)
		{
			RecoilWeight = 1.f;
		}
		else
		{
			RecoilWeight = 0.f;
		}
	}
	else
	{
		if (bCrouching)
		{
			if (bReloading)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.1f;
			}
		}
		else
		{
			if (bAiming || bReloading)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.5f;
			}
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (!ShooterCharacter) return;
	
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	const FRotator Delta { UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };
	const double Target { Delta.Yaw / DeltaTime };
	const double Interp { FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f) };
	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
	
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(
	// 		3,
	// 		-1,
	// 		FColor::Cyan,
	// 		FString::Printf(TEXT("YawDelta: %f"), YawDelta)
	// 	);
	// }
}
