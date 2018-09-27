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
#include "Kismet/GameplayStatics.h"
#include "Traverser/TraverseComponent.h"
#include "UObject/ConstructorHelpers.h"

UClockComponent::UClockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UClockComponent::ThrowClock()
{
	if (!clock_bp_ || !base_item_ || clock_)
		return false;

	FVector location(0.0f);
	if (!GetSpawnLocation(location))
		return false;

	clock_ = GetWorld()->SpawnActor<AActor>(clock_bp_, location, FRotator(0.0f));

	// Save our overlapped actors so we know what to remove later on
	current_actors_in_clock_ = GetOverlappingActors();
	RenderObjectsInClock();

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

	StopRenderingObjectsInClock();

	if (!GetWorld()->DestroyActor(clock_))
		return false;

	clock_ = nullptr;

	// Clear the set of actors
	current_actors_in_clock_.Empty();

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

void UClockComponent::RenderObjectsInClock()
{
	for (auto* actor : current_actors_in_clock_)
	{
		if (!actor)
			return;

		actor->SetActorHiddenInGame(false);

		TArray<UActorComponent*, TInlineAllocator<2>> components;
		actor->GetComponents(components);

		for (auto* component : components)
		{
			auto* primitive_component = Cast<UPrimitiveComponent>(component);
			if (!primitive_component)
				continue;

			primitive_component->SetCollisionResponseToAllChannels(ECR_Block);
		}
	}
}

void UClockComponent::StopRenderingObjectsInClock()
{
	for (auto* actor : current_actors_in_clock_)
	{
		if (!actor)
			return;

		actor->SetActorHiddenInGame(true);

		TArray<UActorComponent*, TInlineAllocator<2>> components;
		actor->GetComponents(components);

		for (auto* component : components)
		{
			auto* primitive_component = Cast<UPrimitiveComponent>(component);
			if (!primitive_component)
				continue;

			primitive_component->SetCollisionResponseToAllChannels(ECR_Overlap);
		}
	}
}

TSet<AActor*> UClockComponent::GetOverlappingActors() const
{
	TSet<AActor*> overlapping_actors;

	TSet<UActorComponent*> clock_components = clock_->GetComponents();
	for (auto* component : clock_components)
	{
		if (!component)
			continue;

		if (component->ComponentHasTag("Collision"))
		{
			auto* collision = Cast<USphereComponent>(component);
			collision->GetOverlappingActors(overlapping_actors, base_item_);
			break;
		}
	}
	
	// Get all actors that are already visible
	TSet<AActor*> actors_to_remove;
	for (auto* actor : overlapping_actors)
		if (!actor->bHidden)
			actors_to_remove.Add(actor);

	// Remove those actors from the actor set
	for (auto* actor : actors_to_remove)
		overlapping_actors.Remove(actor);

	// Clear scoped sets
	actors_to_remove.Empty();
	clock_components.Empty();

	return overlapping_actors;
}

