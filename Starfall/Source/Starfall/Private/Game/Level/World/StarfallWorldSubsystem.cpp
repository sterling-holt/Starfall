// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Level/World/StarfallWorldSubsystem.h"

void UStarfallWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


}

void UStarfallWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UStarfallWorldSubsystem::UpdateTimeOfDay(float DeltaSeconds)
{
	TimeOfDay.CurrentTime += DeltaSeconds / (TimeOfDay.DayLength / 24.0f);
	if (TimeOfDay.CurrentTime >= 24.0f)
	{
		TimeOfDay.CurrentTime -= 24.0f;
	}

}

FTimeOfDay UStarfallWorldSubsystem::GetCurrentTimeOfDay() const
{
	return TimeOfDay;
}