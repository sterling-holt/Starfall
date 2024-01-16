// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Character/StarfallCharacter.h"
//	#include "CharacterPoolItem.h"
#include "StarfallGameModeBase.generated.h"

/**
 * 
 */

USTRUCT()
struct FPoolItem
{
    GENERATED_BODY()


    // The character instance
    AStarfallCharacter* Character;

    // Is this character available for use?
    bool bIsAvailable;
};



UCLASS()
class STARFALL_API AStarfallGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    AStarfallGameModeBase();

    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void InitPool();
    
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    AStarfallCharacter* GetFromPool();

    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void ReturnToPool(AStarfallCharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void ListPoolItems();
   
protected:
    UPROPERTY(EditAnywhere, Category = "Pooling")
    TArray<FPoolItem> Pool;

    //  The character class to pool
    UPROPERTY(EditDefaultsOnly, Category = "Pooling")
    TSubclassOf<ACharacter> CharacterClassToPool;

    // Total number of characters to pool
    UPROPERTY(EditDefaultsOnly, Category = "Pooling")
    int32 PoolSize;
};
