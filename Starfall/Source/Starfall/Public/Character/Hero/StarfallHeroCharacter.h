// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/StarfallCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h" // Include if needed for input handling
#include "StarfallHeroCharacter.generated.h"


//  class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
//  struct FInputActionValue;



//  DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


UCLASS()
class STARFALL_API AStarfallHeroCharacter : public AStarfallCharacter
{
	GENERATED_BODY()
	
    /** Camera boom positioning the camera behind the character */
    //  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    //  USpringArmComponent* CameraBoom;
    //  
    //  /** Follow camera */
    //  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    //  UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCamera;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* CrouchAction;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* SprintAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;


public:
    // Constructor
    AStarfallHeroCharacter(const FObjectInitializer& ObjectInitializer);
    


    // ... Other existing declarations ...




protected:
    /** Called for movement input */
    void Move(const FInputActionValue& Value);
    void Sprint(const FInputActionValue& Value);
    void StopSprint(const FInputActionValue& Value);
    void StartJump(const FInputActionValue& Value);
    void StopJump();


    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    void Crouch(const FInputActionValue& Value);
    void UnCrouch();


protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // To add mapping context
    virtual void BeginPlay();


public:
    /** Returns CameraBoom subobject **/
    //  FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    //  /** Returns FollowCamera subobject **/
    //  FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
    FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
};