#include "InteractComponent.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "FP_Character.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "TimeClock/ClockComponent.h"

#include "DrawDebugHelpers.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	ignoreActor = Cast<AFP_Character>(GetOwner());
	if (!ignoreActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get owner in InteractComponent."));
	}
}

void UInteractComponent::AttemptInteract()
{
	FHitResult hitResult;
	bool didRaycastHit = DoRaycast(OUT hitResult);

	if (didRaycastHit)
	{
		AInteractableActorBase* interactableActor = Cast<AInteractableActorBase>(hitResult.GetActor());
		if (interactableActor)
		{
			interactableActor->InteractWith();
			return;
		}
	}
}

bool UInteractComponent::DoRaycast(OUT FHitResult& hitResult)
{
	UWorld* world = GetWorld();

	FVector viewpointLocation;
	FRotator viewpointRotation;
	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT viewpointLocation, OUT viewpointRotation);

	const FVector startOfTrace = viewpointLocation;
	const FVector traceEnd = viewpointLocation + viewpointRotation.Vector() * interactRange;
	const FCollisionQueryParams queryParams(TEXT(""), false, ignoreActor);

	DrawDebugLine(GetWorld(), startOfTrace, traceEnd, FColor(0, 255, 0), true, 5.0f, 0.0f, 1.0f);

	if (world->LineTraceSingleByChannel(OUT hitResult, startOfTrace, traceEnd, ECollisionChannel::ECC_GameTraceChannel2, queryParams))
	{
		return true;
	}

	return false;
}