// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "StarfallWorldSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FTimeOfDay
{
    GENERATED_BODY()

    // Current time of day in hours
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float CurrentTime;

    // Duration of a full day in game-time hours
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float DayLength;

    // Constructor
    FTimeOfDay() : CurrentTime(0.0f), DayLength(24.0f) {}
};


UCLASS()
class STARFALL_API UStarfallWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
    FTimeOfDay TimeOfDay;
    
    
    // Function to get the current time of day
    UFUNCTION(BlueprintCallable, Category = "Time")
    FTimeOfDay GetCurrentTimeOfDay() const;


public:
    // Override initialization method
    //  UFUNCTION(BlueprintCallable, Category = "Time Of Day")
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Override deinitialization method
    UFUNCTION(BlueprintCallable, Category = "Time Of Day")
    virtual void Deinitialize() override;

    // Update the time of day; typically called every frame
    UFUNCTION(BlueprintCallable, Category = "Time Of Day")
    void UpdateTimeOfDay(float DeltaSeconds);


protected:
};
