#include "Weapon/StarfallWeapon.h"

// Sets default values
AStarfallWeapon::AStarfallWeapon()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize the WeaponType (defaulting to Rifle as an example)
    WeaponType = EWeaponType::Rifle;
    CurrentWeaponState = EWeaponState::Idle;

}


// Called when the game starts or when spawned
void AStarfallWeapon::BeginPlay()
{
    Super::BeginPlay();

    // Initialization or setup code specific to when the weapon is created/spawned goes here
}

// Called every frame
void AStarfallWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Code for actions to perform each frame can be added here
}



void AStarfallWeapon::SetWeaponState(EWeaponState NewState)
{
    CurrentWeaponState = NewState;

    // Handle state change logic here (e.g., triggering animations)
}

void AStarfallWeapon::ResetWeaponState()
{
    SetWeaponState(EWeaponState::Idle);
    // Additional logic for resetting the state
}


void AStarfallWeapon::Fire()
{
    // Implement firing logic here: 
    // - Create and launch projectile
    // - Reduce ammo
    // - Handle firing effects (sound, animation, etc.)
    SetWeaponState(EWeaponState::Firing);
    UE_LOG(LogTemp, Display, TEXT("Firing weapon"));


    //  Scan for target
    //  Spawn projectile
    //  Shoot projectile forward at target? Or should I even bother with this if we're doing hitscan?
    //  Set weapon state back to idle, waiting for more input.

}


void AStarfallWeapon::Aim()
{
    UE_LOG(LogTemp, Display, TEXT("Aiming Weapon"));
}