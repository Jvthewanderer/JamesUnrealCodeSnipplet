// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GridVisuals.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridMeshInst.h"
#include "Components/ChildActorComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/HitResult.h"

AGridVisuals::AGridVisuals()
{
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;
	
	ChildActor_GridMeshInst = CreateDefaultSubobject<UChildActorComponent>("ChildActor_GridMeshInst");
	ChildActor_GridMeshInst->SetupAttachment(Scene);
	ChildActor_GridMeshInst->SetChildActorClass(AGridMeshInst::StaticClass());

	ChildActor_GridMeshInst_Tatical = CreateDefaultSubobject<UChildActorComponent>("ChildActor_GridMeshInst_Tatical");
	ChildActor_GridMeshInst_Tatical->SetupAttachment(Scene);
	ChildActor_GridMeshInst_Tatical->SetChildActorClass(AGridMeshInst::StaticClass());
}

void AGridVisuals::InitGridVisusal(AGrid* Grid)
{
	if (!Grid)
	{
		return;
	}
	else
	{
		VisualGrid = Grid;
	}
	
	GridMeshInst = Cast<AGridMeshInst>(ChildActor_GridMeshInst->GetChildActor());
	GridMeshInst_Tatical = Cast<AGridMeshInst>(ChildActor_GridMeshInst_Tatical->GetChildActor());
	
	if (!GridMeshInst)
	{
		return;
	}

	GridMeshInst->InitGridMeshInstance(VisualGrid->GetCurrentShapeData().FlatMesh, VisualGrid->GetCurrentShapeData().FlatMaterial,ECollisionEnabled::QueryOnly,false);
	GridMeshInst_Tatical->InitGridMeshInstance(VisualGrid->GetCurrentShapeData().Mesh, VisualGrid->GetCurrentShapeData().MeshMaterial, ECollisionEnabled::QueryOnly, false);

	FHitResult Hit;

	SetActorLocation(FVector::ZeroVector,false,nullptr);

	SetOffsetFromGround(OffsetFromGround);

	SetIsTatical(IsTactical);
	
}

void AGridVisuals::SetOffsetFromGround(double Offset)
{
	OffsetFromGround = Offset;
	
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y,OffsetFromGround));
}

void AGridVisuals::DestroyGridVisual()
{
	if (GridMeshInst)
	{
		GridMeshInst->ClearInstace();
	}

	if (GridMeshInst_Tatical)
	{
		GridMeshInst_Tatical->ClearInstace();
	}
}

void AGridVisuals::UpdateTileVisuals(FTileData TileData, bool Debug_ShowTiles)
{
	if (!GridMeshInst)
	{
		return;
	}

	TArray<ETileType> InstanceContains = {ETileType::NONETileType, ETileType::Obstacle};

	GridMeshInst->RemoveInstance(TileData.Index);

	if (!InstanceContains.Contains(TileData.Type))
	{
		GridMeshInst->AddInstace(TileData.Index, TileData.Transform, TileData.State, TileData.Type,Debug_ShowTiles);
	}

	UpdateTileVisualTatical(TileData);

}

void AGridVisuals::UpdateTileVisualTatical(FTileData TileData)
{
	if (TileData.Transform.GetLocation().Z < GridLowestZ)
	{
		GridLowestZ = TileData.Transform.GetLocation().Z;
	}

	if (IsTactical)
	{
		GridMeshInst_Tatical->RemoveInstance(TileData.Index);
		if (TileData.Type != ETileType::NONETileType)
		{
			double ScaleZ = (VisualGrid->GetCurrentShapeData().MeshSize.Z + (TileData.Transform.GetLocation().Z - GridLowestZ)) / VisualGrid->GetCurrentShapeData().MeshSize.Z;

			FTransform TDTransform = FTransform(TileData.Transform.GetRotation(), TileData.Transform.GetLocation(),FVector(TileData.Transform.GetScale3D().X, TileData.Transform.GetScale3D().Y, ScaleZ));

			GridMeshInst_Tatical->AddInstace(TileData.Index, TDTransform, TileData.State, TileData.Type, false);
		}
	}
	else
	{
		NeedToReGenerateTacticalOnNextEnable = true;
	}
}

void AGridVisuals::SetIsTatical(bool Tactical)
{
	ChildActor_GridMeshInst_Tatical->SetHiddenInGame(!Tactical,false);
	
	TArray<AActor*> OutActors;

	UGameplayStatics::GetAllActorsWithTag(this,"NotTactical",OutActors);

	for (AActor* NTacticalActor : OutActors)
	{
		NTacticalActor->SetActorHiddenInGame(Tactical);
	}
	if (Tactical && NeedToReGenerateTacticalOnNextEnable)
	{
		NeedToReGenerateTacticalOnNextEnable = false;

		TArray<FTileData> OutTileData;

		VisualGrid->GridTiles.GenerateValueArray(OutTileData);

		for (FTileData VGTileData : OutTileData)
		{
			UpdateTileVisualTatical(VGTileData);
		}
	}
}
