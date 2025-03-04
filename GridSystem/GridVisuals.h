// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "../Grid/Grid.h"
#include "../Utility/TacTileType.h"
#include "GridVisuals.generated.h"

class AGridMeshInst;
class UChildActorComponent;
class USceneComponent;

UCLASS()
class TACTICSPLUGIN_API AGridVisuals : public AActor
{
	GENERATED_BODY()
public:
	
	AGridVisuals();

	UFUNCTION(BlueprintCallable)
		void InitGridVisusal(AGrid* Grid);

	UFUNCTION(BlueprintCallable)
		void SetOffsetFromGround(double Offset);

	UFUNCTION(BlueprintCallable)
		void DestroyGridVisual();

	UFUNCTION(BlueprintCallable)
		void UpdateTileVisuals(FTileData TileData, bool Debug_ShowTiles);

	UFUNCTION(BlueprintCallable)
		void UpdateTileVisualTatical(FTileData TileData);

	UFUNCTION(BlueprintCallable)
		void SetIsTatical(bool Tactical);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		TObjectPtr<UChildActorComponent> ChildActor_GridMeshInst_Tatical;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		TObjectPtr<UChildActorComponent> ChildActor_GridMeshInst;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		USceneComponent* Scene;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Default")
		TObjectPtr<AGrid> VisualGrid;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
		TObjectPtr<AGridMeshInst> GridMeshInst;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		double OffsetFromGround = 1.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
		TObjectPtr<AGridMeshInst> GridMeshInst_Tatical;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		double GridLowestZ;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		bool IsTactical = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		bool NeedToReGenerateTacticalOnNextEnable;
};
