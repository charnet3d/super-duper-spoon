// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay()"));

	int myInt{ 42 };
	UE_LOG(LogTemp, Warning, TEXT("int myInt: %d"), myInt);

	float myFloat{ 3.14159f };
	UE_LOG(LogTemp, Warning, TEXT("float myFloat: %f"), myFloat);

	double myDouble{ 0.000080845 };
	UE_LOG(LogTemp, Warning, TEXT("double myDouble: %.10lf"), myDouble);

	char myChar{ 'c' };
	UE_LOG(LogTemp, Warning, TEXT("char myChar: %c"), myChar);

	wchar_t myWideChar{ L'J' };
	UE_LOG(LogTemp, Warning, TEXT("wchar_t myWideChar: %lc"), myWideChar);

	bool myBool{ true };
	UE_LOG(LogTemp, Warning, TEXT("bool myBool: %d"), myBool);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

