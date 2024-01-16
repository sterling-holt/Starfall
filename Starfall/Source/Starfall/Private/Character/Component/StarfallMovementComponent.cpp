
#include "Character/Component/StarfallMovementComponent.h"
#include "Character/StarfallCharacter.h"
#include "Components/CapsuleComponent.h"





//  Destiny movement:
//  -   Traits like Mobility affect base walk speed
//  -   Player mass affects player gravity
//  -   Twilight Garrison
//  -
//  -
//  -
//  -
//  -




//  Halo movement:
//  -
//  -
//  -
//  -
//  -
//  -
//  -
//  -
//  -


//  Quake movement:
//  -
//  -
//  -
//  -
//  -
//  -
//  -
//  -
//  -
//  -














UStarfallMovementComponent::UStarfallMovementComponent()
{
    //  FVector Acceleration;
    AirControl = 0.0f;
    //  AirControlBoostMultiplier;
    //  AirControlBoostVelocityThreshold;

	JumpZVelocity = 500.f;
    GravityScale = 1.2f;
	MaxWalkSpeed = 400.f;
	MinAnalogWalkSpeed = 20.f;
    MaxAcceleration = 857.25f;
    
    SprintSpeedMultiplier = 2.0f; // Adjust as needed

    CrouchedCapsuleHalfHeight = 40.f;

    bImpartBaseVelocityX = true;
    bImpartBaseVelocityY = true;
    bImpartBaseVelocityZ = true;
    
    bCanSprint = false;
    bIsSprinting = false;
    SprintSpeed = 800.0f;
    CurrentState = EStarfallMovementState::Idle;


	GetNavAgentPropertiesRef().bCanCrouch = true;
    GetNavAgentPropertiesRef().bCanJump = true;
    GetNavAgentPropertiesRef().bCanFly = false;

    // Set default values for other parameters similarly.
}

void UStarfallMovementComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UStarfallMovementComponent::SetMovementState(EStarfallMovementState NewState)
{
    //  -------------------------------------------------------------------
    PreviousState = CurrentState;           //  Update Previous State
    CurrentState = NewState;                //  Update Current State
    //  -------------------------------------------------------------------

    switch (CurrentState)
    {
        case EStarfallMovementState::Idle:
            //  Logic for Walking state
            UE_LOG(LogTemp, Log, TEXT("Idle"));
            break;

        case EStarfallMovementState::Walking:
            MaxWalkSpeed = 400.f;
            bIsSprinting = false;

            UE_LOG(LogTemp, Log, TEXT("Walking"));
            break;

        case EStarfallMovementState::Sprinting:
            MaxWalkSpeed = 900.f;
            break;

        case EStarfallMovementState::Jumping:
            
            UE_LOG(LogTemp, Log, TEXT("Jumping"));
            break;

        case EStarfallMovementState::CrouchIdle:
            
            
            UE_LOG(LogTemp, Log, TEXT("Crouch Idle"));
            break;
        
        case EStarfallMovementState::CrouchWalk:

            UE_LOG(LogTemp, Log, TEXT("Crouch Walking"));
            break;

        case EStarfallMovementState::CrouchSlide:

        default:
            // Default case if needed
            break;
    }
}

void UStarfallMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // If the velocity is approximately zero, set the state to Idle
    if (IsMovingOnGround())
    {
        //   GetCharacterOwner()->GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchedCapsuleHalfHeight)
        //  if capsule component is crouch height
        //      if velocity.nearzero
        //          state: crouch-idle
        //      else if velocity is more than velocity.nearzero and less than maxwalkspeed
        //          crouch-walk
        //      else if velocity is more than maxwalkspeed
        //          crouch-slide
        //          
        //  else:
        //      state: crouch-walking


        if (CurrentState != EStarfallMovementState::Walking)
        {
            SetMovementState(EStarfallMovementState::Walking);
        }
        
        //  // Jump acceleration logic
        //  if (bIsJumpAccelerating)
        //  {
        //      float JumpAccelerationDuration = 0.5f; // Duration of the acceleration phase in seconds
        //      const float JumpAccelerationRate = 2000.0f; // Rate of acceleration
        //  
        //      float ElapsedTime = GetWorld()->GetTimeSeconds() - JumpStartTime;
        //      if (ElapsedTime < JumpAccelerationDuration)
        //      {
        //          // Calculate acceleration factor (0 to 1)
        //          float AccelerationFactor = ElapsedTime / JumpAccelerationDuration;
        //  
        //          // Apply acceleration to the vertical component of the jump velocity
        //          Velocity.Z += JumpAccelerationRate * AccelerationFactor * DeltaTime;
        //      }
        //      else
        //      {
        //          // End of acceleration phase
        //          Velocity.Z = JumpPeakVelocity; // Apply constant peak velocity
        //          bIsJumpAccelerating = false; // Stop accelerating
        //      }
        //  }
    }

    // You can add more logic here if needed for other states

    // Rest of the tick logic...
}



void UStarfallMovementComponent::Move(const FVector& MoveDirection)
{
    AActor* Owner = GetOwner();
    FTransform OwnerTransform = Owner->GetActorTransform();

    FVector NormalizedMoveDirection = MoveDirection.GetSafeNormal();
    
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // Store the previous vertical velocity
    float PreviousVerticalVelocity = Velocity.Z;

    // Determine the desired speed
    float DesiredSpeed = 400.f;

    // Update only the horizontal velocity
    Velocity = NormalizedMoveDirection * DesiredSpeed;

    // Reapply the vertical velocity
    Velocity.Z = PreviousVerticalVelocity;


    UE_LOG(LogTemp, Display, TEXT("Velocity: %s"), *Velocity.ToString());
    // Rest of your logic...
}




void UStarfallMovementComponent::Sprint(const FVector& MoveDirection)
{
    if (MoveDirection.IsNearlyZero())
    {
        StopSprint();
        return;
    }

    // Increase the movement speed for sprinting
    MaxWalkSpeed *= SprintSpeedMultiplier;

    // Update the movement direction
    Velocity = MoveDirection.GetSafeNormal() * MaxWalkSpeed;

    SetMovementState(EStarfallMovementState::Sprinting);
}
void UStarfallMovementComponent::StopSprint()
{
    // Reset the movement speed
    MaxWalkSpeed /= SprintSpeedMultiplier;

    // Revert to the previous movement state
    SetMovementState(PreviousState);
}


bool UStarfallMovementComponent::CanAttemptJump() const
{
    // Add your conditions here. For instance:
    // return IsMovingOnGround() && !bWantsToCrouch;
    return IsMovingOnGround() ? Super::CanAttemptJump() : false; // Or your custom logic
}

void UStarfallMovementComponent::Jump(bool Input, bool bCanJump)
{
    if (this->CanAttemptJump())
    {
        DoJump(Input);
    }
    else {
        DoJump(false);
    }
}


void UStarfallMovementComponent::UpdateAirControl(float DeltaTime)
{
    if (IsFalling())
    {
        // Implement enhanced air control
        // Modify Velocity based on AirControlFactor and player input
        // Rest of the air control logic...
    }
}

bool UStarfallMovementComponent::DoJump(bool bReplayingMoves)
{
    if (CanAttemptJump())
    {
        //  Update Air control logic
        //  Use momentum * velocity logic
        //  Update speed, use physics simulations within default kismet

        return Super::DoJump(bReplayingMoves);
    }
    else {
        return false;
    }
}


void UStarfallMovementComponent::Crouch(const FVector& IsCrouching)
{
    bIsCrouching = true;
    float SquaredSprintSpeed = SprintSpeed * SprintSpeed;

    // Calculate the squared magnitude of velocity
    float SquaredVelocityMagnitude = this->Velocity.SizeSquared();

    // Check if the squared magnitude of velocity is less than or equal to the squared threshold
    //  GetCharacterOwner()->GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchedCapsuleHalfHeight);

    if (this->Velocity.IsNearlyZero())
    {
        SetMovementState(EStarfallMovementState::CrouchIdle);
    }
    else if (this->Velocity.Size() >= MaxWalkSpeed)
    {
        SetMovementState(EStarfallMovementState::CrouchWalk);
    }
    else
    {
        //  Acceleration ramp slows
        SetMovementState(EStarfallMovementState::CrouchSlide);
        // After velocity slows down to near zero, switch back to crouch idle.
    }
}



void UStarfallMovementComponent::UnCrouch()
{
        GetCharacterOwner()->GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHeight);
}