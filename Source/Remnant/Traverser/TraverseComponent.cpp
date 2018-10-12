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
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Misc/PackageName.h"
#include "Public/TimerManager.h"

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

	//if (past_traverse_shader_.parameter_collection_)
	//	past_traverse_shader_.material_instance_ = GetWorld()->GetParameterCollectionInstance(past_traverse_shader_.parameter_collection_);
	//else
	//	UE_LOG(LogTemp, Error, TEXT("Could not find PC_Past in traverse_component_!"));

	//if (present_traverse_shader_.parameter_collection_)
	//	present_traverse_shader_.material_instance_ = GetWorld()->GetParameterCollectionInstance(present_traverse_shader_.parameter_collection_);
	//else
	//	UE_LOG(LogTemp, Error, TEXT("Could not find PC_Past in traverse_component_!"));

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

		if (level.Key == ALevelStreamManager::PAST)
			level.Value->SetShouldBeVisible(false);
		else if (level.Key == ALevelStreamManager::OBJECT)
		{
			const TArray<AActor*> actors = level.Value->GetLoadedLevel()->Actors;
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
						actor->SetActorHiddenInGame(true);
					}

					else 
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

//void UTraverseComponent::UpdateTraverseShaders()
//{
//	switch (dimension_)
//	{
//	case PAST:
//	{
//		const FCollectionScalarParameter* pc_distance = past_traverse_shader_.parameter_collection_->GetScalarParameterByName("Distance");
//		if (!pc_distance)
//		{
//			UE_LOG(LogTemp, Error, TEXT("Failed to get %s in PC_Past!"), *pc_distance->ParameterName.ToString());
//			return;
//		}
//
//		//past_traverse_shader_.material_instance_->SetScalarParameterValue(pc_distance->ParameterName, past_traverse_shader_.current_distance_);
//
//		break;
//	}
//	case PRESENT:
//	{
//		break;
//	}
//	default:
//		break;
//	}
//}
//
//void UTraverseComponent::StartShaderTimer(FTraverseShader shader)
//{
//	FTimerManager& tm = GetWorld()->GetTimerManager();
//	if (tm.IsTimerActive(shader.shader_timer_handle_))
//		return;
//
//	// Get parameter collections and check if they are valid
//	const FCollectionScalarParameter* alpha_1 = past_traverse_shader_.parameter_collection_->GetScalarParameterByName("Alpha1");
//	const FCollectionScalarParameter* alpha_2 = past_traverse_shader_.parameter_collection_->GetScalarParameterByName("Alpha2");
//	if (!alpha_1 || !alpha_2)
//	{
//		UE_LOG(LogTemp, Error, TEXT("Failed to get alpha value in %s! Please check the spelling!"), *shader.parameter_collection_->GetName());
//		return;
//	}
//
//	// Get alpha from one PC, no need to get both
//	float current_alpha;
//	shader.material_instance_->GetScalarParameterValue(alpha_1->ParameterName, current_alpha);
//
//	// Flip between 1 or 0 alpha
//	shader.material_instance_->SetScalarParameterValue(alpha_1->ParameterName, current_alpha == 0 ? 1.0f : 0.0f);
//	shader.material_instance_->SetScalarParameterValue(alpha_2->ParameterName, current_alpha == 0 ? 1.0f : 0.0f);
//
//	// Start the timer
//	tm.SetTimer(shader.shader_timer_handle_, shader.duration_, false);
//
//	//tm.SetTimer(shader.shader_timer_handle_, this, &UTraverseComponent::OnShaderTimerEnd, shader.duration_, false);
//}
//
//void UTraverseComponent::OnShaderTimerEnd()
//{
//}
