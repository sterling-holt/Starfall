#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACVitals.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STARFALL_API UACVitals : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACVitals();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    // Health of the character
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float Health;
    
    // Shield value
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float Shield;
    
    // Regeneration rate for health/shield
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
    float RegenerationRate;
    
    // Handles the regeneration logic
    UFUNCTION(BlueprintCallable, Category = "Vitals")
        void Regenerate();

    UFUNCTION(BlueprintCallable, Category = "Vitals")
        void Damage(float DamageAmount);
    
    // Timer handle for regeneration
    FTimerHandle RegenerationTimerHandle;


public:
    //  Called every frame
    //  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //  
    //  // Function to apply damage
    //  UFUNCTION(BlueprintCallable, Category = "Vitals")
    //  void ApplyDamage(float DamageAmount);
    //  
    //  // Function to apply healing
    //  UFUNCTION(BlueprintCallable, Category = "Vitals")
    //  void ApplyHealing(float HealingAmount);

    //  Additional functions for handling status effects can be added here
};
