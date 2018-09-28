#include "InteractableTriggerActor.h"

#include "PuzzleSystem/TriggerReceiver.h"

AInteractableTriggerActor::AInteractableTriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AInteractableTriggerActor::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableTriggerActor::InteractWith()
{
	TriggerAllRecievers();
}

void AInteractableTriggerActor::TriggerAllRecievers()
{
	for (ATriggerReceiver* receiver : triggerReceivers)
	{
		receiver->TriggerThisReceiver();
	}
}