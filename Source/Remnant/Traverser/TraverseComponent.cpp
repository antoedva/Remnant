// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"
#include "../FP_Character.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "DimensionTrigger.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"

#define print(format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT(format), ##__VA_ARGS__), false)

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
		FString level_name;
		if (GetWorld()->IsPlayInEditor())
			level_name = FPackageName::GetShortName(level_stream->PackageNameToLoad);
		else
			level_name = FPackageName::GetShortName(level_stream->GetWorldAssetPackageName());

		if (level_name.Compare("ObjectLevel") == 0)
		{
			// Change visibility on items depending on which dimension is current 
			// TODO: Figure out if there should be two different actors/props, or if we just need to change material
			const TArray<AActor*> actors = level_stream->GetLoadedLevel()->Actors;
			for (AActor* actor : actors)
			{
				if (!actor)
					continue;

				if (!actor->HasValidRootComponent())
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

	// TODO:
	// Move level loading to seperate class
	const TArray<ULevelStreaming*> level_streams = GetWorld()->GetStreamingLevels();
	for (ULevelStreaming* level_stream : level_streams)
	{
		FString level_name;
		FName full_level_name;
		FLatentActionInfo latent_info;

		// This is just... bad...
		if (GetWorld()->IsPlayInEditor())
		{
			level_name = FPackageName::GetShortName(level_stream->PackageNameToLoad);
			full_level_name = level_stream->PackageNameToLoad;
		}
		else
		{
			level_name = FPackageName::GetShortName(level_stream->GetWorldAssetPackageName());
			full_level_name = level_stream->GetWorldAssetPackageFName();
		}

		UGameplayStatics::LoadStreamLevel(GetWorld(), full_level_name, true, true, latent_info);

		if (level_name.Compare("Past") == 0)
			level_stream->SetShouldBeVisible(false);

		else if (level_name.Compare("ObjectLevel") == 0)
		{

			const TArray<AActor*> actors = level_stream->GetLoadedLevel()->Actors;
			for (AActor* actor : actors)
			{
				if (!actor)
					continue;

				// Skip redundant actors e.g. WorldSettings.
				// Actor should never not have a root component if it's important here
				if (!actor->HasValidRootComponent())
					continue;

				// Get all components in the actor, max 2 (root, item)
				TArray<UActorComponent*, TInlineAllocator<2>> components;
				actor->GetComponents(components);

				for (auto* component : components)
				{
					UPrimitiveComponent* primitive_comp = Cast<UPrimitiveComponent>(component);
					if (!primitive_comp)
						continue;
					
					if (actor->ActorHasTag("Past"))
					{
						primitive_comp->SetCollisionResponseToAllChannels(ECR_Overlap);
						actor->SetActorHiddenInGame(true);
					}

					else if (actor->ActorHasTag("Present"))
					{
						primitive_comp->SetCollisionResponseToAllChannels(ECR_Block);
						actor->SetActorHiddenInGame(false);
					}

					component->OnActorEnableCollisionChanged(); // I don't know if I need this
				}
			}
		}

		level_stream->BroadcastLevelLoadedStatus(GetWorld(), level_stream->GetFName(), true);
	}
}

void UTraverseComponent::ToggleObjectVisibility(AActor* actor)
{
	// Get all components in the actor, max 2 (root, item)
	TArray<UActorComponent*, TInlineAllocator<2>> components;
	actor->GetComponents(components);

	// TODO:
	// Disable/Enable collision and physics on robots

	// We will call SetActorHiddenInGame several times if
	// If we have more than 1 component (excluding root), and the components have several items with the same tag
	// That is okay as SetActorHiddenInGame() checks if the new hidden is equal to the current hidden
	for (auto* component : components)
	{
		UPrimitiveComponent* primitive_comp = Cast<UPrimitiveComponent>(component);
		if (!primitive_comp)
			continue;

		switch (dimension_)
		{
		case PAST:
		{
			if (actor->ActorHasTag("Past"))
			{
				primitive_comp->SetCollisionResponseToAllChannels(ECR_Overlap);
				actor->SetActorHiddenInGame(true);
			}
			else if (actor->ActorHasTag("Present"))
			{
				primitive_comp->SetCollisionResponseToAllChannels(ECR_Block);
				actor->SetActorHiddenInGame(false);
			}
			break;
		}
		case PRESENT:
		{
			if (actor->ActorHasTag("Past"))
			{
				primitive_comp->SetCollisionResponseToAllChannels(ECR_Block);
				actor->SetActorHiddenInGame(false);
			}
			else if (actor->ActorHasTag("Present"))
			{
				primitive_comp->SetCollisionResponseToAllChannels(ECR_Overlap);
				actor->SetActorHiddenInGame(true);
			}
			break;
		}
		default:
			break;
		}

		component->OnActorEnableCollisionChanged();
	}
}
