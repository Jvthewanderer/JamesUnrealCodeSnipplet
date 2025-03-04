// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionComponent.h"
#include "../Action/Action.h"
#include "../Game/DataAsset/Holo_DataAsset.h"
#include "AbilityComponent.h"

// Sets default values for this component's properties
UActionComponent::UActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	PrimaryComponentTick.bCanEverTick = true;

	// ...

	DamagedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("StatusEffect.KnockedBack")));
	DamagedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("StatusEffect.Disabled")));
}


// Called when the game starts
void UActionComponent::BeginPlay()
{
	Super::BeginPlay();

	TMap<UHolo_AttackModifier*, FAttackModifierInfo> emptyAttackMod;

	InitActions(emptyAttackMod, DefaultSpecialAttack, DefaultSpecialAttackLevel);
	
}


void UActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<UAction*>ActionCopy = Actions;
	for (UAction* Action : ActionCopy)
	{
		if (Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}

	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UActionComponent* UActionComponent::GetActionComponent(AActor* FromActor)
{
	if (FromActor){
		return Cast<UActionComponent>(FromActor->GetComponentByClass(UActionComponent::StaticClass()));
	}
	return nullptr;
}

void UActionComponent::InitActions(const TMap<UHolo_AttackModifier*, FAttackModifierInfo>& newAttackModifiers, UHolo_Skill* SpecialActionMap, int32 newSpecialAttackLevel)
{
	if (GetOwner()->HasAuthority()){
		ClearActions();
		if (newAttackModifiers.Num() > 0)
		{
			DefaultAttackModifiers = newAttackModifiers;
		}

		if (SpecialActionMap) {
			DefaultActions.Add(SpecialActionMap->ActionClass);
			SpecialAttackLevel = newSpecialAttackLevel;
		}

		TArray<FAttackModifierInfo> ModArray;
		TArray<UHolo_AttackModifier*> ModSubclassArray;
		DefaultAttackModifiers.GenerateValueArray(ModArray);
		DefaultAttackModifiers.GenerateKeyArray(ModSubclassArray);
		int32 i = 0;

		for (FAttackModifierInfo currentInfo : ModArray) {
			AttackModifiersMap.Add(currentInfo.ActionName, ModSubclassArray[i]);
			i++;
		}

		for (TSubclassOf<UAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}

	}
}

void UActionComponent::AddAction(AActor* Instigator, TSubclassOf<UAction> ActionClass)
{
	if (!ensure(ActionClass)){
		return;
	}

	UAction* NewAction = NewObject<UAction>(GetOwner(), ActionClass);

	if (NewAction){
		if (AttackModifiersMap.Num() > 0){
			if (AttackModifiersMap.Contains(NewAction->ActionName)) {
				UHolo_AttackModifier* AttackMod = *AttackModifiersMap.Find(NewAction->ActionName);
				NewAction->Initialize(this, AttackMod->AttackModClass, DefaultAttackModifiers[*AttackModifiersMap.Find(AttackMod->ActionExecutionName)].Stack);
			}
			else {
				NewAction->Initialize(this, nullptr, 0);
			}
		}
		else {
			NewAction->Initialize(this, nullptr, 0);
		}

		if (NewAction->bModifyAction) {
			NewAction->SetLevel(SpecialAttackLevel);
		}

		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator))){
			NewAction->StartAction(Instigator);
		}
	}
}

void UActionComponent::RemoveAction(UAction* ActionToRemove)
{
	if (ensure(ActionToRemove && !ActionToRemove->IsRunning())){
		return;
	}
	Actions.Remove(ActionToRemove);
}

void UActionComponent::ClearActions()
{
	Actions.Empty();
}

UAction* UActionComponent::GetAction(TSubclassOf<UAction> ActionClass) const
{
	for (UAction* Action : Actions){
		if (Action && Action->IsA(ActionClass)){
			return Action;
		}
	}
	return nullptr;
}

UAction* UActionComponent::GetActionByName(FName ActionName) const
{
	for (UAction* Action : Actions){
		if (Action && Action->ActionName == ActionName){
			return Action;
		}
	}
	return nullptr;
}

TArray<UAction*> UActionComponent::GetAllAction() const
{
	return Actions;
}

bool UActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UAction* Action : Actions){
		if (Action && Action->ActionName == ActionName){
			if (!Action->CanStart(Instigator) || ContainsStatus(DamagedTags)){
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FailedMsg);
				continue;
			}

			// Do Cosmetic thing So player don't feel like they're waiting for an action to happen.
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UAction* Action : Actions){
		if (Action && Action->ActionName == ActionName){
			if (Action->IsRunning()){
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

bool UActionComponent::TriggerActionByName(AActor* Instigator, FName ActionName)
{
	for (UAction* Action : Actions){
		if (Action && Action->ActionName == ActionName){
			Action->TriggerAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UActionComponent::TriggerActionFromAnimNotifyByName(AActor* Instigator, FName ActionName, FName SocketName)
{
	for (UAction* Action : Actions){
		if (Action && Action->ActionName == ActionName){
			Action->TriggerActionFromAnimNotify(Instigator, SocketName);
			return true;
		}
	}
	return false;
}

bool UActionComponent::ConfirmAction(AActor* Instigator)
{
	if (IsTargeting()){
		for (UAction* Action : Actions){
			if (Action && Action->IsTargeting()){
				Action->TriggerAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

bool UActionComponent::CancleAction(AActor* Instigator)
{
	if (IsTargeting()){
		for (UAction* Action : Actions){
			if (Action && Action->IsTargeting()){
				Action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

void UActionComponent::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;
}

bool UActionComponent::CanSetStatus(FGameplayTag StatusEffectTag)
{
	UAbilityComponent* OwnerAbilityComp = UAbilityComponent::GetAbilityComponent(GetOwner());

	if (!StatusAilmentsGameplayTags.HasTag(StatusEffectTag) && !OwnerAbilityComp->GetBlockedStatusEffect().HasTag(StatusEffectTag)) {
		return true;
	}
	return false;
}

void UActionComponent::AddStatus(FGameplayTag StatusEffectTag)
{
	if (CanSetStatus(StatusEffectTag)) {
		StatusAilmentsGameplayTags.AddTag(StatusEffectTag);
	}
}

void UActionComponent::RemoveStatus(FGameplayTag StatusEffectTag)
{
	StatusAilmentsGameplayTags.RemoveTag(StatusEffectTag);
}

bool UActionComponent::ContainsStatus(FGameplayTagContainer StatusEffectTags)
{
	return StatusAilmentsGameplayTags.HasAny(StatusEffectTags);
}

