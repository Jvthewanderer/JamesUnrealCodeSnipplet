// Fill out your copyright notice in the Description page of Project Settings.

#include "HighlightedTile.h"
#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"




void AHighlightedTile::BeginPlay()
{
	Super::BeginPlay();
	SetColor(BaseColor,SecondaryColor);
	SetScale(BoxScale);
}

AHighlightedTile::AHighlightedTile()
{
	HighlightedTile = CreateDefaultSubobject<UNiagaraComponent>("HighlightedTile");
	HighlightedTile->SetupAttachment(RootComponent);
}

void AHighlightedTile::SetColor(FLinearColor Color_1, FLinearColor Color_2)
{
	HighlightedTile->SetColorParameter(FName("Color_01"),Color_1);
	HighlightedTile->SetColorParameter(FName("Color_02"),Color_2);
}

void AHighlightedTile::SetScale(float Scale)
{
	HighlightedTile->SetFloatParameter(FName("Scale"),Scale);
}

