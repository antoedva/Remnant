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

void UTriggerComponent::TriggerAllReceivers()
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

	onTrigger.Broadcast();
}

void UTriggerComponent::TriggerAllReceiversReverse()
{
	for (ATriggerReceiverActor* receiver : triggerReceivers)
	{
		if (!receiver)
		{
			continue;
		}

		const bool result = receiver->TriggerThisReceiverReverse(static_cast<int>(broadcastChannel));
		if (!result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to trigger this receiver in VolumeTriggerActor."));
		}
	}

	onTriggerReverse.Broadcast();
}

void UTriggerComponent::TriggerReceiver(ATriggerReceiverActor* receiver)
{
	if (!receiver)
		return;

	const bool result = receiver->TriggerThisReceiver(static_cast<int>(broadcastChannel));
	if (!result)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to trigger this receiver in VolumeTriggerActor."));
		return;
	}

	onTrigger.Broadcast();
}

void UTriggerComponent::ForceTriggerReceiver(ATriggerReceiverActor* receiver, const int channel)
{
	if (!receiver)
		return;

	const bool result = receiver->TriggerThisReceiver(channel);
	if (!result)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to trigger this receiver in VolumeTriggerActor."));
		return;
	}

	onTrigger.Broadcast();
}
