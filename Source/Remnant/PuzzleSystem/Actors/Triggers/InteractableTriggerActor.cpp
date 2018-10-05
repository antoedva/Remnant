#include "InteractableTriggerActor.h"

#include "PuzzleSystem/Components/TriggerComponent.h"

AInteractableTriggerActor::AInteractableTriggerActor()
{
	triggerComponent = CreateDefaultSubobject<UTriggerComponent>("Trigger Component");

	PrimaryActorTick.bCanEverTick = false;
}

void AInteractableTriggerActor::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableTriggerActor::InteractWith()
{
	triggerComponent->TriggerAllRecievers();
}