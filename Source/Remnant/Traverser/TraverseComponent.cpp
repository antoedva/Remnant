// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"
#include "../FP_Character.h"
#include "DimensionTrigger.h"
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"

UTraverseComponent::UTraverseComponent()
	: traverse_allowed_(true)
{
}

void UTraverseComponent::TraverseDimension()
{
	if (!traverse_allowed_)
		return;

	TArray<ULevelStreaming*> levels = GetWorld()->GetStreamingLevels();
	for (ULevelStreaming* level : levels)
		level->SetShouldBeVisible(!level->ShouldBeVisible());
}

void UTraverseComponent::BeginPlay()
{
	Super::BeginPlay();

}