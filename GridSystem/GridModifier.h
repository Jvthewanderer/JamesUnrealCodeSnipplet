
#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "../Utility/TacTileType.h"
#include "GridModifier.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AGridModifier : public AActor
{
	GENERATED_BODY()
public:

	AGridModifier();

	UFUNCTION(BlueprintCallable)
		void AddHighlightedTile(FIntPoint Index);

	UFUNCTION(BlueprintCallable)
		void RemoveHighlightedTile();

	UFUNCTION(BlueprintCallable)
		void ClearHighlightedTile();

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		USceneComponent* Scene;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		UStaticMeshComponent* StaticMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		UArrowComponent* Arrow;

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		ETileType Type;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		float Filled;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Default", meta = (MultiLine = "true"))
		bool UseForTileHeight;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default")
		EUnitDirection Direction;

	UPROPERTY(BlueprintReadOnly,  Category = "Default")
		TArray<FIntPoint> HighlightedIndexes;
};
