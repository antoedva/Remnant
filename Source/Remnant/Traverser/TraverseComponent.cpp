// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"
#include "../FP_Character.h"
#include "../LevelStreamManager.h"
#include "../LevelStreamManager.h"
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
	for (auto& level : lsm_->GetAllLevels())
	{
		if (!level.Value)
			continue;

		if (level.Key == ALevelStreamManager::OBJECT)
		{
			// Change visibility on items depending on which dimension is current 
			const TArray<AActor*> actors = level.Value->GetLoadedLevel()->Actors;
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
			level.Value->SetShouldBeVisible(!level.Value->ShouldBeVisible());
	}

	// Set the current dimension to the other dimension
	dimension_ = dimension_ == PAST ? PRESENT : PAST;
}

void UTraverseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!lsm_bp_)
		return;

	lsm_ = Cast<ALevelStreamManager>(GetWorld()->SpawnActor<AActor>(lsm_bp_, FVector::ZeroVector, FRotator::ZeroRotator));
	if (!lsm_)
		return;

	// TODO: 
	// Find a better place to load all levels
	lsm_->LoadAllLevels();

	for (auto& level : lsm_->GetAllLevels())
	{
		if (!level.Value)
			continue;
		if (!level.Value->IsLevelLoaded())
		{
			UE_LOG(LogTemp, Error, TEXT("Level %s is not loaded! Make sure to set the streaming method to always loaded!"), 
				*FPackageName::GetShortName(level.Value->PackageNameToLoad.ToString()));
			continue;
		}

		if (level.Key == ALevelStreamManager::PAST)
			level.Value->SetShouldBeVisible(false);
		else if (level.Key == ALevelStreamManager::OBJECT)
		{
			const TArray<AActor*> actors = level.Value->GetLoadedLevel()->Actors;
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
	}
}

void UTraverseComponent::ToggleObjectVisibility(AActor* actor)
{
	// Get all components in the actor, max 2 (root, item)
	TArray<UActorComponent*, TInlineAllocator<2>> components;
	actor->GetComponents(components);

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
