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

		bool result = false;

		if (broadcastChannel == ETriggerBroadcastChannel::CHANNEL_ONE)
		{
			result = receiver->TriggerThisReceiver(static_cast<int>(ETriggerBroadcastChannel::CHANNEL_ONE));
		}
		else if (broadcastChannel == ETriggerBroadcastChannel::CHANNEL_TWO)
		{
			result = receiver->TriggerThisReceiver(static_cast<int>(ETriggerBroadcastChannel::CHANNEL_TWO));
		}
		else if (broadcastChannel == ETriggerBroadcastChannel::CHANNEL_THREE)
		{
			result = receiver->TriggerThisReceiver(static_cast<int>(ETriggerBroadcastChannel::CHANNEL_THREE));
		}

		if (!result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to trigger this receiver in VolumeTriggerActor. Make sure you have a channel set. MAX_CHANNELS is not valid."));
		}
	}
}