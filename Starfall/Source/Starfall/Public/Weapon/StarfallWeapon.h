#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarfallWeapon.generated.h"

// Enum for weapon types
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle           UMETA(DisplayName = "Rifle"),
    Shotgun         UMETA(DisplayName = "Shotgun"),
    Sniper          UMETA(DisplayName = "Sniper"),
    HandCannon      UMETA(DisplayName = "HandCannon"),
    Sidearm         UMETA(DisplayName = "Sidearm")
    // Add more weapon types as needed
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Firing      UMETA(DisplayName = "Firing"),
    Aiming      UMETA(DisplayName = "Aiming"),
    Reloading   UMETA(DisplayName = "Reloading")
    // Add other states as needed
};



UCLASS()
class STARFALL_API AStarfallWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AStarfallWeapon();
    void Fire();
    void Aim();

    UFUNCTION(BlueprintCallable)
    void SetWeaponState(EWeaponState NewState);

    UFUNCTION(BlueprintCallable)
    void ResetWeaponState();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Weapon type property
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    EWeaponType WeaponType;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    EWeaponState CurrentWeaponState;

    // Placeholder for future weapon stats and perks
    // UPROPERTY(...) 
    // Future stats and perks will go here
};
