// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "../Utility/TacTileType.h"
#include "Grid.generated.h"

class UStaticMesh;
class UMaterialInstance;
class UDataTable;
class UChildActorComponent;
class USceneComponent;
class AGridVisuals;
class AGridPathfinding;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileDataUpdated, FIntPoint, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridGenerated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileStateUpdated, FIntPoint, Index);

UCLASS()
class TACTICSPLUGIN_API AGrid : public AActor
{
	GENERATED_BODY()

public:
	// CONSTRUCTOR
	AGrid();

	UFUNCTION(BlueprintCallable, Category = "Grid Creation")
	void SpawnGrid(FVector CenterLocation, FVector TileSize, FIntPoint TileCount, bool UseEnviroment);

	UFUNCTION(BlueprintCallable, Category = "Grid Creation")
	void DestroyGrid();

	UFUNCTION(BlueprintCallable, Category = "Grid Creation")
	void AddGridTile(FTileData Data, bool Debug_ShowTiles);

	UFUNCTION(BlueprintCallable, Category = "Tile State")
	void ClearStateFromTiles(ETileState State);

	UFUNCTION(BlueprintPure)
	bool IsTileWalkable(const FIntPoint& Index);

	/** Return Shape Data from a Data Table */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GridUtilities")
	FGridShapeData GetCurrentShapeData();

	UFUNCTION(BlueprintCallable, Category = "TileState")
		void AddStateToTile(FIntPoint Index, ETileState State);

	UFUNCTION(BlueprintCallable, Category = "Tile State")
		void RemoveStateToTile(FIntPoint Index, ETileState State);

	UFUNCTION(BlueprintPure, Category = "GridUtilities")
		bool IsIndexVaild(FIntPoint Index);

	UFUNCTION(BlueprintPure, Category = "Mouse Cursor")
		FVector GetTileScale();

	UFUNCTION(BlueprintPure, Category = "Mouse Cursor")
		FVector GetTileLocationFromGridIndex(FIntPoint Index);

	UFUNCTION(BlueprintCallable, Category = "Tile State")
		void RemoveGridTile(const FIntPoint& Index);

			/** Grab a Trace line to where the to place a Grid Tile on */
	UFUNCTION(BlueprintCallable, Category = "Grid Creation")
		void TraceForGround(FVector Location, FIntPoint Index, FVector& OutLocation, ETileType& TileType, EUnitDirection& Direction);

	UFUNCTION(BlueprintCallable, Category = "MouseCursor")
		void GetCursorLocationOnGrid(APlayerController* PlayerController, FVector& Location);

	UFUNCTION(BlueprintPure, Category = "GridUtilities")
		FRotator GetTileRotationFromGridIndex(FIntPoint GridIndex);

	UFUNCTION(BlueprintCallable, Category = "Mouse Cursor")
		FIntPoint GetTileIndexFromWorldLocation(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Mouse Cursor")
		FIntPoint GetTileIndexUnderCursor(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Tile State")
		void GetAllTilesWithState(ETileState State, TArray<FIntPoint>& Indexes); 

	UFUNCTION(BlueprintPure, Category = "Miscellanenous", meta = (CompactNodeTitle = "Successfule Compile"))
		bool CheckForCompileError();

			UFUNCTION(BlueprintCallable)
		FVector FindClosestTileToSnapToTrace(FVector Location,TArray<AActor*>& ActorsToIgnore);

protected:

	void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Grid Creation")
		void CalculateCenterAndBottomLeft(FVector& Center, FVector& BottomLeft);

	UFUNCTION(BlueprintPure)
		FVector ConvertFromWorldToGrid(FVector Location);

	UFUNCTION(BlueprintPure)
		FVector ConvertFromGridToWorld(FVector Location);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		UChildActorComponent* ChildActor_Pathfinding_AI;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		UChildActorComponent* ChildActor_Pathfinding_Reachables;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		UChildActorComponent* ChildActor_Pathfinding;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		UChildActorComponent* ChildActor_GridVisual;

	UPROPERTY(BlueprintReadOnly, Category = "Default")
		USceneComponent* DefaultSceneRoot;

	UPROPERTY(BlueprintReadOnly, Category = "Default", meta = (MultiLine = "true"))
		AGridPathfinding* GridPathfinding_Reachable;

	UPROPERTY(BlueprintReadOnly, Category = "Default", meta = (MultiLine = "true"))
		AGridPathfinding* GridPathfinding_AI;

	UPROPERTY(BlueprintReadOnly, Category = "Default", meta = (MultiLine = "true"))
		AGridPathfinding* GridPathfinding;

	UPROPERTY(BlueprintReadOnly, Category = "Default", meta = (MultiLine = "true"))
		FVector GridBottomLeftCornerLocation;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Default", meta = (MultiLine = "true"))
		FVector GridCenterLocation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default", meta = (MultiLine = "true"))
		FVector GridTileSize;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default", meta = (MultiLine = "true"))
		FIntPoint GridTileCount;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
		UDataTable* ShapeTable;

	UPROPERTY(BlueprintReadWrite, Category = "Default", meta = (MultiLine = "true"))
		AGridVisuals* GridVisuals;

	UPROPERTY(BlueprintReadWrite, Category = "Default", meta = (MultiLine = "true"))
		TMap<FIntPoint, FTileData> GridTiles;

	UPROPERTY(BlueprintReadWrite, Category = "Default", meta = (MultiLine = "true"))
		TMap<ETileState, FIntPointArray> TileStateToIndexed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default", meta = (MultiLine = "true"))
		double MinGridHeigth;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default", meta = (MultiLine = "true"))
		double HeightBetweenLevels;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Debug", meta = (MultiLine = "true"))
		bool bTraceGrid;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Debug")
		bool bShowGrid;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Grid")
		TEnumAsByte<ETraceTypeQuery> TraceChannelType;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Default", meta = (MultiLine = "true"))
		FOnTileDataUpdated OnTileDataUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Default", meta = (MultiLine = "true"))
		FOnGridDestroyed OnGridDestroyed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Default", meta = (MultiLine = "true"))
		FOnGridGenerated OnGridGenerated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Default", meta = (MultiLine = "true"))
		FOnTileStateUpdated OnTileStateUpdated;
};
