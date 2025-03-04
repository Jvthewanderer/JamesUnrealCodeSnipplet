// Fill out your copyright notice in the Description page of Project Settings.


#include "Action.h"
#include "GameFramework/GameStateBase.h"
#include "Components/ActorComponent.h"
#include "../Component/ActionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Ability/Ability_AttackModifier.h"
#include "../Component/AbilityComponent.h"
#include "Components/TimelineComponent.h"
#include "../Game/Holo_Character.h"

UAction::UAction()
{
	BaseDamage = -10.0f;
	RecastTime = 0.0f;

	ActionCommandNames.Add("Attack1");
}

void UAction::Initialize(UActionComponent* NewActionComp, TSubclassOf<UAbility_AttackModifier> newAttackModifier, uint8 Stack)
{
	ActionComponent = NewActionComp;
	AbilityComponent = UAbilityComponent::GetAbilityComponent(ActionComponent->GetOwner());
	AttackModifierClass = newAttackModifier;

	if (AttackModifierClass) {
		AttackModifier = NewObject<UAbility_AttackModifier>(ActionComponent, AttackModifierClass);

		AttackModifier->SetController(UGameplayStatics::GetPlayerController(ActionComponent->GetOwner(), 0));
		AttackModifier->SetAbilityComponent(AbilityComponent);
		AttackModifier->SetInstigator(ActionComponent->GetOwner());
		AttackModifier->SetStack((int32)Stack);

		if (AttackModifier->CanModifyAction()) {
			AttackModifier->ModifyAction(FActionModifierInfo());
			ModifyAction(AttackModifier->GetModifyAction());
		}

		AttackModifier->StartAbility();
	}

	FOnTimelineFloat progressFunction;
	progressFunction.BindUFunction(this, FName("ActionProgress")); // The function EffectProgress gets called
	Timeline.AddInterpFloat(CurveTimeline, progressFunction);

	DamagedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("StatusEffect.KnockedBack")));
	DamagedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("StatusEffect.Disabled")));

}

float UAction::GetTimeRemaining() const
{
	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();

	if (GS && TimeStarted > 0.0f)
	{
		float EndTime = TimeStarted + RecastTime;
		return EndTime - GS->GetServerWorldTimeSeconds();
	}

	return -1.0f;
}

bool UAction::IsRunning() const
{
	return bIsRunning;
}

bool UAction::IsTargeting() const
{
	return bTargeting;
}

bool UAction::CanStart(AActor* Instigator)
{
	if (IsRunning() || GetTimeRemaining() > 0.0f)
	{
		return false;
	}

	return true;
}

void UAction::StartAction_Implementation(AActor* Instigator)
{
	UActionComponent* Comp = GetOwningComponent();
	AHolo_Character* HoloCharacter = Cast<AHolo_Character>(Instigator);

	Comp->StatusAilmentsGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;

	StartCooldown();

	if (bCanAimWhileInAction) {
		if (HoloCharacter) {
			GetWorld()->GetTimerManager().SetTimer(AimingHandle, [HoloCharacter, this] {
				HoloCharacter->AimDirectionLookAtTargetInterp(UGameplayStatics::GetPlayerPawn(GetWorld(), 0), GetWorld()->GetDeltaSeconds(), InterpSpeed);
			},0.02,true);
		}
	}

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);

}

void UAction::StopAction_Implementation(AActor* Instigator)
{
	UActionComponent* Comp = GetOwningComponent();

	Comp->StatusAilmentsGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;

	if (AimingHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(AimingHandle);
	}

	if (AttackModifier) {
		AttackModifier->EndAbility();
	}

	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
}

void UAction::TriggerAction_Implementation(AActor* Instigator)
{
	bTargeting = false;

	GetOwningComponent()->OnActionTriggered.Broadcast(GetOwningComponent(), this);
}

void UAction::TriggerAttackModifier_Implementation()
{
	AttackModifier->OnTrigger();
}

TSubclassOf<UAbility> UAction::GetAttackModifierEffect()
{
	return AttackModifier->AbilityEffectClass;
}

void UAction::TriggerActionFromAnimNotify_Implementation(AActor* Instigator, FName SocketName)
{

}

void UAction::StartCooldown()
{
	TimeStarted = GetWorld()->TimeSeconds;

}

void UAction::ClearCooldown()
{
	TimeStarted = 0.0f;

}

void UAction::ReduceCooldown(float reduceTime)
{
	TimeStarted += reduceTime;
}

UActionComponent* UAction::GetOwningComponent() const
{
	return ActionComponent;
}

UAbilityComponent* UAction::GetAbilityComponent() const
{
	return AbilityComponent;
}

float UAction::GetBaseDamage()
{
	return BaseDamage;
}

void UAction::ModifyAction_Implementation(FActionModifierInfo newAbilityModifier)
{
	BaseDamage += newAbilityModifier.ModifyDamage;
	RecastTime += newAbilityModifier.ModifyRecast;

	ActionModifierInfo = newAbilityModifier;
}

UWorld* UAction::GetWorld() const
{
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}

FAbilityModifierInfo UAction::GetAbilityModifierFromAttackModifier()
{
	if (AttackModifier)
	{
		return AttackModifier->GetAbilityModifier();
	}
	return FAbilityModifierInfo();
}

void UAction::StartTimeline_Implementation()
{
	Timeline.PlayFromStart();
	Timeline.SetPlayRate(PlayRate/ Duration);
	GetWorld()->GetTimerManager().SetTimer(TimelineHandle, this, &UAction::TickTimeline, GetWorld()->GetDeltaSeconds(), true,0.0f);
}

void UAction::TickTimeline_Implementation()
{
	if (Timeline.IsPlaying() && 
		!GetOwningComponent()->ContainsStatus(DamagedTags))
	{
		Timeline.TickTimeline(GetWorld()->GetDeltaSeconds());
	}
	else {
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		ActionTimelineFinished();
	}
}

void UAction::ActionProgress_Implementation(float Value)
{
	PlaybackPosition = Timeline.GetPlaybackPosition();

	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::Printf(TEXT("EffectProgress: timeline: % f value : % f"), Timeline.GetPlaybackPosition(), Value));
}

void UAction::ActionTimelineFinished_Implementation()
{

}
