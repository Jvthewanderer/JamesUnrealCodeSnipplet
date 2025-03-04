// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "../Ability/Ability_AttackModifier.h"
#include "../Global/Attributes.h"
#include "Components/TimelineComponent.h"
#include "Action.generated.h"

class UActionComponent;
class UPaperFlipbook;
class UAbility;
class UHolo_Skill;
//class UAbility_AttackModifier;

/**
 * 
 */
UCLASS(Blueprintable)
class HOLOJAMPRJ_API UAction : public UObject
{
	GENERATED_BODY()
	
public:
    UAction();

    UPROPERTY(EditDefaultsOnly, Category = "Action")
    bool bAutoStart;

    UPROPERTY(EditDefaultsOnly, Category = "Action")
    FName ActionName;

    UPROPERTY()
    float TimeStarted;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
    float Duration = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	float CastTime = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
    float RecastTime = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
    bool HoldCast = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
    float BaseDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
	float ChanceOfAttack = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
	float DistanceToAttack = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
	float ActionSpeed = 1.0; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
	TArray<FName> ActionCommandNames;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
    int32 AddStack = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
	bool bAddStackToAbility = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
    int32 Levels = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
    bool bModifyAction = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Effect")
    TSubclassOf<UAbility> AbilityEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
    TSubclassOf<UAbility_AttackModifier> AttackModifierClass;

    UPROPERTY(BlueprintReadOnly, Category = "Action")
    TObjectPtr<UAbility_AttackModifier> AttackModifier;

    UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTagContainer GrantsTags;

    UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTagContainer BlockedTags;

    UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTag DamageType;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pattern")
    FAttackPattern ActionAttackPattern;

	 UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pattern")
	FActionModifierInfo ActionModifierInfo;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action Info")
    TObjectPtr<UHolo_Skill> ActionInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSoftObjectPtr<UTexture2D> Icon;

	bool bIsRunning = false;

	bool bTargeting = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action Info")
	bool bCanMoveWhileInAction = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action Info")
	bool bCanAimWhileInAction = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action Info")
	bool bUseCastTime = false;

public:

	void Initialize(UActionComponent* ActionComponent, TSubclassOf<UAbility_AttackModifier> AttackModifierClass, uint8 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetTimeRemaining() const;
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool IsTargeting() const;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void TriggerAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void TriggerAttackModifier();

	UFUNCTION(BlueprintCallable, Category = "Action")
	TSubclassOf<UAbility> GetAttackModifierEffect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void TriggerActionFromAnimNotify(AActor* Instigator, FName SocketName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void StartCooldown();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ClearCooldown();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReduceCooldown(float TimeToReduce);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	UActionComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	UAbilityComponent* GetAbilityComponent() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	float GetBaseDamage();

	float GetRecastTimer(){ return RecastTime;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	float UseCastTime() {
		if(bUseCastTime){return CastTime;}

		return Duration;
	}

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetLevel(int32 NewLevel){ 
		Levels = NewLevel; 
		ModifyAction(FActionModifierInfo());
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Info")
	void ModifyAction(FActionModifierInfo NewAbilityModifier);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting")
	float GetActionIndicatorWidth(ETracePattern Trace){ 
		switch (Trace)
		{
		case ETracePattern::Cone:
			return ActionAttackPattern.GetConeRadius();
			break;
		}

		return ActionAttackPattern.TraceWidth *2.f;
	}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting")
	float GetActionIndicatorLength(){ 

		return ActionAttackPattern.TraceDistance;
	}

	UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting")
	FGameplayTag GetAttackModifierTag() {return AttackModifier->Modifier;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting")
	FAbilityModifierInfo GetAbilityModifierFromAttackModifier();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action Timeline")
	void StartTimeline();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting")
	float GetAttackSpeedRate(){return ActionSpeed;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting")
	float GetActionPlayRate(){return ActionPlayRate /ActionSpeed;}

protected:
    UPROPERTY()
    UActionComponent* ActionComponent;

    UPROPERTY()
    UAbilityComponent* AbilityComponent;

    FTimeline Timeline;

    FTimerHandle TimelineHandle;

    FTimerHandle AimingHandle;

	FGameplayTagContainer DamagedTags;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float InterpSpeed = 2.0f;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    float PlayRate = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    float ActionPlayRate = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float PlaybackPosition = 0.0f;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    UCurveFloat* CurveTimeline;

    UFUNCTION(BlueprintNativeEvent, Category = "Action Timeline")
    void TickTimeline();

    UFUNCTION(BlueprintNativeEvent, Category = "Action Timeline")
    void ActionProgress(float Value);

    UFUNCTION(BlueprintNativeEvent, Category = "Action Timeline")
    void ActionTimelineFinished();

};
