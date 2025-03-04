// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "../Global/Attributes.h"
#include "ActionComponent.generated.h"

class UAction;
class UAction_SpecialAttack;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, UActionComponent*, OwningComp, UAction*, Action);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOLOJAMPRJ_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Actions")
	bool bIsTargeting;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UAction>> DefaultActions;
	
	//UPROPERTY(EditAnywhere, Category = "Actions")
	//TSubclassOf<UAction> DefaultSpecialAttack;

	UPROPERTY(BlueprintReadOnly)
	TArray<UAction*> Actions;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TMap<UHolo_AttackModifier*, FAttackModifierInfo> DefaultAttackModifiers;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TObjectPtr<UHolo_Skill> DefaultSpecialAttack;

	UPROPERTY(BlueprintReadOnly)
	int32 SpecialAttackLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 DefaultSpecialAttackLevel = 0;

	TMap<UHolo_AttackModifier*, FAttackModifierInfo> AttackModifiers;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, UHolo_AttackModifier*> AttackModifiersMap;

	UPROPERTY(BlueprintReadOnly)
	bool bStunned;

	UPROPERTY(BlueprintReadOnly)
	bool bDisable;
	
	UPROPERTY(BlueprintReadOnly)
	bool bDodging;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Action")
	static UActionComponent* GetActionComponent(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void InitActions(const TMap<UHolo_AttackModifier*, FAttackModifierInfo>& newAttackModifiers, UHolo_Skill* SpecialActionMap, int32 newSpecialAttackLevel);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddAction(AActor* Instigator, TSubclassOf<UAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveAction(UAction* ActionToRemove);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ClearActions();

	UFUNCTION(BlueprintCallable, Category = "Action")
	UAction* GetAction(TSubclassOf<UAction> ActionClass) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	UAction* GetActionByName(FName ActionName) const;

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<UAction*> GetAllAction() const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool StopActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool TriggerActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool TriggerActionFromAnimNotifyByName(AActor* Instigator, FName ActionName,FName SocketName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ConfirmAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CancleAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetTargeting( bool NewTargeting);

	UFUNCTION(BlueprintPure, Category = "Action")
	bool IsTargeting(){ return bIsTargeting;}

	UFUNCTION(BlueprintPure, Category = "Action")
	bool IsStunned() { return bStunned; }

	UFUNCTION(BlueprintPure, Category = "Action")
	bool IsDisable() { return bDisable; }

	UFUNCTION(BlueprintPure, Category = "Action")
	bool isDodging() { return bDodging;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetStunned(bool newStunned) {bStunned = newStunned;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetDisabled(bool newDisable) {bStunned = newDisable;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SetDodging(bool newDodge) {bDodging = newDodge;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	bool CanSetStatus(FGameplayTag StatusEffectTag);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddStatus(FGameplayTag StatusEffectTag);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveStatus(FGameplayTag StatusEffectTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	bool ContainsStatus(FGameplayTagContainer StatusEffectTags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	FGameplayTagContainer GetStatusAilmentsGameplayTags(){return StatusAilmentsGameplayTags;}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer StatusAilmentsGameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer DamagedTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tag")
	FGameplayTag StunnedTag;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnActionStateChanged OnActionStopped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnActionStateChanged OnActionTriggered;
};
