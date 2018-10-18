#include "InteractableTriggerActor.h"

#include "PuzzleSystem/Components/TriggerComponent.h"
#include "PuzzleSystem/Components/InventoryComponent.h"

AInteractableTriggerActor::AInteractableTriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	triggerComponent = CreateDefaultSubobject<UTriggerComponent>("Trigger Component");
}

void AInteractableTriggerActor::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableTriggerActor::InteractWith(UInventoryComponent* /*inventory*/)
{
	triggerComponent->TriggerAllReceivers();
}