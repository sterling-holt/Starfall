// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ACVitals.h"

UACVitals::UACVitals()
{
    //  Set this component to be initialized when the game starts, and to be ticked every frame.
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize default values
    Health = 20.0f;
    Shield = 100.0f;
    RegenerationRate = 1.0f; // Example value, adjust as needed
}


void UACVitals::BeginPlay()
{
    Super::BeginPlay();

    // Setup regeneration timer
    GetWorld()->GetTimerManager().SetTimer(RegenerationTimerHandle, this, &UACVitals::Regenerate, 1.0f, true);
}

void UACVitals::Regenerate()
{
    // Regenerate health and shield here
    // Ensure not to exceed maximum values

    // Example: Increment Health and Shield
    //  Health = FMath::Min(Health + RegenerationRate, MaxHealth);
    //  Shield = FMath::Min(Shield + RegenerationRate, MaxShield);
    UE_LOG(LogTemp, Display, TEXT("$(Health)"));
}

void UACVitals::Damage(float DamageAmount)
{
    UE_LOG(LogTemp, Display, TEXT("Hello there"));
    // Apply damage to shield first, then health
    // Example logic, adjust as needed
}

/* 
void UACVitals::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Handle continuous effects like poison or healing over time, if applicable
}

*/