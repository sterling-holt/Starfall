// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Mode/StarfallGameModeBase.h"



AStarfallGameModeBase::AStarfallGameModeBase()
{
    // hello
}

void AStarfallGameModeBase::InitPool()
{
    for (int32 i = 0; i < PoolSize; ++i)
    {
        AStarfallCharacter* Character = GetWorld()->SpawnActor<AStarfallCharacter>(CharacterClassToPool, FTransform());
        Character->SetActorEnableCollision(false);
        Character->SetActorHiddenInGame(true);

        FPoolItem Item;
        Item.Character = Character;
        Item.bIsAvailable = true;

        Pool.Add(Item);
    }
}

AStarfallCharacter* AStarfallGameModeBase::GetFromPool()
{
    for (FPoolItem& Item : Pool)
    {
        if (Item.bIsAvailable)
        {
            Item.bIsAvailable = false;
            return Item.Character;
        }
    }

    // Optionally, expand the pool here if needed
    return nullptr;
}

void AStarfallGameModeBase::ReturnToPool(AStarfallCharacter* Character)
{
    for (FPoolItem& Item : Pool)
    {
        if (Item.Character == Character)
        {
            Item.bIsAvailable = true;
            Character->SetActorEnableCollision(false);
            Character->SetActorHiddenInGame(true);
            // Reset character state as needed
            break;
        }
    }
}


void AStarfallGameModeBase::ListPoolItems()
{
    for (const FPoolItem& Item : Pool)
    {
        if (Item.Character != nullptr)
        {
            // Example: Log the character's name and availability
            FString CharacterName = Item.Character->GetName();
            FString Availability = Item.bIsAvailable ? TEXT("Available") : TEXT("In Use");
            UE_LOG(LogTemp, Warning, TEXT("%s is %s"), *CharacterName, *Availability);
        }
    }
}