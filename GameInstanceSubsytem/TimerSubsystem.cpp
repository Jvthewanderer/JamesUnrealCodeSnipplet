// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerSubsystem.h"

// AddTimer: Adds time to the timer and updates high score timer if necessary
void UTimerSubsystem::AddTimer(float addedTime)
{
    // Check if the timer is active
    if (!bTimerActive) {
        return;
    }

    // Determine the multiplier based on ascendTimer flag
    float ascendMultiplier = bAscendTimer ? 1.0f : -1.0f;

    // Update the timer and high score timer
    timer += (addedTime * ascendMultiplier);
    if (timer > highscoreTimer) {
        highscoreTimer = timer;
    }

    // Broadcast that the timer has been updated
    OnTimerUpdated.Broadcast(timer);
}

// SetTimer: Set the timer to a new value
void UTimerSubsystem::SetTimer(float newTimer)
{
    timer = newTimer;
    OnTimerUpdated.Broadcast(timer);
}

// StartTimer: Activate or deactivate the timer
void UTimerSubsystem::StartTimer(bool bActivateTimer)
{
    bTimerActive = bActivateTimer;
    OnTimerActivated.Broadcast(bTimerActive);
}

// ResetTimer: Reset the timer to 0
void UTimerSubsystem::ResetTimer()
{
    timer = 0.0f;
    OnTimerUpdated.Broadcast(timer);
}

// AscendTimer: Set the ascendTimer flag To whether to ascend or descend the timer
void UTimerSubsystem::SetAscendTimer(bool bAscend)
{
    bAscendTimer = bAscend;
}