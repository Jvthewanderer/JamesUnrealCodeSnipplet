// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Public/TacGlobal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "../Utility/TacTileType.h"
#include "UObject/NoExportTypes.h"
#include "GridPathfinding.generated.h"

class USceneComponent;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathfindingDataUpdated, FIntPoint, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathfindingDataCleared);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathfindingCompleted, FIntPointArray, Path);


UCLASS(Blueprintable, BlueprintType)
class AGridPathfinding : public AActor
{
	GENERATED_BODY()
public:
	AGridPathfinding();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Neighbors")
		TArray<FPathfindingData> GetValidTileNeighbors(FIntPoint Index, bool IncludeDiagonals, const TArray<ETileType>& ValidTypes);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Neighbors")
		TArray<FIntPoint> GetNeighborsIndexesForSquareFourSides(FIntPoint Index, bool IncludeDiagonals);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Neighbors")
		TArray<FIntPoint> GetNeighborsIndexesForSquareEightSides(FIntPoint Index);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Neighbors")
		TArray<FIntPoint> GetNeighborIndexes(FIntPoint Index, bool IncludeDiagonals);

	/** Find the path base on the grid From the Start Point to the Target point */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		TArray<FIntPoint> FindPath(FIntPoint Start, FIntPoint Target, bool Diagonals, const TArray<ETileType>& TileTypes, double Delay, double MaxMs, bool ReturnReachable, int32 PathLength, int32 PathJump);

	/** Check If the Tile Data Is valid */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		bool IsInputDataVaild();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		void DiscoverTile(FPathfindingData TilePathData);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Utilities")
		int32 GetMinimumCostBetweenTwoTiles(FIntPoint Index_1, FIntPoint Index_2, bool Diagonals);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		bool AnalyseNextDiscoveredTile();

	/** Use PathfindingData to set up path from Starting point to End Point */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		TArray<FIntPoint> GeneratePath();

	/** Use the Cheapest Tile to use in the path */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		FPathfindingData PullCheapestTileOutOfDiscoveredList();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		bool DiscoverNextNeighbor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		void InsertTileInDiscoveredArray(FPathfindingData TileData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Utilities")
		void ClearGeneratedData();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Utilities")
		bool IsDiagonal(FIntPoint Index_1, FIntPoint Index_2);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Utilities")
		int32 GetTileSortingCost(FPathfindingData TileData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		int32 GetPathCost(const TArray<FIntPoint>& Path);

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable)
		void FindPathWithDelay();

	UFUNCTION()
		bool AnalyseNextDiscoveredTile_Recursive(FDateTime LoopTime);

protected:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		USceneComponent* DefaultSceneRoot;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		APlayerController* Controller;


	void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		AGrid* Grid;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Generated Data", meta = (MultiLine = "true"))
		TArray<FIntPoint> DiscoveredTileIndexes;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Generated Data", meta = (MultiLine = "true"))
		TArray<int32> DiscoveredTileSortingCost;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Generated Data", meta = (MultiLine = "true"))
		TArray<FIntPoint> AnalysedTileIndexes;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Generated Data", meta = (MultiLine = "true"))
		TArray<FPathfindingData> CurrentNeighbors;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Generated Data", meta = (MultiLine = "true"))
		TMap<FIntPoint, FPathfindingData> PathfindingData;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		double DelayBetweenIteration;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		double MaxMsPerFrame = 5.0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		TArray<ETileType > ValidTileTypes;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		FIntPoint StartIndex;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		FIntPoint TargetIndex;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Generated Data", meta = (MultiLine = "true"))
		FPathfindingData CurrentDiscoveredTile;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Generated Data", meta = (MultiLine = "true"))
		FPathfindingData CurrentNeighbor;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		int32 MaxPathLength;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		int32 MaxJump;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		double StepHeight;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		bool bIncludeDiagonals;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		bool ReturnReachableTiles;

	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		FOnPathfindingDataUpdated OnPathfindingDataUpdated;

	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		FOnPathfindingDataCleared OnPathFindingDataCleared;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		FOnPathfindingCompleted OnPathfindingCompleted;

};