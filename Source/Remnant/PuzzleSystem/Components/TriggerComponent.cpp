#include "TriggerComponent.h"

#include "PuzzleSystem/Actors/TriggerReceiverActor.h"

UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTriggerComponent::TriggerAllRecievers()
{
	for (ATriggerReceiverActor* receiver : triggerReceivers)
	{
		if (!receiver)
		{
			continue;
		}

		const bool result = receiver->TriggerThisReceiver(static_cast<int>(broadcastChannel));
		if (!result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to trigger this receiver in VolumeTriggerActor."));
		}
	}
}