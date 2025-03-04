// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "HighlightedTile.generated.h"

class UNiagaraComponent;
class USceneComponent;

UCLASS(Blueprintable, BlueprintType)
class TACTICSPLUGIN_API AHighlightedTile : public AActor
{
	GENERATED_BODY()
public:

	AHighlightedTile();

	UFUNCTION(BlueprintCallable)
		void SetColor(FLinearColor Color_1, FLinearColor Color_2);

	UFUNCTION(BlueprintCallable)
		void SetScale(float Scale);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		UNiagaraComponent* HighlightedTile;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default", meta = (ExposeOnSpawn = "true"))
		FLinearColor BaseColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default", meta = (ExposeOnSpawn = "true"))
		FLinearColor SecondaryColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default", meta = (ExposeOnSpawn = "true"))
		float BoxScale;

protected:
	void BeginPlay() override;

};
