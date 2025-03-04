// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, float, UpdatedTimer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerActivated, bool, bTimerActivated);

/**
 * 
 */
UCLASS()
class UTILITYASSET_API UTimerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Utility Timer")
	void AddTimer(float addedTime);

	UFUNCTION(BlueprintCallable, Category = "Utility Timer")
	void SetTimer(float newTimer);

	UFUNCTION(BlueprintCallable, Category = "Utility Timer")
	void StartTimer(bool timerActivate);

	UFUNCTION(BlueprintCallable, Category = "Utility Timer")
	void ResetTimer();

	UFUNCTION(BlueprintCallable, Category = "Utility Timer")
	void SetAscendTimer(bool ascend);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility Timer")
	float GetTimer() {return timer;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility Timer")
	float GetHighScoreTimer(){return highscoreTimer;}

protected:
	float timer = 0.0f;

	float highscoreTimer = 0.0f;
	
	bool bTimerActive = false;

	bool bAscendTimer = false;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Utility Timer")
	FOnTimerUpdated OnTimerUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Utility Timer")
	FOnTimerActivated OnTimerActivated;

};
