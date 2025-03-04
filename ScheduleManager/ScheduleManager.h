#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "../Data/SL_DataAsset.h"
#include "../GISubsytem/TimeSubsystem.h"
#include "ScheduleManager.generated.h"

class UGameEvent;
class UActivity;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventChanged, UGameEvent*, Event);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRJSCHOOLLIFE_API UScheduleManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UScheduleManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	UGameEvent* CreateEvent(TSubclassOf<UGameEvent> Event);

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void RemoveEvent(FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void ClearEvents();

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	UGameEvent* GetActiveEvent(FGameplayTag EventTag) ;

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void AddNewEventInPool(int32 ScheduleTime, UGameEvent_DA* NewEvent);

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void RemoveEventFromPool(int32 ScheduleTime, UGameEvent_DA* Event);

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void InitSchedulePool();

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void OnTimeUpdated(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void RefreshActiveEvents();

public:

	UPROPERTY(BlueprintAssignable)
	FOnEventChanged OnNewEventCreated;

	UPROPERTY(BlueprintAssignable)
	FOnEventChanged OnEventRemoved;


protected:

	float NextScheduleTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Schedule")
	FGameplayTag DefaultDayTag;

	UPROPERTY()
	TMap<FGameplayTag, UGameEvent*> ActiveEvents;

	UPROPERTY()
	TMap<int32, FScheduleData> SchedulePool;

	UPROPERTY()
	TObjectPtr<UTimeSubsystem> TimeSubsystem;
		
};
