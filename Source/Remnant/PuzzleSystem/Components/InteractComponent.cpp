#include "InteractComponent.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "FP_Character.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "PuzzleSystem/Components/InventoryComponent.h"
#include "UI/InGameUI.h"
#include "instances/RemnantGameInstance.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	currentHitActor = nullptr;
	ignoreActor = Cast<AFP_Character>(GetOwner());
	if (!ignoreActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get owner in InteractComponent."));
	}

	cachedInventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>();
	cachedInGameUI = Cast<URemnantGameInstance>(GetOwner()->GetGameInstance())->InGameUI;

	if (cachedInGameUI)
	{
		cachedInGameUI->SetCrosshairBrush(crosshairBrush);
	}
}

void UInteractComponent::TickingRaycast()
{
	FHitResult hitResult;
	bool didRaycastHit = DoRaycast(OUT hitResult);

	if (didRaycastHit)
	{
		if (!currentHitActor)
		{
			currentHitActor = hitResult.GetActor();
			/*cachedInGameUI->SetCrosshairOpacity(1.0f);*/
		}
	}
	else
	{
		if (currentHitActor)
		{
			currentHitActor = nullptr;
			/*cachedInGameUI->SetCrosshairOpacity(0.25f);*/
		}
	}
}

void UInteractComponent::AttemptInteract()
{
	if (!currentHitActor)
	{
		return;
	}

	AInteractableActorBase* interactableActor = Cast<AInteractableActorBase>(currentHitActor);
	if (interactableActor)
	{
		if (cachedInventoryComponent)
		{
			interactableActor->InteractWith(cachedInventoryComponent);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("variable cachedInventoryComponent in InteractComponent is nullptr!"));
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

	if (world->LineTraceSingleByChannel(OUT hitResult, startOfTrace, traceEnd, ECollisionChannel::ECC_GameTraceChannel2, queryParams))
	{
		return true;
	}

	return false;
}