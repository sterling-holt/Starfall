// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/StarfallCharacter.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AStarfallCharacter::AStarfallCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStarfallMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	Self = this;


 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//	UCapsuleComponent* CollisionComponent = GetCapsuleComponent();
	//	if (CollisionComponent)
	//	{
	//		// Enable physics simulation
	//		CollisionComponent->SetSimulatePhysics(true);
	//		// Constrain rotation to prevent the character from falling over
	//		CollisionComponent->BodyInstance.bLockXRotation = true;
	//		CollisionComponent->BodyInstance.bLockYRotation = true;
	//		CollisionComponent->BodyInstance.bLockZRotation = false; // Allow rotation around Z-axis if needed
	//	
	//	
	//		// Configure additional physics settings as needed
	//		CollisionComponent->BodyInstance.SetMassOverride(75.0f); // Set mass (in kilograms)
	//		CollisionComponent->SetLinearDamping(0.1f); // Set linear damping
	//		CollisionComponent->SetAngularDamping(0.1f); // Set angular damping
	//	
	//		// More configuration as needed...
	//	}

	StarfallMovement = Cast<UStarfallMovementComponent>(GetCharacterMovement());
	StarfallMovement->CurrentState = EStarfallMovementState::Idle;


	UPROPERTY(BlueprintReadWrite, Category = "Mounted Components")
	Vitals = CreateDefaultSubobject<UACVitals>(TEXT("Vitals"));

	UPROPERTY(BlueprintReadWrite, Category = "Mounted Components")
	Arsenal = CreateDefaultSubobject<UStarfallArsenalComponent>(TEXT("Arsenal"));
}

// Called when the game starts or when spawned
void AStarfallCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStarfallCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AStarfallCharacter::Move(const FVector& Value)
{
	if (StarfallMovement)
	{
		// Set to Idle state when there's no input or not on the ground
		StarfallMovement->Move(Value);
	}
}

void AStarfallCharacter::Sprint(const FVector& Value)
{
	if (StarfallMovement)
	{
		StarfallMovement->Sprint(Value);
	}
}
void AStarfallCharacter::StopSprint(const FVector& Value)
{
	if (StarfallMovement)
	{
		StarfallMovement->StopSprint();
	}
}






//	Character Movement?
void AStarfallCharacter::StartJump(const FVector& Value, bool bCanJump)
{
	
	if (!bCanJump)
	{
		bool bCanJump = CanJump();	//	if no variable is passed in, fallback to character default
	}

	if (StarfallMovement)
	{
		StarfallMovement->Jump(true, bCanJump);
	}
}

void AStarfallCharacter::StopJump()
{
	if (StarfallMovement)
	{
		StarfallMovement->Jump(false, false);
	}
}


void AStarfallCharacter::Crouch(const FVector& Value)
{
	if (StarfallMovement)
	{
		StarfallMovement->Crouch(Value);
	}
}

void AStarfallCharacter::UnCrouch()
{
	if (StarfallMovement)
	{
		StarfallMovement->UnCrouch();
	}
}




UStarfallMovementComponent* AStarfallCharacter::GetStarfallMovementComponent() const
{
	return Cast<UStarfallMovementComponent>(GetMovementComponent());
}



bool AStarfallCharacter::IsOnGround()
{
	return StarfallMovement->IsMovingOnGround();
}




void AStarfallCharacter::Fire(const FVector& Value)
{
	if (Arsenal)
	{
		Arsenal->AimWeapon();
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("No weapon equipped"));
	}
}

void AStarfallCharacter::Aim(const FVector& Value)
{
	if (Arsenal)
	{
		Arsenal->FireWeapon();
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("No weapon equipped"));
	}
}