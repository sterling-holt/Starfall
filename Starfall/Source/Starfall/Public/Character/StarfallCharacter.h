// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Component/ACVitals.h"
#include "Component/StarfallArsenalComponent.h"
#include "Component/StarfallMovementComponent.h"
#include "StarfallCharacter.generated.h"

struct FInputActionValue;


UCLASS()
class STARFALL_API AStarfallCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStarfallCharacter(const FObjectInitializer& ObjectInitializer);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UStarfallMovementComponent* StarfallMovement;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	UStarfallMovementComponent* GetStarfallMovementComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsOnGround();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UACVitals* Vitals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStarfallArsenalComponent* Arsenal;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AStarfallCharacter> Self;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//	Movement Component
	void Move(const FVector& Value);
	void StartJump(const FVector& Value, bool bCanJump);
	void StopJump();
	void Crouch(const FVector& Value);
	void UnCrouch();
	void Sprint(const FVector& Value);
	void StopSprint(const FVector& Value);


	//	Arsenal Component
	void Fire(const FVector& Value);
	void Aim(const FVector& Value);

private:
	FVector MovementInputVector;
};
