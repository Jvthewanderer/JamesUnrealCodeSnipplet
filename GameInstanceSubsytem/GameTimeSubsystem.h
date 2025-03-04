// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameTimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimeUpdated, float, timeInSecond);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelayTimeUpdated, float, timeInSecond);

/**
 * 
 */
UCLASS()
class UTILITYSUBSYSTEMS_API UGameTimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Time Subsystem")
	void SetTime(float TimeInSecond);

	UFUNCTION(BlueprintCallable, Category = "Time Subsystem")
	void SetDelayPerInGameMinute(float Delay) {delayPerInGameMinute = Delay;}

	UFUNCTION(BlueprintCallable, Category = "Time Subsystem")
	void StartTime() { 
		bTimeStart = true;
		OnTimeUpdated.Broadcast(GetHourPerMin());
		}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time Subsystem")
	float GetTime(){return currentTime;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time Subsystem")
	int32 GetHour();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time Subsystem")
	int32 GetHourInMilitaryTime();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time Subsystem")
	int32 GetMinute();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time Subsystem")
	bool AMOrPM() {return currentTime >= 1200;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time Subsystem")
	float GetDelayTime(){return delayPerInGameMinute;}

	UFUNCTION(BlueprintCallable, Category = "Time Subsystem")
	void PauseTime(){bTimeStart = false;}

	UFUNCTION(BlueprintCallable, Category = "Time Subsystem")
	void AdvanceTime(float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time Subsystem")
	float GetHourPerMin() { return hourPerMin; }

	UPROPERTY(BlueprintAssignable, Category = "Time Subsystem")
	FTimeUpdated OnTimeUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Time Subsystem")
	FDelayTimeUpdated OnDelayTimeUpdated;

protected:

	UPROPERTY()
	float currentTime = 1200.f;

	UPROPERTY()
	float delayPerInGameMinute = 0.333f;

	float delayDelta = 0.f;

	float hourPerMin = 0.f;

	UPROPERTY()
	float maxTime = 2400.f;

	UPROPERTY()
	bool bTimeStart = false;


	
};
