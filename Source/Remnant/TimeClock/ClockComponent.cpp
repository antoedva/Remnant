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
	if (!clock_bp_ || !base_item_ || clock_)
		return false;

	if (!GetSpawnLocation(spawn_location_))
		return false;

	clock_ = GetWorld()->SpawnActor<AActor>(clock_bp_, spawn_location_, FRotator(0.0f));

	// Save our overlapped actors so we know what to remove later on
	//current_actors_in_clock_ = GetOverlappingActors();
	GetOverlappingActors(current_actors_in_clock_, base_item_);
	GetOverlappingActors(actors_to_freeze_);

	ToggleObjectsInClock();
	ToggleFrozenActors();

	// TODO: Add traverse shader here
	//auto* traverse = Cast<AFP_Character>(GetOwner())->GetTraverseComponent();
	//if (!traverse)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed to get traverse component in ClockComponent!"));
	//	return false;
	//}

	//StartShader(traverse->present_traverse_shader_);
	//StartShader(traverse->past_traverse_shader_);

	return true;
}

bool UClockComponent::PickUpClock()
{
	if (!clock_)
		return false;

	FHitResult result;
	if (!LineTrace(result))
		return false;

	if (result.Actor != clock_)
		return false;

	ToggleObjectsInClock();
	ToggleFrozenActors();

	if (!GetWorld()->DestroyActor(clock_))
		return false;

	clock_ = nullptr;

	// Clear the set of actors
	current_actors_in_clock_.Empty();
	actors_to_freeze_.Empty();

	//auto* traverse = Cast<AFP_Character>(GetOwner())->GetTraverseComponent();
	//if (!traverse)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed to get traverse component in ClockComponent!"));
	//	return false;
	//}
	//StopShader(traverse->present_traverse_shader_);
	//StopShader(traverse->past_traverse_shader_);

	return true;
}

void UClockComponent::BeginPlay()
{
	Super::BeginPlay();

	player_ = Cast<AFP_Character>(GetOwner());
	if (!player_)
		UE_LOG(LogTemp, Error, TEXT("Failed to get player in ClockComponent!"));
}

void UClockComponent::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function)
{
	Super::TickComponent(delta_time, tick_type, this_tick_function);

}

bool UClockComponent::GetSpawnLocation(OUT FVector& location) const
{
	FHitResult result;
	if (!LineTrace(result))
		return false;

	location = result.ImpactPoint;
	return true;
}

bool UClockComponent::LineTrace(OUT FHitResult & result) const
{
	// TODO: Make sure to only trace against floor, with some special cases
	const float distance = 500.0f;
	const UCameraComponent* camera = player_->GetCameraComponent();
	const FVector trace_start = camera->GetComponentLocation();
	const FVector trace_end = trace_start + (camera->GetForwardVector() * distance);

	const FCollisionQueryParams query_params(TEXT(""), true, player_);
	DrawDebugLine(GetWorld(), trace_start, trace_end, FColor(255, 0, 0), true, 5.0f, 0.0f, 1.0f);

	if (!GetWorld()->LineTraceSingleByChannel(result, trace_start, trace_end, ECC_GameTraceChannel1, query_params))
		return false;

	return true;
}

void UClockComponent::ToggleObjectsInClock()
{
	for (auto* actor : current_actors_in_clock_)
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
	ci->SetScalarParameterValue(FName("Distance"), 250.0f); // This should be over time, and the length of the sphere

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

	ci->SetScalarParameterValue(FName("Distance"), 0.0f); // This should be over time, and the length of the sphere

	return true;
}

bool UClockComponent::GetOverlappingActors(TSet<AActor*>& out_actors, TSubclassOf<AActor> filter) const
{
	//TSet<AActor*> overlapping_actors;

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

	// Keep this if we want to change how the clock works
	// Get all actors that are already visible
	//TSet<AActor*> actors_to_remove;
	//for (auto* actor : overlapping_actors)
	//{
		//if (!actor->bHidden)
			//actors_to_remove.Add(actor);
	//}

	 //Remove those actors from the actor set
	//for (auto* actor : actors_to_remove)
		//overlapping_actors.Remove(actor);

	// Clear scoped sets
	//actors_to_remove.Empty();
	//clock_components.Empty();

	//return overlapping_actors;
	
	return false;
}

void UClockComponent::ToggleFrozenActors()
{
	for (auto* actor : actors_to_freeze_)
	{
		if(!actor)
			continue;

		auto* trigger_actor = Cast<ATriggerReceiverActor>(actor); 

		if(!trigger_actor)
			continue;

		// Trigger channel 10 or 11 based on whether whether we want to freeze or unfreeze it
		trigger_actor->TriggerThisReceiver(static_cast<int>(
			trigger_actor->GetIsFrozen() ? 	ETriggerBroadcastChannel::CHANNEL_ELEVEN : ETriggerBroadcastChannel::CHANNEL_TEN));
		trigger_actor->SetFrozen(!trigger_actor->GetIsFrozen());
	}
}

