// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ScoringSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, int32, updatedScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMuliplierUpdated, int32, updatedMultipler);

/**
 * 
 */
UCLASS()
class UTILITYASSET_API UScoringSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game Jam Scoring")
	void SetScore(int32 newScore) {
		score += newScore * muliplier;
		if (score > highScore) {
			highScore = score;
		}
		onScoreUpdated.Broadcast(score);
	}

	UFUNCTION(BlueprintCallable, Category = "Game Jam Scoring")
	void SetHighScore(int32 newHighScore) {
		highScore = newHighScore;
	}

	UFUNCTION(BlueprintCallable, Category = "Game Jam Scoring")
	void SetMultiplier(int32 newMulitplier) { 
		muliplier = newMulitplier;
		if (muliplier < 1)
		{
			muliplier = 1;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Game Jam Scoring")
	void ResetScoreSettings() {
		score = 0;
		muliplier = 1;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Jam Scoring")
	int32 GetScore() {return score;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Jam Scoring")
	int32 GetHighScore() {return highScore;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Jam Scoring")
	int32 GetMultiplier(){return muliplier;}

protected:
	
	int32 score = 0;

	int32 highScore = 0;
	
	int32 muliplier = 1;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Game Jam Scoring")
	FOnScoreUpdated onScoreUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Game Jam Scoring")
	FOnMuliplierUpdated onMultiplierUpdated;
};
