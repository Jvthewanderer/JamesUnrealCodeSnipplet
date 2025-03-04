// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "../Grid/Grid.h"
#include "../Utility/TacTileType.h"
#include "GridMeshInst.generated.h"

class UStaticMesh;
class UMaterialInterface;


UCLASS(Blueprintable, BlueprintType)
class TACTICSPLUGIN_API AGridMeshInst : public AActor
{
	GENERATED_BODY()
public:

	AGridMeshInst();

	UFUNCTION(BlueprintCallable)
		void AddInstace(FIntPoint Index, FTransform InstanceTransform,const TArray<ETileState> &States, ETileType TileType, bool DebugShowTiles);

	UFUNCTION(BlueprintCallable)
		void RemoveInstance(FIntPoint ItemToFind);

	UFUNCTION(BlueprintCallable)
		void ClearInstace();

	UFUNCTION(BlueprintCallable)
		void InitGridMeshInstance(UStaticMesh* Mesh, UMaterialInterface* Material, ECollisionEnabled::Type Collision, bool ColorBaseOnTileType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void GetColorFromStates(const TArray<ETileState>& TileStates, ETileType TileType, FLinearColor& OutColor, float& OutIsFilled, float& OutFullOpacity);
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Default")
		UInstancedStaticMeshComponent* InstancedStaticMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		TArray<FIntPoint> InstanceIndexes;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		bool IsColorBaseOnTileType;
};
