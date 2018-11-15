#include "InteractComponent.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "FP_Character.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "PuzzleSystem/Components/InventoryComponent.h"
#include "UI/InGameUI.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/StaticMeshComponent.h"
#include "FPPlayerController.h"
#include "PuzzleSystem/Actors/PickUpActor.h"
#include "PuzzleSystem/Actors/TriggerReceiverActor.h"
#include "PuzzleSystem/Actors/Triggers/LockedInteractableTriggerActor.h"

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
}

void UInteractComponent::TickingRaycast()
{
	FHitResult hitResult;
	bool didRaycastHit = DoRaycast(OUT hitResult);

	if (didRaycastHit)
	{
		if (hitResult.GetActor()->IsA(ATriggerReceiverActor::StaticClass()))
			return;

		if (!currentHitActor)
		{
			currentHitActor = hitResult.GetActor();

			auto* lita = Cast<ALockedInteractableTriggerActor>(currentHitActor);
			if (lita)
			{
				if (Cast<AFP_Character>(GetOwner())->GetInventoryComponent()->HasItem(lita->GetRequiredItem()))
					lita->on_look_at_.Broadcast();
			}

			UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
			if (ui)
			{
				if (ui->crosshairImage)
				{
					ui->crosshairImage->SetOpacity(1.0f);
				}

				if (ui->pickupText)
				{
					if (currentHitActor->ActorHasTag("Tooltip"))
					{
						const FText text = FText::FromString(FString("Press E to interact"));
						ui->pickupText->SetText(text);
					}

					APickUpActor* pickupActor = Cast<APickUpActor>(currentHitActor);
					auto* trigger = Cast<AInteractableActorBase>(currentHitActor);

					if (pickupActor)
					{
						const FText pickupText = FText::FromString(pickupActor->GetName());
						ui->pickupText->SetText(pickupText);

						ToggleHighlight(pickupActor);
					}
					else if (trigger)
						ToggleHighlight(trigger);
				}
			}
		}
	}
	else
	{
		if (currentHitActor)
		{
			APickUpActor* pickupActor = Cast<APickUpActor>(currentHitActor);
			auto* trigger = Cast<AInteractableActorBase>(currentHitActor);

			auto* lita = Cast<ALockedInteractableTriggerActor>(currentHitActor);
			if (lita)
			{
				if (Cast<AFP_Character>(GetOwner())->GetInventoryComponent()->HasItem(lita->GetRequiredItem()))
					lita->on_look_away_.Broadcast();
			}

			UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
			if (ui)
			{
				if (ui->crosshairImage)
				{
					ui->crosshairImage->SetOpacity(0.4f);
				}

				if (ui->pickupText)
				{
					// Don't change the text to "" if we recently tried to use an unusable item
					if (ui->pickupText->GetText().CompareTo(FText::FromString("Missing required item")) != 0)
					{
						FText pickupText = FText::FromString("");
						ui->pickupText->SetText(pickupText);
					}

					if (pickupActor)
						ToggleHighlight(pickupActor);
					else if (trigger)
						ToggleHighlight(trigger);
				}

				currentHitActor = nullptr;
			}
		}
	}
}

bool UInteractComponent::AttemptInteract()
{
	if (!currentHitActor)
	{
		return false;
	}

	AInteractableActorBase* interactableActor = Cast<AInteractableActorBase>(currentHitActor);
	if (interactableActor)
	{
		if (cachedInventoryComponent)
		{
			auto* ch = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI->crosshairImage;
			if (ch)
				ch->SetOpacity(0.4f);
			ToggleHighlight(currentHitActor);

			currentHitActor = nullptr;
			interactableActor->InteractWith(cachedInventoryComponent);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("variable cachedInventoryComponent in InteractComponent is nullptr!"));
		}
	}
	return false;
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

void UInteractComponent::ToggleHighlight(AActor* actor)
{
	for (auto* comp : actor->GetComponents())
	{
		auto* sm = Cast<UStaticMeshComponent>(comp);
		if (!sm)
			continue;
		sm->SetRenderCustomDepth(!sm->bRenderCustomDepth);
	}
}
