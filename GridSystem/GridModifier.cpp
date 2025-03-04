
#include "GridModifier.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"

AGridModifier::AGridModifier()
{
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");

	RootComponent = Scene;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Scene);
	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->SetupAttachment(Scene);

	Type = ETileType::Obstacle;
	Direction = EUnitDirection::North;

	StaticMesh->bHiddenInGame = true;

	UseForTileHeight = false;
	Filled = 1.0f;
}

void AGridModifier::AddHighlightedTile(FIntPoint Index)
{
	HighlightedIndexes.AddUnique(Index);
}

void AGridModifier::RemoveHighlightedTile()
{

}

void AGridModifier::ClearHighlightedTile()
{

}
