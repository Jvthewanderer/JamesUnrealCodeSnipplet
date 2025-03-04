// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "GridPathfinding.h"
#include "GridVisuals.h"
#include "GridModifier.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/HitResult.h"
#include "Engine/DataTable.h"
#include "../Utility/TacTileType.h"

// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");

	RootComponent = DefaultSceneRoot;
	ChildActor_GridVisual = CreateDefaultSubobject<UChildActorComponent>("ChildActor_GridVisual");
	ChildActor_GridVisual->SetupAttachment(DefaultSceneRoot);
	ChildActor_GridVisual->SetChildActorClass(AGridVisuals::StaticClass());

	ChildActor_Pathfinding_AI = CreateDefaultSubobject<UChildActorComponent>("ChildActor_Pathfinding_AI");
	ChildActor_Pathfinding_Reachables = CreateDefaultSubobject<UChildActorComponent>("ChildActor_Pathfinding_Reachables");
	ChildActor_Pathfinding = CreateDefaultSubobject<UChildActorComponent>("ChildActor_Pathfinding");

	ChildActor_Pathfinding_AI->SetupAttachment(DefaultSceneRoot);
	ChildActor_Pathfinding_Reachables->SetupAttachment(DefaultSceneRoot);
	ChildActor_Pathfinding->SetupAttachment(DefaultSceneRoot);
	
	ChildActor_Pathfinding_AI->SetChildActorClass(AGridPathfinding::StaticClass());
	ChildActor_Pathfinding_Reachables->SetChildActorClass(AGridPathfinding::StaticClass());
	ChildActor_Pathfinding->SetChildActorClass(AGridPathfinding::StaticClass());

}

void AGrid::OnConstruction(const FTransform& InTransform)
{
    Super::OnConstruction(InTransform);

    // Clear existing grid visuals
    if (GridVisuals)
    {
        GridVisuals->Destroy();
    }

	ChildActor_GridVisual->DestroyChildActor();
	ChildActor_GridVisual->CreateChildActor();
	GridVisuals = Cast<AGridVisuals>(ChildActor_GridVisual->GetChildActor());

	ChildActor_Pathfinding->DestroyChildActor();
	ChildActor_Pathfinding->CreateChildActor();
	GridPathfinding = Cast<AGridPathfinding>(ChildActor_Pathfinding->GetChildActor());

	ChildActor_Pathfinding_AI->DestroyChildActor();
	ChildActor_Pathfinding_AI->CreateChildActor();
	GridPathfinding_AI = Cast<AGridPathfinding>(ChildActor_Pathfinding_AI->GetChildActor());

	ChildActor_Pathfinding_Reachables->DestroyChildActor();
	ChildActor_Pathfinding_Reachables->CreateChildActor();
	GridPathfinding_Reachable = Cast<AGridPathfinding>(ChildActor_Pathfinding_Reachables->GetChildActor());

    // Spawn grid at actor location
    SpawnGrid(GetActorLocation(), GridTileSize, GridTileCount, true);
}

void AGrid::SpawnGrid(FVector CenterLocation, FVector TileSize, FIntPoint TileCount, bool UseEnvironment)
{
    GridCenterLocation = CenterLocation;

    GridPathfinding->Grid = this;
    GridPathfinding_AI->Grid = this;
    GridPathfinding_Reachable->Grid = this;

    if (GridVisuals)
    {
		GridVisuals->InitGridVisusal(this);
        DestroyGrid();

        CalculateCenterAndBottomLeft(GridCenterLocation, GridBottomLeftCornerLocation);

        FIntPoint Index;
        FTransform TileTransform;
        for (int32 LocX = 0; LocX < GridTileCount.X; LocX++)
        {
            for (int32 LocY = 0; LocY < GridTileCount.Y; LocY++)
            {
                Index = FIntPoint(LocX, LocY);
                TileTransform = FTransform(GetTileRotationFromGridIndex(Index), GetTileLocationFromGridIndex(Index), GetTileScale());

                if (UseEnvironment)
                {
                    FVector OutLocation;
                    ETileType OutTileType;
                    EUnitDirection OutUnitDirection;

                    TraceForGround(TileTransform.GetLocation(), Index, OutLocation, OutTileType, OutUnitDirection);
                    FTileData Data;
                    Data.Index = Index;
                    Data.Type = OutTileType;
                    Data.Transform = FTransform(TileTransform.GetRotation(), OutLocation, TileTransform.GetScale3D());
                    Data.StartGridDirection = OutUnitDirection;
                    AddGridTile(Data, bShowGrid);
                }
                else
                {
                    FTileData Data;
                    Data.Index = Index;
                    Data.Type = ETileType::Normal;
                    Data.Transform = TileTransform;
                    Data.StartGridDirection = EUnitDirection::North;
                    AddGridTile(Data, bShowGrid);
                }
            }
        }
    }

    OnGridGenerated.Broadcast();
}

FGridShapeData AGrid::GetCurrentShapeData()
{
	if (ShapeTable)
	{
		FString ContextString = "";
		FGridShapeData* Data = ShapeTable->FindRow<FGridShapeData>(FName(TEXT("Square")),ContextString);
		FGridShapeData Ret_Data = *Data;
		return Ret_Data;
	}

	return FGridShapeData();
}

void AGrid::DestroyGrid()
{
	GridTiles.Empty();
	if (GridVisuals)
	{
		GridVisuals->DestroyGridVisual();
	}
	OnGridDestroyed.Broadcast();
}

void AGrid::AddGridTile(FTileData Data, bool Debug_ShowTiles)
{
	GridTiles.Add(Data.Index, Data);
	GridVisuals->UpdateTileVisuals(Data, Debug_ShowTiles);
	OnTileDataUpdated.Broadcast(Data.Index);
}

void AGrid::CalculateCenterAndBottomLeft(FVector& Center, FVector& BottomLeft)
{
	int32 X = FMath::RoundToInt(GridTileSize.X);
	int32 Y = FMath::RoundToInt(GridTileSize.Y);

	FIntPoint GidTileSizeIntPoint(X, Y);

	Center = FVector(UKismetMathLibrary::GridSnap_Float(GridCenterLocation.X, GridTileSize.X),
					UKismetMathLibrary::GridSnap_Float(GridCenterLocation.Y, GridTileSize.Y) ,
					UKismetMathLibrary::GridSnap_Float(GridCenterLocation.Z, GridTileSize.Z));

	BottomLeft = GridCenterLocation - FVector(((GridTileCount - FIntPoint(GridTileCount.X % 2, GridTileCount.Y % 2)) / FIntPoint(2,2)) * GidTileSizeIntPoint,0.f);

}

FRotator AGrid::GetTileRotationFromGridIndex(FIntPoint GridIndex)
{
	return FRotator::ZeroRotator;
}

void AGrid::TraceForGround(FVector Location, FIntPoint Index, FVector& OutLocation, ETileType& TileType, EUnitDirection& Direction)
{
	ETileType Ret_Type = ETileType::Normal;
	bool Loc_IsHeightFound = false;
	bool Loc_IsHighlightedTileFound = false;
	float Radius = GridTileSize.X / 3.0;
	float HeightResult = 0.0f;

	TArray<FHitResult> OutHit;
	TArray<AActor*> ActorToIgnore;
	EDrawDebugTrace::Type Debug_Trace = bTraceGrid ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	UKismetSystemLibrary::SphereTraceMulti(this,Location + FVector(0,0,100000.0), Location - FVector(0, 0, 100000.0), Radius,
										TraceChannelType,false, ActorToIgnore, Debug_Trace, OutHit,true);

	if (OutHit.Num() > 0)
	{
		Loc_IsHeightFound = false;
		Loc_IsHighlightedTileFound = false;
		Ret_Type = ETileType::Normal;

		for (FHitResult Hit : OutHit)
		{
			AGridModifier* GridMod = Cast<AGridModifier>(Hit.GetActor());
			if (GridMod)
			{
				Ret_Type = GridMod->Type;
				if (Ret_Type == ETileType::StartingPoint)
				{
					// Return Direction
					Direction = GridMod->Direction;
					if (!Loc_IsHighlightedTileFound)
					{
						GridMod->AddHighlightedTile(Index);
						Loc_IsHighlightedTileFound = true;
					}

					if (GridMod->UseForTileHeight)
					{
						Loc_IsHeightFound = true;
						HeightResult = UKismetMathLibrary::GridSnap_Float(Hit.Location.Z - Radius, 0.0);
					}
				}
			}
			else if (!Loc_IsHeightFound)
			{
				HeightResult = UKismetMathLibrary::GridSnap_Float(Hit.Location.Z - Radius, 0.0);
			}
		}
	}
		//Return Location And Tile Type
		OutLocation.X = Location.X;
		OutLocation.Y = Location.Y;
		OutLocation.Z = HeightResult;
		TileType = Ret_Type;
}

void AGrid::GetCursorLocationOnGrid(APlayerController* PlayerController, FVector& Location)
{
	FHitResult Hit;
	FVector WLocation, WDirection,Intersection;
	float T;
	if (PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, Hit))
	{
		//Return Location
		Location = Hit.Location;
		return;
	}
	else if (PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery2, false, Hit))
	{
		//Return Location
		Location = Hit.Location;
		return;
	}
	else if (PlayerController->DeprojectMousePositionToWorld(WLocation,WDirection))
	{
		if (UKismetMathLibrary::LinePlaneIntersection(WLocation, WDirection * 9999999.0, UKismetMathLibrary::MakePlaneFromPointAndNormal(GridCenterLocation, FVector(0.0, 0.0, 1.0)), T, Intersection))
		{
			//Return Location 
			Location = Intersection;
			return;
		}
		else 
		{
			Location = FVector(-999.0, -999.0, -999.0);
			return;
		}
	}
	else
	{
		Location = FVector(-999.0,-999.0,-999.0);
		return;
	}
}

FIntPoint AGrid::GetTileIndexFromWorldLocation(FVector Location)
{
	FVector LocationOnGrid, LGridValue;
	FVector2D SnapLocationToGrid, Ret_Vec;

	LocationOnGrid = Location - GridCenterLocation;
	LGridValue = LocationOnGrid - FVector(100.0,100.0,0.0);

	SnapLocationToGrid = FVector2D(FVector(UKismetMathLibrary::GridSnap_Float(LGridValue.X, GridTileSize.X),
		UKismetMathLibrary::GridSnap_Float(LGridValue.Y, GridTileSize.Y),
		UKismetMathLibrary::GridSnap_Float(LGridValue.Z, GridTileSize.Z)));

	Ret_Vec = SnapLocationToGrid / FVector2D(GridTileSize);
	return FIntPoint(FMath::RoundToInt(Ret_Vec.X), FMath::RoundToInt(Ret_Vec.Y));
}

FIntPoint AGrid::GetTileIndexUnderCursor(APlayerController* PlayerController)
{
	FVector OutLoc;
	GetCursorLocationOnGrid(PlayerController,OutLoc);
	return GetTileIndexFromWorldLocation(OutLoc);
}

void AGrid::AddStateToTile(FIntPoint Index, ETileState State)
{
	if (!GridTiles.Contains(Index))
	{
		return;
	}
	FTileData Loc_TileData = *GridTiles.Find(Index);

	TArray<FIntPoint> OutIndexes;
	if (GridTiles.Contains(Index))
	{
		Loc_TileData.State.AddUnique(State);
		if (Loc_TileData.State.Num() >= 0)
		{
			//Update Data in GridTiles Map
			GridTiles.Add(Loc_TileData.Index, Loc_TileData);
			GetAllTilesWithState(State, OutIndexes);
			OutIndexes.Add(Index);
			TileStateToIndexed.Add(State,FIntPointArray(OutIndexes));

			GridVisuals->UpdateTileVisuals(Loc_TileData,bShowGrid);
			OnTileStateUpdated.Broadcast(Index);
		}
	}
}

void AGrid::RemoveStateToTile(FIntPoint Index, ETileState State)
{
	if (!GridTiles.Contains(Index))
	{
		return;
	}
	FTileData Loc_TileData = *GridTiles.Find(Index);
	TArray<FIntPoint> OutIndexes;
	if (GridTiles.Contains(Index))
	{
		Loc_TileData.State.Remove(State);
		if (!Loc_TileData.State.Contains(State))
		{
			//Update Data in GridTiles Map
			GridTiles.Add(Loc_TileData.Index, Loc_TileData);
			GetAllTilesWithState(State, OutIndexes);
			OutIndexes.Remove(Index);
			TileStateToIndexed.Add(State, FIntPointArray(OutIndexes));

			GridVisuals->UpdateTileVisuals(Loc_TileData, bShowGrid);
			OnTileStateUpdated.Broadcast(Index);
		}
	}
}

bool AGrid::IsIndexVaild(FIntPoint Index)
{
	return GridTiles.Contains(Index);
}

FVector AGrid::GetTileScale()
{
	return GridTileSize / GetCurrentShapeData().MeshSize;
}

FVector AGrid::GetTileLocationFromGridIndex(FIntPoint Index)
{
	return GridCenterLocation + (GridTileSize * FVector(Index.X, Index.Y, 0)) + FVector(100, 100, 1);
}

void AGrid::RemoveGridTile(const FIntPoint& Index)
{
	GridTiles.Remove(Index);
	if (!GridTiles.Contains(Index))
	{
		FTileData Loc_TileData;
		Loc_TileData.Index = Index;
		Loc_TileData.Type = ETileType::NONETileType;
		Loc_TileData.Transform = FTransform();
		Loc_TileData.UnitOnTile = nullptr;
		Loc_TileData.StartGridDirection = EUnitDirection::North;
		GridVisuals->UpdateTileVisuals(Loc_TileData,false);
		OnTileDataUpdated.Broadcast(Index);
	}
}

void AGrid::GetAllTilesWithState(ETileState State, TArray<FIntPoint>& Indexes)
{
	if (TileStateToIndexed.Contains(State))
	{
		FIntPointArray Ret_Array = *TileStateToIndexed.Find(State);
		Indexes.Append(Ret_Array.IntPointArray);
	}
	else
	{	TArray<FIntPoint> EmptyArray;
		Indexes = EmptyArray;
	}

}

void AGrid::ClearStateFromTiles(ETileState State)
{
	TArray<FIntPoint> OutIndexes;
	GetAllTilesWithState(State, OutIndexes);

	for (FIntPoint Int : OutIndexes)
	{
		RemoveStateToTile(Int,State);
	}
}

bool AGrid::IsTileWalkable(const FIntPoint& Index)
{
	if (GridTiles.Contains(Index))
	{
		FTileData Loc_TileData = *GridTiles.Find(Index);
		//TODO: Make BPF of this Tile Search
		TArray<ETileType> NotWalkableTile = { ETileType::NONETileType, ETileType::Obstacle };
		return !NotWalkableTile.Contains(Loc_TileData.Type);
	}

	return false;

}

bool AGrid::CheckForCompileError()
{
	return true;
}

FVector AGrid::FindClosestTileToSnapToTrace(FVector Location, TArray<AActor*>& ActorsToIgnore)
{
	FVector Start = FVector(Location.X,Location.Y, 10000000.0);
	FVector End = FVector(Location.X,Location.Y,-1000000000.0);
	FHitResult Hit;
	bool bHitLoc = UKismetSystemLibrary::LineTraceSingle(this, Start, End, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

	FVector HitLoc = bHitLoc ? Hit.Location : Location;
	
	return HitLoc + FVector(100.0,100.0,0);
}

FVector AGrid::ConvertFromWorldToGrid(FVector Location)
{
	return 	UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), Location);
}

FVector AGrid::ConvertFromGridToWorld(FVector Location)
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), Location);
}
