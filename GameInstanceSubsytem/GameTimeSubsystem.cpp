// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

void UTimeSubsystem::SetTime(float TimeInSecond)
{
	currentTime = TimeInSecond > maxTime ? 0.f : TimeInSecond;
	int32 inCurrentTime = UKismetMathLibrary::FFloor(currentTime / 100.f);
	int32 minute = static_cast<int32>((currentTime - (inCurrentTime * 100)));

	hourPerMin = minute + inCurrentTime * 60.f;

	OnTimeUpdated.Broadcast(currentTime);
}

int32 UTimeSubsystem::GetHour()
{
	int32 returnHour = UKismetMathLibrary::FFloor(currentTime / 100.f);

	if (returnHour >= 24 || returnHour <= 0) {
		returnHour = 12;
	}

	if (returnHour > 12) {
		returnHour = returnHour - 12;
	}

	return returnHour;
}

int32 UTimeSubsystem::GetHourInMilitaryTime()
{
	return UKismetMathLibrary::FFloor(currentTime / 100.f);
}

int32 UTimeSubsystem::GetMinute()
{
	int32 inCurrentTime = UKismetMathLibrary::FFloor(currentTime/100.f);
	int32 minute = static_cast<int32>((currentTime - (inCurrentTime * 100)));
	return minute;
}

void UTimeSubsystem::AdvanceTime(float DeltaTime)
{
	if (bTimeStart)
	{
		delayDelta = delayDelta + DeltaTime;
		OnDelayTimeUpdated.Broadcast(delayDelta);

		if (delayDelta >= delayPerInGameMinute) {
			delayDelta = 0.f;
			currentTime = FMath::Clamp(currentTime + 1.0f, 0.f, maxTime); // Using FMath::Clamp to ensure CurrentTime stays within bounds
			hourPerMin = hourPerMin + 1.f;

			int32 inCurrentTime = UKismetMathLibrary::FFloor(currentTime / 100.f);
			int32 minute = static_cast<int32>((currentTime - (inCurrentTime * 100)));

			if (minute > 59)
			{
				inCurrentTime++;
				currentTime = (inCurrentTime * 100);

				if (currentTime >= maxTime)
				{
					hourPerMin = 0.f;
					currentTime = 0.f;
				}
			}

			if (OnTimeUpdated.IsBound()) {
				OnTimeUpdated.Broadcast(hourPerMin);
			}

		}

	}
}

