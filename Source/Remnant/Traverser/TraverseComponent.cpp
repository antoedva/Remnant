// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"
#include "../FP_Character.h"
#include "../LevelStreamManager.h"
#include "../LevelStreamManager.h"
#include "Components/TimelineComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "DimensionTrigger.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Misc/PackageName.h"
#include "Public/TimerManager.h"
#include "Engine/Light.h"

#define print(format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT(format), ##__VA_ARGS__), false)

UTraverseComponent::UTraverseComponent(const FObjectInitializer& init)
	: traverse_allowed_(true)
	, dimension_(PRESENT)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTraverseComponent::TraverseDimension()
{
	if (!traverse_allowed_)
		return;

	if (!lsm_)
	{
		UE_LOG(LogTemp, Error, TEXT("Level manager not set in CH_Player->traverse_component_->LevelManager!"));
		return;
	}

	// Get all streaming levels (the ones under "Persistent Level" in the "Levels" tab)
	for (auto& level : lsm_->GetAllLevels())
	{
		if (!level.Value)
			continue;
		const TArray<AActor*> actors = level.Value->GetLoadedLevel()->Actors;

		if (use_old_traverse_)
		{
			if (level.Key == ALevelStreamManager::OBJECT)
			{
				// Change visibility on items depending on which dimension is current 
				for (AActor* actor : actors)
				{
					if (!actor)
						continue;

					if (!actor->HasValidRootComponent())
						continue;

					ToggleObjectVisibility(actor);
				}
			}
			else
				level.Value->SetShouldBeVisible(!level.Value->ShouldBeVisible());
		}
		else
			level_actor_arrays_.Add(level.Key, actors);
	}

	SpawnSphere();
	past_start_.Broadcast();
	present_start_.Broadcast();

	// Set the current dimension to the other dimension
	dimension_ = dimension_ == PAST ? PRESENT : PAST;
}

void UTraverseComponent::SpawnSphere()
{
	if (!sphere_bp_ || sphere_)
		return;

	sphere_ = GetWorld()->SpawnActor<AActor>(sphere_bp_, FVector(GetOwner()->GetActorLocation()), FRotator(0.0f));
	sphere_->SetActorScale3D(FVector(0.0f));
	UpdateLevelObjects();
}

void UTraverseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!lsm_bp_)
	{
		UE_LOG(LogTemp, Error, TEXT("Level manager not set in CH_Player->traverse_component_->LevelManager!"));
		return;
	}

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

		const TArray<AActor*> actors = level.Value->GetLoadedLevel()->Actors;

		if (level.Key == ALevelStreamManager::PAST)
		{
			if (use_old_traverse_)
				level.Value->SetShouldBeVisible(false);
			else
			{
				for (auto* actor : actors)
				{
					auto* light = Cast<ALight>(actor);
					if (light)
					{
						light->ToggleEnabled();
						continue;
					}
					actor->SetActorEnableCollision(false);
					//actor->SetActorHiddenInGame(true);
				}
			}
		}

		else if (level.Key == ALevelStreamManager::OBJECT)
		{
			for (AActor* actor : actors)
			{
				if (!actor)
					continue;

				// Skip redundant actors e.g. WorldSettings
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
						if (use_old_traverse_)
							actor->SetActorHiddenInGame(true);
					}

					else if (actor->ActorHasTag("Present"))
					{
						primitive_comp->SetCollisionResponseToAllChannels(ECR_Block);
						if (use_old_traverse_)
							actor->SetActorHiddenInGame(false);
					}

					component->OnActorEnableCollisionChanged(); // I don't know if I need this
				}
			}
		}
	}
}

void UTraverseComponent::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function)
{
	Super::TickComponent(delta_time, tick_type, this_tick_function);

	if (sphere_)
	{
		if (!UpdateLevelObjects())
			return;

		// TODO:
		// Calculate distance increase speed based on how long it should last
		// It should always cover the map no matter what
		current_distance_ += 1000.0f * delta_time;

		sphere_->SetActorScale3D(FVector(current_distance_ * sphere_scale_scale_)); // Something
		//sphere_->SetActorLocation(FVector(GetOwner()->GetActorLocation().X + current_distance_, 0.0f, 0.0f));
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

void UTraverseComponent::UpdateTraverseShaders()
{
	switch (dimension_)
	{
	case PAST:
	{
		const FCollectionScalarParameter* pc_distance = past_traverse_shader_.parameter_collection_->GetScalarParameterByName("Distance");
		if (!pc_distance)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get %s in PC_Past!"), *pc_distance->ParameterName.ToString());
			return;
		}

		//past_traverse_shader_.material_instance_->SetScalarParameterValue(pc_distance->ParameterName, past_traverse_shader_.current_distance_);

		break;
	}
	case PRESENT:
	{
		break;
	}
	default:
		break;
	}
}

void UTraverseComponent::StartShaderTimer(FTraverseShader shader)
{
	//FTimerManager& tm = GetWorld()->GetTimerManager();
	//if (tm.IsTimerActive(shader.shader_timer_handle_))
	//	return;

	// Get parameter collections and check if they are valid
	const FCollectionScalarParameter* alpha_1 = past_traverse_shader_.parameter_collection_->GetScalarParameterByName("Alpha1");
	const FCollectionScalarParameter* alpha_2 = past_traverse_shader_.parameter_collection_->GetScalarParameterByName("Alpha2");
	if (!alpha_1 || !alpha_2)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get alpha value in %s! Please check the spelling!"), *shader.parameter_collection_->GetName());
		return;
	}

	// Get alpha from one PC, no need to get both
	float current_alpha;
	shader.collection_instance_->GetScalarParameterValue(alpha_1->ParameterName, current_alpha);

	// Flip between 1 or 0 alpha
	shader.collection_instance_->SetScalarParameterValue(alpha_1->ParameterName, current_alpha == 0 ? 1.0f : 0.0f);
	shader.collection_instance_->SetScalarParameterValue(alpha_2->ParameterName, current_alpha == 0 ? 1.0f : 0.0f);

	//shader.distance_timeline_->
	// Start the timer
	//tm.SetTimer(shader.shader_timer_handle_, shader.duration_, false);

	//tm.SetTimer(shader.shader_timer_handle_, this, &UTraverseComponent::OnShaderTimerEnd, shader.duration_, false);
}

void UTraverseComponent::SortActors(AActor* player, TArray<AActor*> array_to_sort, TArray<AActor*>& output)
{
	array_to_sort.Sort([player](const AActor& a, const AActor& b)
	{
		return a.GetDistanceTo(player) > b.GetDistanceTo(player);
	});
	output = array_to_sort;
}

bool UTraverseComponent::UpdateLevelObjects()
{
	if (ChangeActorCollision())
		return true;

	GetWorld()->DestroyActor(sphere_);
	sphere_ = nullptr;
	current_distance_ = 0.0f;

	return false;
}

bool UTraverseComponent::ChangeActorCollision()
{
	bool is_empty[3] = { false, false, false };

	for (auto& a : level_actor_arrays_)
	{
		if (a.Value.Num() == 0)
		{
			is_empty[a.Key] = true;
			continue;
		}

		for (int32 i = 0; i < a.Value.Num(); ++i)
		{
			AActor* actor = a.Value[i];
			if (!actor)
				continue;

			// Go inside if distance is more than 10k, we won't ever see those actors either way
			if (actor->GetDistanceTo(GetOwner()) <= current_distance_ || actor->GetDistanceTo(GetOwner()) > 10000.0f)
			{
				// Check if the current actor is a light
				auto* light = Cast<ALight>(actor);
				// If it is, flip hidden
				if (light)
					light->ToggleEnabled();

				// If it's a sky sphere, flip hidden, this is ugly
				else if (actor->GetName().Compare("BP_Sky_Sphere") == 0 || actor->GetName().Compare("BP_Sky_Sphere_Present") == 0)
					actor->SetActorHiddenInGame(!actor->bHidden);

				else if (a.Key == ALevelStreamManager::OBJECT)
				{
					TArray<UActorComponent*, TInlineAllocator<2>> components;
					actor->GetComponents(components);

					for (auto* component : components)
					{
						UPrimitiveComponent* primitive_comp = Cast<UPrimitiveComponent>(component);
						if (!primitive_comp)
							continue;

						// Flip collision preset
						if (actor->ActorHasTag("Past") || actor->ActorHasTag("Present"))
							primitive_comp->SetCollisionResponseToAllChannels(primitive_comp->GetCollisionResponseToChannel(ECC_MAX) == ECR_Overlap ? ECR_Block : ECR_Overlap);
					}
				}
				else
					actor->SetActorEnableCollision(!actor->GetActorEnableCollision());

				a.Value[i] = a.Value[a.Value.Num() - 1];
				a.Value.Pop();
				i--;
			}
		}
	}

	if (is_empty[0] && is_empty[1] && is_empty[2])
		return false;

	return true;
}
//void UTraverseComponent::OnShaderTimerEnd()
//{
//}
