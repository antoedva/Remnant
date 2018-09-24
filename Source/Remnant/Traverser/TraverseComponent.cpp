// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"
#include "../FP_Character.h"
#include "DimensionTrigger.h"
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

UTraverseComponent::UTraverseComponent()
	: traverse_allowed_(true)
	, dimension_(PRESENT)
{
}

void UTraverseComponent::TraverseDimension()
{
	if (!traverse_allowed_)
		return;

	// Get all streaming levels (the ones under "Persistent Level" in the "Levels" tab)
	const TArray<ULevelStreaming*> level_streams = GetWorld()->GetStreamingLevels();
	for (ULevelStreaming* level_stream : level_streams)
	{
		if (!level_stream)
			continue;

		// TODO: It's bad to compare by name, change this
		// World reference in editor did not work well
		const FString level_name = *level_stream->GetWorldAssetPackageName();
		if (level_name.Compare("/Game/Dimensions/UEDPIE_0_ObjectLevel") == 0)
		{
			// Change visibility on items depending on which dimension is current 
			// TODO: Figure out if there should be two different actors/props, or if we just need to change material
			const TArray<AActor*> actors = level_stream->GetLoadedLevel()->Actors;
			for (AActor* actor : actors)
			{
				if (!actor)
					continue;

				ToggleObjectVisibility(actor);
			}
		}

		// Traverse
		else
			level_stream->SetShouldBeVisible(!level_stream->ShouldBeVisible());
	}

	// Set the current dimension to the other dimension
	dimension_ = dimension_ == PAST ? PRESENT : PAST;
}

void UTraverseComponent::BeginPlay()
{
	Super::BeginPlay();

	// This is kind of WET but oh well
	// https://en.wikipedia.org/wiki/Don%27t_repeat_yourself
	const TArray<ULevelStreaming*> level_streams = GetWorld()->GetStreamingLevels();
	for (ULevelStreaming* level_stream : level_streams)
	{
		// Set visibility on each stream to match the starting dimension
		const FString level_name = *level_stream->GetWorldAssetPackageName();
		if (level_name.Compare("/Game/Dimensions/UEDPIE_0_Past") == 0)
			level_stream->SetShouldBeVisible(false);
		else if (level_name.Compare("/Game/Dimensions/UEDPIE_0_Present") == 0)
			level_stream->SetShouldBeVisible(true);
		else if (level_name.Compare("/Game/Dimensions/UEDPIE_0_ObjectLevel") == 0)
		{
			const TArray<AActor*> actors = level_stream->GetLoadedLevel()->Actors;
			for (AActor* actor : actors)
			{
				if (!actor)
					continue;

				// Set visibility on each actor to match the starting dimension
				if (actor->ActorHasTag("Past"))
					actor->SetActorHiddenInGame(true);
				else if (actor->ActorHasTag("Present"))
					actor->SetActorHiddenInGame(false);
			}
		}
	}
}

void UTraverseComponent::ToggleObjectVisibility(AActor* actor)
{
	switch (dimension_)
	{
	case PAST:
	{
		if (actor->ActorHasTag("Past"))
			actor->SetActorHiddenInGame(true);
		else if (actor->ActorHasTag("Present"))
			actor->SetActorHiddenInGame(false);
		break;
	}
	case PRESENT:
	{
		if (actor->ActorHasTag("Past"))
			actor->SetActorHiddenInGame(false);
		else if (actor->ActorHasTag("Present"))
			actor->SetActorHiddenInGame(true);
		break;
	}
	default:
		break;
	}
}
