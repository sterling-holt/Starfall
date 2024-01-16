#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/StarfallWeapon.h"
#include "StarfallArsenalComponent.generated.h"

// Enum for weapon slots
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
    Primary   UMETA(DisplayName = "Primary"),
    Special   UMETA(DisplayName = "Special"), // Renamed from Secondary
    Heavy     UMETA(DisplayName = "Heavy")
};

// Struct to hold weapon slot information
USTRUCT(BlueprintType)
struct FWeaponSlot
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    TSubclassOf<AStarfallWeapon> EquippedWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    TArray<TSubclassOf<AStarfallWeapon>> ReserveWeapons;

    FWeaponSlot() : EquippedWeapon(nullptr)
    {
        ReserveWeapons.SetNum(9); // Initialize with 9 reserve slots
    }
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STARFALL_API UStarfallArsenalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStarfallArsenalComponent();

    // Method to equip a weapon
    void EquipWeapon(TSubclassOf<AStarfallWeapon> WeaponClass);

    void SpawnEquippedWeapon();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    bool bAreWeaponsAllowed(bool bAllowed);

    // Method to fire the currently equipped weapon
    void FireWeapon();
    void AimWeapon();

protected:
    // Currently equipped weapon
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
    TSubclassOf<AStarfallWeapon> EquippedWeapon;
};