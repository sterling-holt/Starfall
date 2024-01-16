// =======================================================================================
// StarfallMovementComponent.h
// Description: Defines the movement component for characters in Starfall game.
//              Manages different movement states like walking, sprinting, and jumping.
// =======================================================================================

#pragma once

// ================================== Includes ============================================
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StarfallMovementComponent.generated.h"

// ============================ Movement States Enumeration ===============================
UENUM(BlueprintType)
enum class EStarfallMovementState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Walking     UMETA(DisplayName = "Walking"),
    Sprinting   UMETA(DisplayName = "Sprinting"),
    Jumping     UMETA(DisplayName = "Jumping"),
    CrouchIdle  UMETA(DisplayName = "CrouchIdle"),
    CrouchWalk  UMETA(DisplayName = "CrouchWalk"),
    CrouchSlide UMETA(DisplayName = "CrouchSlide")
    // Add other states as needed
};

// ========================== Starfall Movement Component Class ============================
UCLASS()
class STARFALL_API UStarfallMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    // --------------------------------- Constructor ---------------------------------------
    UStarfallMovementComponent();

    // ------------------------------ Public Functions -------------------------------------
    void Move(const FVector& MoveDirection);
    void Sprint(const FVector& MoveDirection);
    void StopSprint();
    void Crouch(const FVector& IsCrouching);
    void UnCrouch();

    // New declarations for jump boost
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AirControlFactor;
    void UpdateAirControl(float DeltaTime);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
    float MinJumpBoostMultiplier = 1.0f; // Minimum multiplier for jump boost

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
    float MaxJumpBoostMultiplier = 1.5f; // Maximum multiplier for jump boost

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintSpeedMultiplier;

    void Jump(bool bReplayingMoves, bool bCanJump);
    bool CanAttemptJump() const;
    virtual bool DoJump(bool bReplayingMoves) override;
    void OnJumpUpdate(float JumpStartTime, float JumpAccelerationDuration, float JumpAccelerationRate, FVector& JumpVelocity);

    void SetMovementState(EStarfallMovementState NewState);

    // ------------------------------ Public Properties ------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    EStarfallMovementState CurrentState;
    EStarfallMovementState PreviousState;
    EStarfallMovementState StarfallMovementState;

    FVector CurrentVelocity;
    float CrouchedCapsuleHalfHeight;
    float CapsuleHeight;
    float SprintSpeed;
    bool bCanSprint;
    bool bIsSprinting;
    bool bIsCrouching;

protected:
    // ------------------------------ Protected Functions ----------------------------------
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
    // ------------------------------- Private Members -------------------------------------
    float JumpStartTime;
    bool IsJumpAccelerating;
    const float JumpStartVelocity = 500.f;
    const float JumpPeakVelocity = 1000.f;
    const float JumpAccelerationDuration = 0.5f;
    const float JumpAccelerationRate = 2000.0f;
};