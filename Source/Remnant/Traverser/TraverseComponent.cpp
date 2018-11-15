// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"

#include "../FP_Character.h"
#include "DimensionTrigger.h"

#include "Curves/CurveFloat.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Containers/ContainerAllocationPolicies.h"

#include "Engine/Engine.h"
#include "Engine/LevelBounds.h"
#include "Engine/Light.h"
#include "Engine/DirectionalLight.h"
#include "Engine/LevelStreamingVolume.h"
#include "Engine/World.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "Public/TimerManager.h"

#include "UObject/ConstructorHelpers.h"

#include "PuzzleSystem/Actors/PickUpActor.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "PuzzleSystem/Actors/TriggerReceiverActor.h"
#include "PuzzleSystem/Actors/Triggers/VolumeTriggerActor.h"

#include "PuzzleSystem/Components/TriggerComponent.h"
#include "PuzzleSystem/Components/InventoryComponent.h"
#include "PuzzleSystem/Components/InteractComponent.h"

#include "Engine/ExponentialHeightFog.h"

#define print(format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT(format), ##__VA_ARGS__), false)

UTraverseComponent::UTraverseComponent(const FObjectInitializer& init)
	: dimension_(PRESENT)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTraverseComponent::TraverseDimension()
{
	if (!lsm_)
	{
		UE_LOG(LogTemp, Error, TEXT("Level manager not set in CH_Player->traverse_component_->LevelManager!"));
		return;
	}

	for (auto& level : lsm_->GetAllLevels())
	{
		if (!level.Value)
			continue;

		TArray<AActor*> past_actors;
		TArray<AActor*> present_actors;
		TArray<AActor*> obj_actors;

		switch (level.Key)
		{
		case LevelID::OBJECT:
		{
			for (auto* stream : level.Value->GetLevelStreams())
			{
				TArray<AActor*> actors = stream->GetLoadedLevel()->Actors;
				for (auto* actor : actors)
					obj_actors.Add(actor);
			}

			level_actor_arrays_.Add(level.Key, obj_actors);
			break;
		}
		case LevelID::PRESENT:
		{
			for (auto* stream : level.Value->GetLevelStreams())
			{
				TArray<AActor*> actors = stream->GetLoadedLevel()->Actors;
				for (auto* actor : actors)
					present_actors.Add(actor);;
			}

			level_actor_arrays_.Add(level.Key, present_actors);
		}
		case LevelID::PAST:
		{
			for (auto* stream : level.Value->GetLevelStreams())
			{
				TArray<AActor*> actors = stream->GetLoadedLevel()->Actors;
				for (auto* actor : actors)
					past_actors.Add(actor);
			}

			level_actor_arrays_.Add(level.Key, past_actors);
			break;
		}
		default:
			break;
		}

	}

	timeline_.PlayFromStart();
	SpawnSphere();
	TraverseShaderStart(traverse_shader_);
	first_skipped_ = true;

	// Set the current dimension to the other dimension
	dimension_ = dimension_ == PAST ? PRESENT : PAST;

	on_traverse_.Broadcast();
}

void UTraverseComponent::SpawnSphere()
{
	if (!sphere_bp_)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find sphere_bp_! Check CH_Player->TraverseComponent"));
		return;
	}
	if (sphere_)
		GetWorld()->DestroyActor(sphere_);

	sphere_ = GetWorld()->SpawnActor<AActor>(sphere_bp_, FVector(GetOwner()->GetActorLocation()), FRotator(0.0f));
	sphere_->SetActorScale3D(FVector(0.0f));
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

		TArray<ULevelStreaming*> streams = level.Value->GetLevelStreams();
		if (!(streams.Num() > 0))
			continue;

		for (auto* stream : streams)
		{
			if (!stream->IsLevelLoaded())
			{
				UE_LOG(LogTemp, Error, TEXT("Level %s is not loaded! Make sure to set the streaming method to always loaded!"),
					*FPackageName::GetShortName(stream->PackageNameToLoad.ToString()));
				continue;
			}

			const TArray<AActor*> actors = stream->GetLoadedLevel()->Actors;

			if (level.Key == LevelID::PAST)
			{
				level_bounds_ = level.Value->GetLevelBounds();
				level_length_ = level_bounds_.GetExtent().Distance(level_bounds_.Min, level_bounds_.Max);

				if (level_length_ == 0.0f)
					UE_LOG(LogTemp, Warning, TEXT("Level length is 0! Make sure you have a level streaming volume that covers the map!"));

				for (auto* actor : actors)
				{
					auto* light = Cast<ALight>(actor);
					if (light)
					{
						if(!light->IsA(ADirectionalLight::StaticClass()))
							light->ToggleEnabled();
					}

					// Toggle fog
					else if (actor->IsA(AExponentialHeightFog::StaticClass()))
						actor->SetActorHiddenInGame(true);

					actor->SetActorEnableCollision(false);
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

					for (auto* component : actor->GetComponents())
					{
						UPrimitiveComponent* primitive_comp = Cast<UPrimitiveComponent>(component);
						if (!primitive_comp)
							continue;

						if (component->IsA(UBoxComponent::StaticClass()))
							continue;

						if (actor->ActorHasTag("Past"))
						{
							if (actor->IsA(APickUpActor::StaticClass()) || actor->IsA(AInteractableActorBase::StaticClass())
								|| actor->IsA(AVolumeTriggerActor::StaticClass()) || actor->IsA(UTriggerComponent::StaticClass())
								|| actor->IsA(UInventoryComponent::StaticClass()) || actor->IsA(UInteractComponent::StaticClass()))
								primitive_comp->SetCollisionResponseToAllChannels(ECR_Ignore);
							else
								primitive_comp->SetCollisionResponseToAllChannels(ECR_Overlap);
						}

						else if (actor->ActorHasTag("Present"))
							primitive_comp->SetCollisionResponseToAllChannels(ECR_Block);

						component->OnActorEnableCollisionChanged(); // I don't know if I need this
					}
				}
			}
		}
	}

	InitializeShaders();
	SetupTimeline();
}

void UTraverseComponent::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function)
{
	Super::TickComponent(delta_time, tick_type, this_tick_function);

	timeline_.TickTimeline(delta_time);

	if (sphere_)
	{
		if (!UpdateLevelObjects())
			return;

		// magic_number_ is magical yes, but it's a carefully calculated value that makes the sphere fit inside the level bounds
		const float max_scale = curve_->GetFloatValue(timeline_length_) / magic_number_;
		// Set sphere scale based on timeline position and length
		const float slope = max_scale / timeline_length_;
		const float val = slope * timeline_position_;

		sphere_->SetActorScale3D(FVector(val));
	}
}

void UTraverseComponent::InitializeShaders()
{
	auto* pc = traverse_shader_.parameter_collection_;
	if (!pc)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get traverse shader parameter collection!"));
		return;
	}

	auto* ci = GetWorld()->GetParameterCollectionInstance(pc);
	traverse_shader_.collection_instance_ = ci;

	// Change default alpha values of past shaders here so we can see whats going on when not playing the game
	ci->SetScalarParameterValue(FName("Alpha 1"), 1.0f);
	ci->SetScalarParameterValue(FName("Alpha 2"), 0.0f);

	ci->SetScalarParameterValue(FName("ToggleOpacity"), 0.0f);
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
	// Used to keep track on which array is empty
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
				{
					// Skip directional light as we fade those with blueprints
					if (!light->IsA(ADirectionalLight::StaticClass()))
						light->ToggleEnabled();
				}

				// Toggle fog
				else if (actor->IsA(AExponentialHeightFog::StaticClass()))
					actor->SetActorHiddenInGame(!actor->bHidden);

				// If it's a sky sphere, flip hidden, this is ugly
				else if (actor->GetName().Compare("BP_Sky_Sphere_Past") == 0 || actor->GetName().Compare("BP_Sky_Sphere_Present") == 0)
					actor->SetActorHiddenInGame(!actor->bHidden);

				else if (a.Key == LevelID::OBJECT)
				{
					for (auto* component : actor->GetComponents())
					{
						UPrimitiveComponent* primitive_comp = Cast<UPrimitiveComponent>(component);
						if (!primitive_comp)
							continue;

						if(component->IsA(UBoxComponent::StaticClass()))
							continue;

						if(actor->ActorHasTag("Pickupable"))
							primitive_comp->SetSimulatePhysics(!primitive_comp->BodyInstance.bSimulatePhysics);

						// For some reason, I can't just do a simple block ? overlap : block, because the getter for the collisionresponse is stupid, oh well
						switch (dimension_)
						{
						case PAST:
						{
							if (actor->ActorHasTag("Past"))
								primitive_comp->SetCollisionResponseToAllChannels(ECR_Block);
							else if (actor->ActorHasTag("Present"))
							{
								// Do a really expensive check because tagging everything is annoying and is easy to miss
								if (actor->IsA(APickUpActor::StaticClass()) || actor->IsA(AInteractableActorBase::StaticClass())
								    || actor->IsA(AVolumeTriggerActor::StaticClass()) || actor->IsA(UTriggerComponent::StaticClass()) 
									|| actor->IsA(UInventoryComponent::StaticClass()) || actor->IsA(UInteractComponent::StaticClass()))
									primitive_comp->SetCollisionResponseToAllChannels(ECR_Ignore);
								else
									primitive_comp->SetCollisionResponseToAllChannels(ECR_Overlap);
							}

							break;
						}
						case PRESENT:
						{
							if (actor->ActorHasTag("Present"))
								primitive_comp->SetCollisionResponseToAllChannels(ECR_Block);
							else if (actor->ActorHasTag("Past"))
							{
								// And another really expensive check
								if (actor->IsA(APickUpActor::StaticClass()) || actor->IsA(AInteractableActorBase::StaticClass())
									|| actor->IsA(AVolumeTriggerActor::StaticClass()) || actor->IsA(UTriggerComponent::StaticClass())
									|| actor->IsA(UInventoryComponent::StaticClass()) || actor->IsA(UInteractComponent::StaticClass()))
									primitive_comp->SetCollisionResponseToAllChannels(ECR_Ignore);
								else
									primitive_comp->SetCollisionResponseToAllChannels(ECR_Overlap);

							}
							break;
						}
						default:
							break;
						}

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
	{
		UE_LOG(LogTemp, Error, TEXT("Traverse float curve not set!"));
		return;
	}

	FOnTimelineFloat tl_cb;
	FOnTimelineEventStatic tl_end_cb;

	tl_cb.BindUFunction(this, FName("TimelineCB"));
	tl_end_cb.BindUFunction(this, FName{ TEXT("TimelineEndCB") });
	timeline_.AddInterpFloat(curve_, tl_cb);
	timeline_.SetTimelineFinishedFunc(tl_end_cb);

	timeline_length_ = timeline_.GetTimelineLength();
}

void UTraverseComponent::TimelineCB()
{
	if (!curve_)
	{
		UE_LOG(LogTemp, Error, TEXT("Traverse float curve not set!"));
		return;
	}

	timeline_position_ = timeline_.GetPlaybackPosition();
	curve_value_ = curve_->GetFloatValue(timeline_position_);

	if (!traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Distance"), curve_value_))
		UE_LOG(LogTemp, Warning, TEXT("Failed to find distance paramater"));
}

void UTraverseComponent::TimelineEndCB()
{
	if (!traverse_shader_.collection_instance_->SetScalarParameterValue(FName("Distance"), level_length_))
		UE_LOG(LogTemp, Warning, TEXT("Failed to find distance paramater"));

	ChangeActorCollision(true);
	if (sphere_)
	{
		GetWorld()->DestroyActor(sphere_);
		sphere_ = nullptr;
	}
}

