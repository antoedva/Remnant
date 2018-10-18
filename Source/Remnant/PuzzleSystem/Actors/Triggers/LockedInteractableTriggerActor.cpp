#include "LockedInteractableTriggerActor.h"

#include "PuzzleSystem/Components/InventoryComponent.h"
#include "PuzzleSystem/Components/TriggerComponent.h"

ALockedInteractableTriggerActor::ALockedInteractableTriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	triggerComponent = CreateDefaultSubobject<UTriggerComponent>("Trigger Component");
}

void ALockedInteractableTriggerActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALockedInteractableTriggerActor::InteractWith(UInventoryComponent* inventory)
{
	if (inventory->HasItem(requiredItemToTrigger))
	{
		triggerComponent->TriggerAllReceivers();
	}
	else
	{
		// TO DO
		// Send an event - "Missing required item."
		// Could be used for a voiceline, locked item sfx, a ui feedback or something else
		UE_LOG(LogTemp, Warning, TEXT("Missing required item"));
	}
}