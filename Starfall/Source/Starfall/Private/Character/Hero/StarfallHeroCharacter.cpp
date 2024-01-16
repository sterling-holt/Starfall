// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Hero/StarfallHeroCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AStarfallHeroCharacter::AStarfallHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    // Constructor logic (if any)

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	//	GetCharacterMovement()->JumpZVelocity = 300.f;
	//	GetCharacterMovement()->AirControl = 1.0f;
	//	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	//	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	//	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;


	// Create a camera boom (pulls in towards the player if there is a collision)
	//	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//	CameraBoom->SetupAttachment(GetMesh(), "head");
	//	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	//	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	//	
	//	
	//	// Create a follow camera
	//	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Create a follow camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), "head"); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FirstPersonCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

}

void AStarfallHeroCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


void AStarfallHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
	
		//	Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AStarfallHeroCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AStarfallHeroCharacter::StopJump);
		//	
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AStarfallHeroCharacter::Crouch);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AStarfallHeroCharacter::Move);
		
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AStarfallHeroCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AStarfallHeroCharacter::StopSprint);
	
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AStarfallHeroCharacter::Look);
	}
	else
	{
		//	UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


/*
*/
void AStarfallHeroCharacter::Move(const FInputActionValue& Value)
{
	//	input is a Vector2D

	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		


		//	add movement 
		//	AddMovementInput(ForwardDirection, MovementVector.Y);
		//	AddMovementInput(RightDirection, MovementVector.X);
	

		FVector MovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
		
		// Call CustomMove on the movement component
		//	UStarfallMovementComponent* MovementComp = Cast<UStarfallMovementComponent>(GetCharacterMovement());
		AStarfallCharacter::Move(MovementDirection);	
	}
}



void AStarfallHeroCharacter::Sprint(const FInputActionValue& Value)
{
	const FVector test = Value.Get<FVector>();


	Super::Sprint(test);
}


void AStarfallHeroCharacter::StopSprint(const FInputActionValue& Value)
{
	const FVector Input = Value.Get<FVector>();

	Super::StopSprint(Input);
}







void AStarfallHeroCharacter::Look(const FInputActionValue& Value)
{
	//	input is a Vector2D


	//UE_LOG(LogTemp, Log, TEXT("Looking around, nothing in sight"));


	FVector2D LookAxisVector = Value.Get<FVector2D>();
	 
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void AStarfallHeroCharacter::Crouch(const FInputActionValue& Value)
{
	const FVector Input = Value.Get<FVector>();

	Super::Crouch(Input);
}

void AStarfallHeroCharacter::UnCrouch()
{
	Super::UnCrouch();
}


void AStarfallHeroCharacter::StartJump(const FInputActionValue& Value)
{
	const FVector Input = Value.Get<FVector>();

	Super::StartJump(Input, CanJump());
}

void AStarfallHeroCharacter::StopJump()
{
	Super::StopJump();
}