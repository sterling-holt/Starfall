#include "Character/Component/StarfallArsenalComponent.h"
#include "Weapon/StarfallWeapon.h"

UStarfallArsenalComponent::UStarfallArsenalComponent()
    : bAreWeaponsAllowed(true)
{
    EquippedWeapon = nullptr;
}



void UStarfallArsenalComponent::SpawnEquippedWeapon()
{
    if (!bAreWeaponsAllowed)
    {
        return; // Exit if weapons are not allowed
    }

    // Implementation of spawning the equipped weapon
}



//  void UStarfallArsenalComponent::EquipWeapon(TSubclassOf<AStarfallWeapon> WeaponClass)
//  {
//      if (!GetWorld() || !WeaponClass)
//      {
//          return;
//      }
//  
//      //  Spawn the weapon actor
//      //  FActorSpawnParameters SpawnParams;
//      //  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//      //  EquippedWeapon = GetWorld()->SpawnActor<AStarfallWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
//  
//      //  Additional logic for attaching the weapon to the character can be added here
//  }

void UStarfallArsenalComponent::FireWeapon()
{
    if (EquippedWeapon)
    {
        //  EquippedWeapon->Fire(); // Assuming AStarfallWeapon has a Fire method
    }
}


void UStarfallArsenalComponent::AimWeapon()
{
    if (EquippedWeapon)
    {
        //  EquippedWeapon->Aim();
    }
}