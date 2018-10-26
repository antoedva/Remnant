// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "ClockComponent.h"
#include "../FP_Character.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
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

	ToggleObjectsInClock(current_actors_in_clock_);
	ToggleFrozenActors();

	if (!traverse_component_)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get traverse component in ClockComponent!"));
		return false;
	}

	StartShader(traverse_component_->GetTraverseShader());
	timeline_.PlayFromStart();

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

	if (!GetWorld()->DestroyActor(clock_))
		return false;

	clock_ = nullptr;

	// Clear the set of actors
	current_actors_in_clock_.Empty();
	actors_to_freeze_.Empty();

	timeline_.Reverse();
	has_reversed_ = true;

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
}

void UClockComponent::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function)
{
	Super::TickComponent(delta_time, tick_type, this_tick_function);

	timeline_.TickTimeline(delta_time);

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

void UClockComponent::ToggleObjectsInClock(TSet<AActor*> actor_set)
{
	for (auto* actor : actor_set)
	{
		if (!actor)
			return;

		TArray<UActorComponent*, TInlineAllocator<2>> components;
		actor->GetComponents(components);

		for (auto* component : components)
		{
			auto* primitive_component = Cast<UPrimitiveComponent>(component);
			if (!primitive_component)
				continue;

			primitive_component->SetCollisionResponseToAllChannels(primitive_component->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Block ? ECR_Overlap : ECR_Block);
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

bool UClockComponent::GetOverlappingActors(TSet<AActor*>& out_actors, TSubclassOf<AActor> filter) const
{
	TSet<UActorComponent*> clock_components = clock_->GetComponents();
	for (auto* component : clock_components)
	{
		if (!component)
			continue;

		if (component->ComponentHasTag("Collision"))
		{
			auto* collision = Cast<USphereComponent>(component);
			collision->GetOverlappingActors(out_actors, filter);
			return true;
		}
	}

	return false;
}

void UClockComponent::ToggleFrozenActors()
{
	for (auto* actor : actors_to_freeze_)
	{
		if (!actor)
			continue;

		auto* trigger_actor = Cast<ATriggerReceiverActor>(actor);

		if (!trigger_actor)
			continue;

		// Trigger channel 10 or 11 based on whether whether we want to freeze or unfreeze it
		trigger_actor->TriggerThisReceiver(static_cast<int>(
			trigger_actor->GetIsFrozen() ? ETriggerBroadcastChannel::CHANNEL_ELEVEN : ETriggerBroadcastChannel::CHANNEL_TEN));
		trigger_actor->SetFrozen(!trigger_actor->GetIsFrozen());

		ToggleObjectsInClock(actors_to_freeze_);
	}
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
}

void UClockComponent::TimelineEndCB()
{
	if (has_reversed_)
	{
		StopShader(traverse_component_->GetTraverseShader());
		has_reversed_ = false;
	}
}

