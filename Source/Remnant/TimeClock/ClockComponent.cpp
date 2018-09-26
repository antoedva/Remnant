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

	TSet<AActor*> actors = GetOverlappingActors();
	for (auto* actor : actors)
	{
		if (!actor)
			return false;

		// Show the actors
		UE_LOG(LogTemp, Warning, TEXT("%s"), *actor->GetName());
	}

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

	if (!GetWorld()->DestroyActor(clock_))
		return false;

	clock_ = nullptr;

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
			return overlapping_actors;
		}
	}
	return TSet<AActor*>();
}

