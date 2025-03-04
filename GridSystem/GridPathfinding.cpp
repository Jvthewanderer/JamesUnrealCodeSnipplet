// Fill out your copyright notice in the Description page of Project Settings.

#include "GridPathfinding.h"
#include "../Utility/TacTileType.h"
#include "Components/SceneComponent.h"
#include "../Utility/Interface/TacPlayerActionInterface.h"
#include "../BLibrary/TacUtilityBFL.h"
#include "../Unit/Unit.h"
#include "../Component/CombatManagerComponent.h"
#include <stdlib.h>
#include "Kismet/KismetMathLibrary.h"
#include "UObject/NoExportTypes.h"
#include "Algo/Replace.h"
#include "Kismet/KismetArrayLibrary.h"


void AGridPathfinding::BeginPlay()
{
	Super::BeginPlay();
	Controller = UGameplayStatics::GetPlayerController(this, 0);
}


AGridPathfinding::AGridPathfinding()
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	RootComponent = DefaultSceneRoot;
	StepHeight = 10.0;
}

TArray<FPathfindingData> AGridPathfinding::GetValidTileNeighbors_Implementation(FIntPoint Index, bool IncludeDiagonals, const TArray<ETileType>& ValidTypes)
{
    // Array to store the neighboring tiles
    TArray<FPathfindingData> NeighborsArray;
    
    FTileData CurrentTileData, NeighborTileData;
    
    // Check if the current tile exists in the grid
    if (!Grid->GridTiles.Contains(Index))
    {
        return NeighborsArray;
    }

    // Get the combat manager component
    UCombatManagerComponent* CombatManager = ITacPlayerActionInterface::Execute_GetCombatManager(UGameplayStatics::GetPlayerController(this, 0));

    if (!CombatManager) 
    {
        return NeighborsArray;
    }

    CurrentTileData = *Grid->GridTiles.Find(Index);

    // Find the indexes of the neighboring tiles
    TArray<FIntPoint> NeighborIndexes = GetNeighborIndexes(Index, IncludeDiagonals);

    for (FIntPoint NeighborIndex : NeighborIndexes)
    {
        if (!Grid->GridTiles.Contains(NeighborIndex))
        {
            continue;
        }

        NeighborTileData = *Grid->GridTiles.Find(NeighborIndex);

        if (UTacUtilityBFL::IsTileTypeWalkable(NeighborTileData.Type))
        {
            // Check if the tile is being occupied by a unit in the same Team.
            // If so we can move past them, else skip this iteration.
            if (NeighborTileData.UnitOnTile)
            {
                if (!CombatManager->CurrentUnit)
                {
                    continue;
                }

                if (NeighborTileData.UnitOnTile->TeamIndex != CombatManager->CurrentUnit->TeamIndex)
                {
                    continue;
                }
            }
            
            // Check if we can move to this tile based on the unit's jump distance
            if (abs(NeighborTileData.Transform.GetLocation().Z - CurrentTileData.Transform.GetLocation().Z) <=
                StepHeight + (double)(MaxJump * 50))
            {
                FPathfindingData NeighborData;
                NeighborData.Index = NeighborTileData.Index;
                NeighborData.CostToEnterTile = UTacUtilityBFL::GetTileTypeCost(NeighborTileData.Type);
                NeighborData.CostFromStart = 999999;
                NeighborData.MinimumCostToTarget = 999999;
                NeighborData.PreviousIndex = Index;

                NeighborsArray.Add(NeighborData);
            }
        }
    }

    return NeighborsArray;
}


TArray<FIntPoint> AGridPathfinding::GetNeighborsIndexesForSquareFourSides_Implementation(FIntPoint Index, bool IncludeDiagonals)
{
	TArray<FIntPoint> OutPath;
	if (IncludeDiagonals)
	{
		OutPath = {Index + FIntPoint(1,0), Index + FIntPoint(1,1), Index + FIntPoint(0,1), Index + FIntPoint(-1,1),
				   Index + FIntPoint(-1,0), Index + FIntPoint(-1,-1), Index + FIntPoint(0,-1), Index + FIntPoint(1,-1)};

	}
	else
	{
		OutPath = { Index + FIntPoint(1,0), Index + FIntPoint(0,1), Index + FIntPoint(-1,0), Index + FIntPoint(0,-1) };
	}
	return OutPath;
}

TArray<FIntPoint> AGridPathfinding::GetNeighborsIndexesForSquareEightSides_Implementation(FIntPoint Index)
{
	TArray<FIntPoint> OutPath;

	OutPath = { Index + FIntPoint(1,0), Index + FIntPoint(1,1), Index + FIntPoint(0,1), Index + FIntPoint(-1,1),
		   Index + FIntPoint(-1,0), Index + FIntPoint(-1,-1), Index + FIntPoint(0,-1), Index + FIntPoint(1,-1) };

	return OutPath;
}

TArray<FIntPoint> AGridPathfinding::GetNeighborIndexes_Implementation(FIntPoint Index, bool IncludeDiagonals)
{
	TArray<FIntPoint> OutPath;

	OutPath = GetNeighborsIndexesForSquareFourSides(Index,IncludeDiagonals);

	return OutPath;
}

TArray<FIntPoint> AGridPathfinding::FindPath_Implementation(FIntPoint Start, FIntPoint Target, bool Diagonals, const TArray<ETileType>& TileTypes, 
															double Delay, double MaxMs, bool ReturnReachable, int32 PathLength, int32 PathJump)
{
	TArray<FIntPoint> OutPath;
	TArray<FIntPoint> EmptyPath;
	FIntPointArray OutPathTemp;

	StartIndex = Start;
	TargetIndex = Target;
	bIncludeDiagonals = Diagonals;
	ValidTileTypes = TileTypes;
	DelayBetweenIteration = Delay;
	MaxMsPerFrame = MaxMs;
	ReturnReachableTiles = ReturnReachable;
	MaxPathLength = PathLength;
	MaxJump = PathJump;

	ClearGeneratedData();
	if (IsInputDataVaild())
	{
		FPathfindingData PathData;
		PathData.Index = StartIndex;
		PathData.CostToEnterTile = 1;
		PathData.CostFromStart = 0;
		PathData.MinimumCostToTarget = GetMinimumCostBetweenTwoTiles(StartIndex, TargetIndex, bIncludeDiagonals);
		PathData.PreviousIndex = FIntPoint(-999,-999);
		
		DiscoverTile(PathData);

		if (DelayBetweenIteration <= 0.0)
		{
			while (DiscoveredTileIndexes.Num() > 0)
			{
				if (AnalyseNextDiscoveredTile())
				{
					OutPath = GeneratePath();
					OutPathTemp.IntPointArray = OutPath;
					OnPathfindingCompleted.Broadcast(OutPathTemp);
					return OutPath;
				}
			}

			OutPath = ReturnReachableTiles ? AnalysedTileIndexes : EmptyPath;
			OutPathTemp.IntPointArray = OutPath;
			OnPathfindingCompleted.Broadcast(OutPathTemp);
			return OutPath;
			
		}
		else
		{
			FindPathWithDelay();
			return EmptyPath;
		}
	}
	else
	{
		OutPathTemp.IntPointArray = EmptyPath;
		OnPathfindingCompleted.Broadcast(OutPathTemp);

		return EmptyPath;
	}

	return OutPath;
}

bool AGridPathfinding::IsInputDataVaild_Implementation()
{
	if (StartIndex != TargetIndex)
	{
		if (Grid->IsTileWalkable(StartIndex))
		{
			if (!ReturnReachableTiles)
			{
				if (Grid->IsTileWalkable(TargetIndex))
				{
					if (GetMinimumCostBetweenTwoTiles(StartIndex, TargetIndex, bIncludeDiagonals) <= MaxPathLength)
					{
						FTileData TileDataBreak = *Grid->GridTiles.Find(TargetIndex);

						if (ValidTileTypes.Contains(TileDataBreak.Type))
						{
							if (TileDataBreak.UnitOnTile)
							{
								return false;
							}
							else
							{
								return true;
							}
						}
					}
				}
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

void AGridPathfinding::DiscoverTile_Implementation(FPathfindingData TilePathData)
{
	PathfindingData.Add(TilePathData.Index,TilePathData);
	InsertTileInDiscoveredArray(TilePathData);
	OnPathfindingDataUpdated.Broadcast(TilePathData.Index);
}

int32 AGridPathfinding::GetMinimumCostBetweenTwoTiles_Implementation(FIntPoint Index_1, FIntPoint Index_2, bool Diagonals)
{
	int32 MinimumCost = 0;

	if (Diagonals)
	{
		MinimumCost = UKismetMathLibrary::Max(abs(FIntPoint(Index_1 - Index_2).X), abs(FIntPoint(Index_1 - Index_2).Y));
	}
	else
	{
		MinimumCost = abs(Index_1.X - Index_2.X) + abs(Index_1.Y - Index_2.Y);
	}
	return MinimumCost;
}

bool AGridPathfinding::AnalyseNextDiscoveredTile_Implementation()
{
	CurrentDiscoveredTile =  PullCheapestTileOutOfDiscoveredList();

	OnPathfindingDataUpdated.Broadcast(CurrentDiscoveredTile.Index);

	CurrentNeighbors = GetValidTileNeighbors(CurrentDiscoveredTile.Index,bIncludeDiagonals,ValidTileTypes);

	while (CurrentNeighbors.Num() > 0)
	{
		if (DiscoverNextNeighbor())
		{
			return true;
		}
	}

	return false;
}

TArray<FIntPoint> AGridPathfinding::GeneratePath_Implementation()
{
    // Initialize an array to store the inverted path
    TArray<FIntPoint> InvertedPath;
    
    // Initialize the current location to the target index
    FIntPoint Loc_Current = TargetIndex;

    // Traverse the path from target to start index
    while (Loc_Current != StartIndex)
    {
        // Add the current location to the inverted path
        InvertedPath.Add(Loc_Current);

        // Retrieve pathfinding data for the current location
        FPathfindingData LocPathData = *PathfindingData.Find(Loc_Current);

        // Update the current location to the previous index in the path
        Loc_Current = LocPathData.PreviousIndex;
    }

    // Reverse the inverted path to get the correct path from start to target
	// Using the Algo library to reverse Array because Unreal Doesn't have Reverse()
    Algo::Reverse(InvertedPath);

    // Return the final path
    return InvertedPath;
}

FPathfindingData AGridPathfinding::PullCheapestTileOutOfDiscoveredList_Implementation()
{
	FIntPoint TileIndex;
	FPathfindingData TilePathData;

	TileIndex = DiscoveredTileIndexes[0];

	DiscoveredTileSortingCost.RemoveAt(0);
	DiscoveredTileIndexes.RemoveAt(0);

	AnalysedTileIndexes.Add(TileIndex);
	TilePathData = *PathfindingData.Find(TileIndex);

	return TilePathData;
}

bool AGridPathfinding::DiscoverNextNeighbor_Implementation()
{
    int32 CostFromStart, IndexInDiscovered;
    FPathfindingData TilePathData;
    CurrentNeighbor = CurrentNeighbors[0];

    CurrentNeighbors.RemoveAt(0);

    if (!AnalysedTileIndexes.Contains(CurrentNeighbor.Index))
    {
        CostFromStart = CurrentDiscoveredTile.CostFromStart + CurrentNeighbor.CostToEnterTile;

        if (CostFromStart <= MaxPathLength)
        {
            IndexInDiscovered = DiscoveredTileIndexes.Find(CurrentNeighbor.Index);

            if (IndexInDiscovered == -1)
            {
                // Add logic here for newly discovered tile
            }
            else
            {
                CurrentNeighbor = *PathfindingData.Find(CurrentNeighbor.Index);

                if (CostFromStart < CurrentNeighbor.CostFromStart)
                {
                    DiscoveredTileIndexes.Remove(IndexInDiscovered);
                    DiscoveredTileSortingCost.Remove(IndexInDiscovered);
                }
                else
                {
                    // Return false if cost is not less
                    return false;
                }
            }

            TilePathData.Index = CurrentNeighbor.Index;
            TilePathData.CostToEnterTile = CurrentNeighbor.CostToEnterTile;
            TilePathData.CostFromStart = CostFromStart;
            TilePathData.MinimumCostToTarget = GetMinimumCostBetweenTwoTiles(CurrentNeighbor.Index, TargetIndex, bIncludeDiagonals);
            TilePathData.PreviousIndex = CurrentDiscoveredTile.Index;

            DiscoverTile(TilePathData);

            if (CurrentNeighbor.Index == TargetIndex)
            {
                // Return true if target found
                return true;
            }
            else
            {
                // Return false if target not found
                return false;
            }
        }
    }
    // Return false if conditions not met
    return false;
}

void AGridPathfinding::InsertTileInDiscoveredArray_Implementation(FPathfindingData TileData)
{
	int32 LocStartingCost = GetTileSortingCost(TileData);

	// Find the correct position to insert the tile based on its cost
	int32 InsertIndex = 0; // Default to 0 for insertion at the start
	for (; InsertIndex < DiscoveredTileSortingCost.Num(); ++InsertIndex)
	{
		if (LocStartingCost < DiscoveredTileSortingCost[InsertIndex])
		{
			// Found the position where the current tile's cost is less than the compared tile's cost
			break;
		}
	}

	// Insert at the found position or at the end if no suitable position was found
	DiscoveredTileSortingCost.Insert(LocStartingCost, InsertIndex);
	DiscoveredTileIndexes.Insert(TileData.Index, InsertIndex);

}

void AGridPathfinding::ClearGeneratedData_Implementation()
{
	PathfindingData.Empty();
	DiscoveredTileSortingCost.Empty();
	DiscoveredTileIndexes.Empty();
	AnalysedTileIndexes.Empty();
	OnPathFindingDataCleared.Broadcast();
}

bool AGridPathfinding::IsDiagonal_Implementation(FIntPoint Index_1, FIntPoint Index_2)
{
	TArray<FIntPoint> LocNeighbors;
	LocNeighbors = GetNeighborIndexes(Index_1,false);

	return !LocNeighbors.Contains(Index_2);
}

int32 AGridPathfinding::GetTileSortingCost_Implementation(FPathfindingData TileData)
{
	int32 AddOne = IsDiagonal(TileData.Index, TileData.PreviousIndex) ? 1 : 0;

	return AddOne + ((TileData.CostFromStart + TileData.MinimumCostToTarget) * 2);
}

int32 AGridPathfinding::GetPathCost_Implementation(const TArray<FIntPoint>& Path)
{
	int32 RetCost = 0;

	for (FIntPoint CurrentPath : Path)
	{
		FTileData TileDataBreak = *Grid->GridTiles.Find(CurrentPath);
		RetCost = RetCost + UTacUtilityBFL::GetTileTypeCost(TileDataBreak.Type);
	}
	return RetCost;
}

void AGridPathfinding::FindPathWithDelay_Implementation()
{
	TArray<FIntPoint> OutPath, EmptyPath;

// 	AsyncTask(ENamedThreads::AnyThread, []()
// 	{
// 		// This code will run asynchronously, without freezing the game thread
// 		while (true)
// 		{
// 			FDateTime LoopStartTime = UKismetMathLibrary::Now();
// 			if (DiscoveredTileIndexes.Num() > 0)
// 			{
// 				if (AnalyseNextDiscoveredTile_Recursive(LoopStartTime))
// 				{
// 					break;
// 				}
// 				else
// 				{
// 					FLatentActionInfo LatentInfo;
// 					LatentInfo.CallbackTarget = this;
// 					LatentInfo.ExecutionFunction = FName("FindPathWithDelay");
// 					UKismetSystemLibrary::RetriggerableDelay(this, DelayBetweenIteration, LatentInfo);
// 					continue;
// 				}
// 
// 			}
// 			else
// 			{
// 				OutPath = ReturnReachableTiles ? AnalysedTileIndexes : EmptyPath;
// 				OnPathfindingCompleted.Broadcast(OutPath);
// 				break;
// 			}
// 		}
// 	});

}

bool AGridPathfinding::AnalyseNextDiscoveredTile_Recursive(FDateTime LoopTime)
{
    TArray<FIntPoint> OutPath; // Stores the path found
    FIntPointArray OutPathTemp; // Temporary storage for path
    if (AnalyseNextDiscoveredTile())
    {
        OutPath = GeneratePath(); // Generate the path
        OutPathTemp.IntPointArray = OutPath; // Store the path in temporary storage
        OnPathfindingCompleted.Broadcast(OutPathTemp); // Broadcast the completed path
        return true; // Pathfinding successful
    }
    else
    {
        if (MaxMsPerFrame > 0.0f)
        {
            if (UKismetMathLibrary::GetTotalMilliseconds(UKismetMathLibrary::Now() - LoopTime) < MaxMsPerFrame)
            {
                AnalyseNextDiscoveredTile_Recursive(LoopTime); // Continue pathfinding recursively
            }
        }
        return false; // Pathfinding unsuccessful
    }

}