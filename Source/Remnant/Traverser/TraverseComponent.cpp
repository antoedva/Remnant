// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"

#include "../FP_Character.h"
#include "DimensionTrigger.h"

#include "Curves/CurveFloat.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/ContainerAllocationPolicies.h"

#include "Engine/Engine.h"
#include "Engine/LevelBounds.h"
#include "Engine/Light.h"
#include "Engine/LevelStreamingVolume.h"
#include "Engine/World.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "Public/TimerManager.h"

#include "UObject/ConstructorHelpers.h"

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

	// To be removed when all the traverse materials are done
	if (use_old_traverse_)
	{
		// Get all streaming levels (the ones under "Persistent Level" in the "Levels" tab)
		for (auto& level : lsm_->GetAllLevels())
		{
			if (!level.Value)
				continue;

			ULevelStreaming* stream = level.Value->GetLevelStream();
			if (!stream)
				continue;

			const TArray<AActor*> actors = stream->GetLoadedLevel()->Actors;

			if (level.Key == LevelID::OBJECT)
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
				stream->SetShouldBeVisible(!stream->ShouldBeVisible());
		}
	}
	// !
	else
	{
		for (auto& level : lsm_->GetAllLevels())
		{
			if (!level.Value)
				continue;

			ULevelStreaming* stream = level.Value->GetLevelStream();
			if (!stream)
				continue;

			level_actor_arrays_.Add(level.Key, stream->GetLoadedLevel()->Actors);
		}

		SpawnSphere();
		timeline_.PlayFromStart();
		TraverseShaderStart(past_traverse_shader_);
		TraverseShaderStart(present_traverse_shader_);
		first_skipped_ = true;
	}

	// Set the current dimension to the other dimension
	dimension_ = dimension_ == PAST ? PRESENT : PAST;
}

void UTraverseComponent::SpawnSphere()
{
	if (!sphere_bp_)
		return;
	if (sphere_)
		GetWorld()->DestroyActor(sphere_);

	sphere_ = GetWorld()->SpawnActor<AActor>(sphere_bp_, FVector(GetOwner()->GetActorLocation()), FRotator(0.0f));
	sphere_->SetActorScale3D(FVector(0.0f));
	UpdateLevelObjects();
}

void UTraverseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!use_old_traverse_)
	{
		InitializeShaders();
		SetupTimeline();
	}

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

		ULevelStreaming* stream = level.Value->GetLevelStream();
		if (!stream)
			continue;

		if (!stream->IsLevelLoaded())
		{
			UE_LOG(LogTemp, Error, TEXT("Level %s is not loaded! Make sure to set the streaming method to always loaded!"),
				*FPackageName::GetShortName(stream->PackageNameToLoad.ToString()));
			continue;
		}

		const TArray<AActor*> actors = stream->GetLoadedLevel()->Actors;

		if (level.Key == LevelID::PAST)
		{
			if (use_old_traverse_)
				stream->SetShouldBeVisible(false);
			else
			{
				level_bounds_ = level.Value->GetLeveLBounds();
				level_length_ = level_bounds_.GetExtent().Distance(level_bounds_.Min, level_bounds_.Max);

				for (auto* actor : actors)
				{
					auto* light = Cast<ALight>(actor);
					if (light)
					{
						light->ToggleEnabled();
						continue;
					}
					actor->SetActorEnableCollision(false);
				}
			}
		}

		else if (level.Key == LevelID::OBJECT)
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

	if (use_old_traverse_)
		return;

	timeline_.TickTimeline(delta_time);

	if (sphere_)
	{
		if (!UpdateLevelObjects())
			return;

		// 2.81195079086115929701230228471 is magical yes, but it's a carefully calculated value that makes the sphere fit inside the level bounds
		// TODO: 
		// Move level_scale and max_scale outside of tick
		const FVector level_scale = lsm_->GetLevel(LevelID::PAST)->GetVolume()->GetActorScale();
		const FVector max_scale = level_scale * 2.81195079086115929701230228471f;
		// Set sphere scale based on timeline position and length
		const float tl_length = timeline_.GetTimelineLength();
		const float dv = max_scale.X;
		const float slope = dv / tl_length;
		const float val = slope * timeline_position_;

		sphere_->SetActorScale3D(FVector(val));
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

void UTraverseComponent::InitializeShaders()
{
	if (!present_traverse_shader_.parameter_collection_ || !past_traverse_shader_.parameter_collection_)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get collection instance! Make sure they are added to traverse shader"));
		return;
	}

	present_traverse_shader_.collection_instance_ = GetWorld()->GetParameterCollectionInstance(present_traverse_shader_.parameter_collection_);
	past_traverse_shader_.collection_instance_ = GetWorld()->GetParameterCollectionInstance(past_traverse_shader_.parameter_collection_);

	// Change default alpha values of past shaders here so we can see whats going on when not playing the game
	past_traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Alpha 1"), 1.0f);
	past_traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Alpha 2"), 0.0f);
}

void UTraverseComponent::TraverseShaderStart(FTraverseShader shader)
{
	auto* ci = shader.collection_instance_;
	if (!ci)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get collection instance! Make sure they are added to traverse shader"));
		return;
	}

	if (first_skipped_)
	{
		float alpha1;
		float alpha2;
		ci->GetScalarParameterValue(FName("Alpha 1"), alpha1);
		ci->GetScalarParameterValue(FName("Alpha 2"), alpha2);

		// Flip between 1 or 0 alpha
		ci->SetScalarParameterValue(FName("Alpha 1"), alpha1 == 0.0f ? 1.0f : 0.0f);
		ci->SetScalarParameterValue(FName("Alpha 2"), alpha2 == 0.0f ? 1.0f : 0.0f);
	}

	ci->SetVectorParameterValue(FName("Start Position"), GetOwner()->GetActorLocation());
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

	return false;
}

bool UTraverseComponent::ChangeActorCollision(const bool ignore_distance)
{
	bool is_empty[3] = { false, false, false };

	for (auto& a : level_actor_arrays_)
	{
		if (a.Value.Num() == 0)
		{
			is_empty[(int)a.Key] = true;
			continue;
		}

		for (int32 i = 0; i < a.Value.Num(); ++i)
		{
			AActor* actor = a.Value[i];
			if (!actor)
				continue;

			// Go inside if distance is more than map distance / 2, we won't notice the change to those either way
			if (actor->GetDistanceTo(GetOwner()) <= curve_value_ || actor->GetDistanceTo(GetOwner()) > level_length_ * 0.5f || ignore_distance)
			{
				// Check if the current actor is a light
				auto* light = Cast<ALight>(actor);
				// If it is, flip hidden
				if (light)
					light->ToggleEnabled();

				// If it's a sky sphere, flip hidden, this is ugly
				else if (actor->GetName().Compare("BP_Sky_Sphere_Past") == 0 || actor->GetName().Compare("BP_Sky_Sphere_Present") == 0)
					actor->SetActorHiddenInGame(!actor->bHidden);

				else if (a.Key == LevelID::OBJECT)
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

void UTraverseComponent::SetupTimeline()
{
	if (!curve_)
		return;

	FOnTimelineFloat tl_cb;
	FOnTimelineEventStatic tl_end_cb;

	tl_cb.BindUFunction(this, FName("TimelineCB"));
	tl_end_cb.BindUFunction(this, FName{ TEXT("TimelineEndCB") });
	timeline_.AddInterpFloat(curve_, tl_cb);
	timeline_.SetTimelineFinishedFunc(tl_end_cb);
}

void UTraverseComponent::TimelineCB()
{
	timeline_position_ = timeline_.GetPlaybackPosition();
	curve_value_ = curve_->GetFloatValue(timeline_position_);

	if (!past_traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Distance"), curve_value_))
		UE_LOG(LogTemp, Warning, TEXT("Failed to find distance paramater"));

	if (!present_traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Distance"), curve_value_))
		UE_LOG(LogTemp, Warning, TEXT("Failed to find distance paramater"));
}

void UTraverseComponent::TimelineEndCB()
{
	if (!past_traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Distance"), level_length_))
		UE_LOG(LogTemp, Warning, TEXT("Failed to find distance paramater"));

	if (!present_traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Distance"), level_length_))
		UE_LOG(LogTemp, Warning, TEXT("Failed to find distance paramater"));


	ChangeActorCollision(true);
	if (sphere_)
	{
		GetWorld()->DestroyActor(sphere_);
		sphere_ = nullptr;
	}
}

