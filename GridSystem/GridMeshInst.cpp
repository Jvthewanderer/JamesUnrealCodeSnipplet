// Fill out your copyright notice in the Description page of Project Settings.

#include "GridMeshInst.h"
#include "../Utility/TacTileType.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

AGridMeshInst::AGridMeshInst()
{
	PrimaryActorTick.bCanEverTick = false;
	InstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedStaticMesh");

	RootComponent = InstancedStaticMesh;

}

void AGridMeshInst::AddInstace(FIntPoint Index, FTransform InstanceTransform, const TArray<ETileState>& States, ETileType TileType, bool DebugShowTiles)
{
	// Initialize variables
	int32 i;
	FLinearColor OutColor;
	float OutIsFilled = 0.0f;
	float OutFullOpacity = 0.0f;
	float DebugTileValue = 0.0f;

	// Remove instance at Index
	RemoveInstance(Index);

	// Add a new instance with InstanceTransform
	InstancedStaticMesh->AddInstance(InstanceTransform);

	// Add Index to InstanceIndexes and find its position in the array
	InstanceIndexes.Add(Index);
	InstanceIndexes.Find(Index, i);

	// Get color information from states
	GetColorFromStates(States, TileType, OutColor, OutIsFilled, OutFullOpacity);

	// Set custom data values in the InstancedStaticMesh
	InstancedStaticMesh->SetCustomDataValue(i, 0, OutColor.R);
	InstancedStaticMesh->SetCustomDataValue(i, 1, OutColor.G);
	InstancedStaticMesh->SetCustomDataValue(i, 2, OutColor.B);
	InstancedStaticMesh->SetCustomDataValue(i, 3, OutIsFilled);

	// Set DebugTileValue based on condition
	DebugTileValue = DebugShowTiles ? 1.0 : OutFullOpacity;

	// Set custom data value for DebugTileValue
	InstancedStaticMesh->SetCustomDataValue(i, 4, DebugTileValue);
}

void AGridMeshInst::RemoveInstance(FIntPoint ItemToFind)
{
	// Check if the list of instance indexes is empty
	if (InstanceIndexes.IsEmpty())
	{
		return;
	}

	int32 Index;
	// Check if the item to find exists in the list of instance indexes
	if (InstanceIndexes.Contains(ItemToFind))
	{
		// Find the index of the item to find
		InstanceIndexes.Find(ItemToFind, Index);
    
		// Remove the instance at the found index
		InstancedStaticMesh->RemoveInstance(Index);
    
		// Remove the item from the list of instance indexes
		InstanceIndexes.Remove(ItemToFind);
	}
}

void AGridMeshInst::ClearInstace()
{
	InstancedStaticMesh->ClearInstances();
	InstanceIndexes.Empty();
}

void AGridMeshInst::InitGridMeshInstance(UStaticMesh* Mesh, UMaterialInterface* Material, ECollisionEnabled::Type Collision, bool ColorBaseOnTileType)
{
	InstancedStaticMesh->SetStaticMesh(Mesh);
	InstancedStaticMesh->SetMaterial(0,Material);
	IsColorBaseOnTileType = ColorBaseOnTileType;
	InstancedStaticMesh->SetCollisionEnabled(Collision);
}

void AGridMeshInst::GetColorFromStates_Implementation(const TArray<ETileState>& TileStates, ETileType TileType, FLinearColor& OutColor, float& OutIsFilled, float& OutFullOpacity)
{
	OutColor = FLinearColor::Black;
	OutIsFilled = 1.0f;
	OutFullOpacity = 1.0f;
}



