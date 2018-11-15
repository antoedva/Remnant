// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "ClockComponent.h"
#include "../FP_Character.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Traverser/TraverseComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "../PuzzleSystem/TriggerBroadcastChannel.h"
#include "../PuzzleSystem/Actors/TriggerReceiverActor.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

UClockComponent::UClockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UClockComponent::ThrowClock()
{
	// Already exists
	if (clock_)
		return false;

	if (!clock_bp_ || !base_item_)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get clock or base item!"));
		return false;
	}

	if (!GetSpawnLocation(spawn_location_))
		return false;

	clock_ = GetWorld()->SpawnActor<AActor>(clock_bp_, spawn_location_, FRotator(0.0f));

	// Save our overlapped actors so we know what to remove later on
	GetOverlappingActors(current_actors_in_clock_, base_item_);
	GetOverlappingActors(actors_to_freeze_);
	GetOverlappingActors(tagged_actors_, nullptr, FName("Special Snowflake"));

	ToggleObjectsInClock(current_actors_in_clock_);
	ToggleFrozenActors();

	if (!traverse_component_)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get traverse component in ClockComponent!"));
		return false;
	}

	StartShader(traverse_component_->GetTraverseShader());
	timeline_.PlayFromStart();

	on_clock_placed_.Broadcast(spawn_location_);

	return true;
}

bool UClockComponent::PickUpClock(const bool ignore_linetrace)
{
	if (!clock_)
		return false;

	if (!ignore_linetrace)
	{
		FHitResult result;
		if (!LineTrace(result))
			return false;

		if (result.Actor != clock_)
			return false;
	}

	ToggleObjectsInClock(current_actors_in_clock_);
	ToggleFrozenActors();
	ResetTaggedActors();

	// Clear the set of actors
	current_actors_in_clock_.Empty();
	actors_to_freeze_.Empty();
	tagged_actors_.Empty();

	timeline_.Reverse();
	has_reversed_ = true;
	on_clock_picked_up_.Broadcast();

	return true;
}

void UClockComponent::BeginPlay()
{
	Super::BeginPlay();

	player_ = Cast<AFP_Character>(GetOwner());
	if (!player_)
		UE_LOG(LogTemp, Error, TEXT("Failed to get player in ClockComponent!"));

	traverse_component_ = Cast<AFP_Character>(GetOwner())->GetTraverseComponent();
	if (!traverse_component_)
		UE_LOG(LogTemp, Error, TEXT("Failed to get traverse component in ClockComponent!"));

	SetupClock();
	SetupTimeline();

	if (parameter_collection_)
	{
		collection_instance_ = GetWorld()->GetParameterCollectionInstance(parameter_collection_);
		if (!collection_instance_)
			return;

		if (!collection_instance_->GetScalarParameterValue(FName("Outline Width"), outline_width_))
			UE_LOG(LogTemp, Error, TEXT("Failed to get Outline Width in Outline_Parameters!"));
		if (!collection_instance_->GetScalarParameterValue(FName("Exponent"), outline_strength_))
			UE_LOG(LogTemp, Error, TEXT("Failed to get Exponent in Outline_Parameters!"));
	}
}

void UClockComponent::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function)
{
	Super::TickComponent(delta_time, tick_type, this_tick_function);

	timeline_.TickTimeline(delta_time);

	if (clock_)
	{
		if (!player_)
			return;
		if (!(tagged_actors_.Num() > 0))
			return;
		SwapTaggedActors();
	}
}

void UClockComponent::ResetTaggedActors()
{
	// REMINDER: THIS FUNCTION IS UGLY!!
	for (auto* actor : tagged_actors_)
	{
		switch (traverse_component_->GetCurrentDimension())
		{
		case UTraverseComponent::PAST:
		{
			if (actor->ActorHasTag(FName("Past")))
			{
				for (auto* component : actor->GetComponents())
				{
					auto* prim = Cast<UPrimitiveComponent>(component);
					if (!prim)
						continue;

					prim->SetCollisionResponseToAllChannels(ECR_Block);
				}
			}
			else
			{
				for (auto* component : actor->GetComponents())
				{
					auto* prim = Cast<UPrimitiveComponent>(component);
					if (!prim)
						continue;

					prim->SetCollisionResponseToAllChannels(ECR_Overlap);
				}
			}
		}
		case UTraverseComponent::PRESENT:
		{
			if (actor->ActorHasTag(FName("Present")))
			{
				for (auto* component : actor->GetComponents())
				{
					auto* prim = Cast<UPrimitiveComponent>(component);
					if (!prim)
						continue;

					prim->SetCollisionResponseToAllChannels(ECR_Block);
				}
			}
			else
			{
				for (auto* component : actor->GetComponents())
				{
					auto* prim = Cast<UPrimitiveComponent>(component);
					if (!prim)
						continue;

					prim->SetCollisionResponseToAllChannels(ECR_Overlap);
				}
			}
		}
		default:
			break;
		}
	}
}

void UClockComponent::SwapTaggedActors()
{
	switch (traverse_component_->GetCurrentDimension())
	{
	case UTraverseComponent::PAST:
	{
		for (auto* actor : tagged_actors_)
		{
			if (actor->ActorHasTag(FName("Past")))
			{
				if (player_->GetDistanceTo(clock_) <= clock_length_)
					ToggleObjectsInClock(tagged_actors_, true, ECR_Overlap);
				else
					ToggleObjectsInClock(tagged_actors_, true, ECR_Block);
			}
			else
			{
				if (player_->GetDistanceTo(clock_) <= clock_length_)
					ToggleObjectsInClock(tagged_actors_, true, ECR_Block);
				else
					ToggleObjectsInClock(tagged_actors_, true, ECR_Overlap);
			}
		}
	}

	case UTraverseComponent::PRESENT:
	{
		for (auto* actor : tagged_actors_)
		{
			if (actor->ActorHasTag(FName("Present")))
			{
				if (player_->GetDistanceTo(clock_) <= clock_length_)
					ToggleObjectsInClock(tagged_actors_, true, ECR_Overlap);
				else
					ToggleObjectsInClock(tagged_actors_, true, ECR_Block);
			}
			else
			{
				if (player_->GetDistanceTo(clock_) <= clock_length_)
					ToggleObjectsInClock(tagged_actors_, true, ECR_Block);
				else
					ToggleObjectsInClock(tagged_actors_, true, ECR_Overlap);
			}
		}
	}
	default:
		break;
	}
}

void UClockComponent::SetupClock()
{
	clock_ = GetWorld()->SpawnActor<AActor>(clock_bp_, FVector(0.0f), FRotator(0.0f));
	for (auto* component : clock_->GetComponents())
	{
		if (!component)
			continue;

		if (component->ComponentHasTag("Mesh"))
		{
			auto* mesh = Cast<UStaticMeshComponent>(component);
			clock_length_ = mesh->Bounds.SphereRadius;
			break;
		}
	}

	GetWorld()->DestroyActor(clock_);
	clock_ = nullptr;
}

bool UClockComponent::GetSpawnLocation(OUT FVector& location) const
{
	FHitResult result;
	if (!LineTrace(result))
		return false;

	location = result.ImpactPoint;
	return true;
}

bool UClockComponent::LineTrace(OUT FHitResult& result) const
{
	// TODO: Make sure to only trace against floor, with some special cases
	const UCameraComponent* camera = player_->GetCameraComponent();
	const FVector trace_start = camera->GetComponentLocation();
	const FVector trace_end = trace_start + (camera->GetForwardVector() * throw_distance_);

	const FCollisionQueryParams query_params(TEXT(""), true, player_);
	//DrawDebugLine(GetWorld(), trace_start, trace_end, FColor(255, 0, 0), true, 5.0f, 0.0f, 1.0f);

	if (!GetWorld()->LineTraceSingleByChannel(result, trace_start, trace_end, ECC_GameTraceChannel1, query_params))
		return false;

	return true;
}

void UClockComponent::ToggleObjectsInClock(TSet<AActor*> actor_set, bool force, ECollisionResponse response)
{
	for (auto* actor : actor_set)
	{
		if (!actor)
			continue;;
		if (actor->ActorHasTag("Pickupable"))
			continue;

		for (auto* component : actor->GetComponents())
		{
			auto* prim = Cast<UPrimitiveComponent>(component);
			if (!prim)
				continue;

			if (prim->IsA(UBoxComponent::StaticClass()) || prim->IsA(USphereComponent::StaticClass()))
				prim->SetCollisionResponseToAllChannels(prim->GetCollisionResponseToChannel(ECC_WorldDynamic) == ECR_Overlap ? ECR_Ignore : ECR_Overlap);

			else
			{
				if (force)
					prim->SetCollisionResponseToAllChannels(response);
				else
					prim->SetCollisionResponseToAllChannels(prim->GetCollisionResponseToChannel(ECC_WorldDynamic) == ECR_Block ? ECR_Overlap : ECR_Block);
			}

			component->OnActorEnableCollisionChanged();

			break;
		}
	}
}

bool UClockComponent::StartShader(FTraverseShader shader)
{
	auto* ci = shader.collection_instance_;
	if (!ci)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get traverse shaders in ClockComponent!"));
		return false;
	}

	ci->SetVectorParameterValue(FName("Start Position"), spawn_location_);

	if (traverse_component_->GetFirstSkipped())
	{
		float alpha1;
		float alpha2;
		ci->GetScalarParameterValue(FName("Alpha 1"), alpha1);
		ci->GetScalarParameterValue(FName("Alpha 2"), alpha2);

		// Flip between 1 or 0 alpha
		ci->SetScalarParameterValue(FName("Alpha 1"), alpha1 == 0.0f ? 1.0f : 0.0f);
		ci->SetScalarParameterValue(FName("Alpha 2"), alpha2 == 0.0f ? 1.0f : 0.0f);
	}

	ci->GetScalarParameterValue(FName("Distance"), last_distance_);
	collection_instance_->SetScalarParameterValue(FName("Outline Width"), outline_width_ * 0.5f);
	collection_instance_->SetScalarParameterValue(FName("Exponent"), outline_strength_ * 0.7f);

	return true;
}

bool UClockComponent::StopShader(FTraverseShader shader)
{
	auto* ci = shader.collection_instance_;
	if (!ci)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get traverse shaders in ClockComponent!"));
		return false;
	}

	ci->SetScalarParameterValue(FName("Distance"), last_distance_);
	collection_instance_->SetScalarParameterValue(FName("Outline Width"), outline_width_);
	collection_instance_->SetScalarParameterValue(FName("Exponent"), outline_strength_);

	if (traverse_component_->GetFirstSkipped())
	{
		float alpha1;
		float alpha2;
		ci->GetScalarParameterValue(FName("Alpha 1"), alpha1);
		ci->GetScalarParameterValue(FName("Alpha 2"), alpha2);

		// Flip between 1 or 0 alpha
		ci->SetScalarParameterValue(FName("Alpha 1"), alpha1 == 0.0f ? 1.0f : 0.0f);
		ci->SetScalarParameterValue(FName("Alpha 2"), alpha2 == 0.0f ? 1.0f : 0.0f);
	}

	return true;
}

bool UClockComponent::GetOverlappingActors(TSet<AActor*>& out_actors, TSubclassOf<AActor> filter, const FName& optional_tag) const
{
	TSet<UActorComponent*> clock_components = clock_->GetComponents();
	for (auto* component : clock_components)
	{
		if (!component)
			continue;

		if (component->ComponentHasTag("Mesh"))
		{
			auto* mesh = Cast<UStaticMeshComponent>(component);
			mesh->GetOverlappingActors(out_actors, filter);

			// Remove specific actors from array if they exist
			if (out_actors.Contains(GetOwner()))
				out_actors.Remove(GetOwner());
			if (out_actors.Contains(clock_))
				out_actors.Remove(clock_);

			// If an optional tag is set, remove overlapped actors without that tag
			if (optional_tag.Compare(FName("")) != 0)
			{
				TSet<AActor*> actors_to_remove;
				for (auto* actor : out_actors)
				{
					if (!actor->ActorHasTag(optional_tag))
						actors_to_remove.Add(actor);
				}
				for (auto* actor : actors_to_remove)
				{
					if (out_actors.Contains(actor))
						out_actors.Remove(actor);
				}
			}

			return true;
		}
	}

	return false;
}

void UClockComponent::ToggleFrozenActors()
{
	TArray<AActor*> remove;
	for (auto* actor : actors_to_freeze_)
	{
		if (!actor)
			continue;

		auto* receiver = Cast<ATriggerReceiverActor>(actor);

		if (!receiver)
		{
			remove.Add(receiver);
			continue;
		}

		// Trigger channel 10 or 11 based on whether whether we want to freeze or unfreeze it
		receiver->TriggerThisReceiver(static_cast<int>(
			receiver->GetIsFrozen() ? ETriggerBroadcastChannel::CHANNEL_ELEVEN : ETriggerBroadcastChannel::CHANNEL_TEN));
		receiver->TriggerThisReceiverReverse(static_cast<int>(
			receiver->GetIsFrozen() ? ETriggerBroadcastChannel::CHANNEL_ELEVEN : ETriggerBroadcastChannel::CHANNEL_TEN));
		receiver->SetFrozen(!receiver->GetIsFrozen());
	}

	// Remove unwanted actors
	for (auto* actor : remove)
	{
		if (actors_to_freeze_.Contains(actor))
			actors_to_freeze_.Remove(actor);
	}
	remove.Empty();

	ToggleObjectsInClock(actors_to_freeze_);
}

void UClockComponent::SetupTimeline()
{
	curve_ = NewObject<UCurveFloat>();
	timeline_.SetTimelineLengthMode(TL_TimelineLength);
	timeline_.SetTimelineLength(0.5f);
	curve_->FloatCurve.AddKey(0.0f, 0.0f);
	curve_->FloatCurve.AddKey(timeline_.GetTimelineLength(), clock_length_);

	FOnTimelineFloat tl_cb;
	FOnTimelineEventStatic tl_end_cb;

	tl_cb.BindUFunction(this, FName("TimelineCB"));
	tl_end_cb.BindUFunction(this, FName{ TEXT("TimelineEndCB") });
	timeline_.AddInterpFloat(curve_, tl_cb);
	timeline_.SetTimelineFinishedFunc(tl_end_cb);
}

void UClockComponent::TimelineCB()
{
	const float timeline_position = timeline_.GetPlaybackPosition();
	const float curve_value = curve_->GetFloatValue(timeline_position);

	traverse_component_->GetTraverseShader().collection_instance_->SetScalarParameterValue(FName("Distance"), curve_value);

	if (!clock_)
		return;

	for (auto* component : clock_->GetComponents())
	{
		if (!component)
			continue;

		if (component->ComponentHasTag("Mesh"))
		{
			auto* mesh = Cast<UStaticMeshComponent>(component);
			if (!mesh)
				continue;

			// Yes I like magic
			const float magic_number = 160.0f;
			const float max_scale = curve_->GetFloatValue(timeline_.GetTimelineLength()) / magic_number;
			const float slope = max_scale / timeline_.GetTimelineLength();
			const float val = slope * timeline_position;
			mesh->SetRelativeScale3D(FVector(val));

			break;
		}
	}
}

void UClockComponent::TimelineEndCB()
{
	if (has_reversed_)
	{
		GetWorld()->DestroyActor(clock_);
		clock_ = nullptr;

		StopShader(traverse_component_->GetTraverseShader());
		has_reversed_ = false;
	}
}