// Fill out your copyright notice in the Description page of Project Settings.


#include "ScheduleManager.h"
#include "../GISubsytem/TimeSubsystem.h"
#include "../Game/SL_GameInstance.h"
#include "../Data/SL_DataAsset.h"
#include "../Events/GameEvent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UScheduleManager::UScheduleManager()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UScheduleManager::BeginPlay()
{
	Super::BeginPlay();

	// ...

	TimeSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UTimeSubsystem>();
	if (TimeSubsystem)
	{
		TimeSubsystem->OnTimeUpdated.AddDynamic(this, &UScheduleManager::OnTimeUpdated);
	}


}

void UScheduleManager::OnTimeUpdated(float DeltaTime)
{
	//Add new events on Correlated time.
	if (SchedulePool.Contains((int32)(TimeSubsystem->GetTime()))) {
		FScheduleData CurrentData = *SchedulePool.Find((int32)(TimeSubsystem->GetTime()));

		for (UGameEvent_DA* Event : CurrentData.EventList) {

			//Check if event is already active
			if (ActiveEvents.Contains(Event->EventTag)) {
				continue;
			}

			UGameEvent* CreatedEvent = CreateEvent(Event->EventClass);

			if (CreatedEvent) {
				OnNewEventCreated.Broadcast(CreatedEvent);
			}

		}

	}

	//Remove old events when Event is Over
	TArray<FGameplayTag> Keys;
	ActiveEvents.GenerateKeyArray(Keys);

	if (Keys.Num() < 1) {
		return;
	}

	for (FGameplayTag EventTag : Keys)
	{
		if (ActiveEvents.Contains(EventTag))
		{
			UGameEvent* Event = *ActiveEvents.Find(EventTag);

			if (TimeSubsystem->GetTime() >= Event->GetEndEventTime())
			{
				OnEventRemoved.Broadcast(Event);
				Event->EndGameEvent();
				ActiveEvents.Remove(EventTag);

			}
		}
	}
}

void UScheduleManager::RefreshActiveEvents()
{
	TArray<FGameplayTag> KeyList;
	ActiveEvents.GenerateKeyArray(KeyList);

	for (FGameplayTag ActiveTag : KeyList) {

		UGameEvent* CreatedEvent = *ActiveEvents.Find(ActiveTag);

		OnNewEventCreated.Broadcast(CreatedEvent);
	}
}

UGameEvent* UScheduleManager::CreateEvent(TSubclassOf<UGameEvent> Event)
{
	//Create Event
	if (!Event) {
		return nullptr;
	}
	UGameEvent* NewEvent = NewObject<UGameEvent>(this, Event);

	//Initialize Activities on Event
	NewEvent->InitGameEvent(GetWorld());

	ActiveEvents.Add(NewEvent->GetEventTag(), NewEvent);

	return NewEvent;
}

void UScheduleManager::RemoveEvent(FGameplayTag EventTag)
{
	if (ActiveEvents.Contains(EventTag))
	{
		UGameEvent* Event = *ActiveEvents.Find(EventTag);

		Event->EndGameEvent();
		ActiveEvents.Remove(EventTag);

	}
}

void UScheduleManager::ClearEvents()
{
	TArray<UGameEvent*> Events;
	ActiveEvents.GenerateValueArray(Events);

	for (UGameEvent* Event : Events)
	{
		OnEventRemoved.Broadcast(Event);
		Event->EndGameEvent();
	}

	ActiveEvents.Empty();
}

UGameEvent* UScheduleManager::GetActiveEvent(FGameplayTag EventTag)
{
	if (!ActiveEvents.Contains(EventTag)) {
		return nullptr;
	}

	return *ActiveEvents.Find(EventTag);
}

void UScheduleManager::AddNewEventInPool(int32 ScheduleTime, UGameEvent_DA* NewEvent)
{
	if (SchedulePool.Contains(ScheduleTime))
	{
		SchedulePool[ScheduleTime].EventList.Add(NewEvent);
	}
	else {
		FScheduleData NewScheduleData;
		NewScheduleData.EventList.Add(NewEvent);

		SchedulePool.Add(ScheduleTime, FScheduleData{ NewScheduleData });
	}
}

void UScheduleManager::RemoveEventFromPool(int32 ScheduleTime, UGameEvent_DA* Event)
{
	if (!SchedulePool.Contains(ScheduleTime)) {
		if (SchedulePool[ScheduleTime].EventList.Num() < 1)
		{
			SchedulePool.Remove(ScheduleTime);
		}
		else {
			SchedulePool[ScheduleTime].EventList.Remove(Event);
		}

	}
}

void UScheduleManager::InitSchedulePool()
{
	USL_GameInstance* GI = Cast<USL_GameInstance>(UGameplayStatics::GetGameInstance(this));

	if (GI)
	{
		//Get Schedule Data
		UGameSchedule_DA* Scheduledata = GI->GetScheduleData(DefaultDayTag);

		if (Scheduledata) {
			SchedulePool = Scheduledata->SchduleMap;

			//Add Additional Events in Pool

		}

		//check if Any Schedule before the time of day is active

		TArray<int32> OutTimeList;

		SchedulePool.GenerateKeyArray(OutTimeList);
		for (int32 Time : OutTimeList)
		{
			if (Time <= (int32)TimeSubsystem->GetTime())
			{
				FScheduleData CurrentData = *SchedulePool.Find(Time);

				for (UGameEvent_DA* Event : CurrentData.EventList) {
					UGameEvent* CreatedEvent = CreateEvent(Event->EventClass);

					OnNewEventCreated.Broadcast(CreatedEvent);
				}
			}
		}

		//Remove old events
		TArray<FGameplayTag> Keys;
		ActiveEvents.GenerateKeyArray(Keys);

		if (Keys.Num() < 1) {
			return;
		}

		for (FGameplayTag EventTag : Keys)
		{
			if (ActiveEvents.Contains(EventTag))
			{
				UGameEvent* Event = *ActiveEvents.Find(EventTag);

				if (TimeSubsystem->GetTime() >= Event->GetEndEventTime())
				{
					OnEventRemoved.Broadcast(Event);
					Event->EndGameEvent();
					ActiveEvents.Remove(EventTag);

				}
			}
		}

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No Game Instance Found"));
	}

}

