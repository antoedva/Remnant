#include "BlueprintLinkComponent.h"
#include "../TriggerBroadcastChannel.h"
#include "../Actors/TriggerReceiverActor.h"

UBlueprintLinkComponent::UBlueprintLinkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBlueprintLinkComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBlueprintLinkComponent::BroadcastToBlueprint(int channel)
{
	if (static_cast<ETriggerBroadcastChannel>(channel) != ETriggerBroadcastChannel::ALL)
	{
		on_trigger_.Broadcast(channel);
		return;
	}

	for (unsigned i = 0; i < static_cast<int>(ETriggerBroadcastChannel::ALL); ++i)
	{
		// Skip freeze channels
		if (i == static_cast<int>(ETriggerBroadcastChannel::CHANNEL_ELEVEN) || i == static_cast<int>(ETriggerBroadcastChannel::CHANNEL_TEN))
			continue;

		on_trigger_.Broadcast(i);
	}
}

void UBlueprintLinkComponent::BroadcastToBlueprintReverse(int channel)
{
	if (static_cast<ETriggerBroadcastChannel>(channel) != ETriggerBroadcastChannel::ALL)
	{
		onTriggerReverse.Broadcast(channel);
		return;
	}

	for (unsigned i = 0; i < static_cast<int>(ETriggerBroadcastChannel::ALL); ++i)
	{
		// Skip freeze channels
		if (i == static_cast<int>(ETriggerBroadcastChannel::CHANNEL_ELEVEN) || i == static_cast<int>(ETriggerBroadcastChannel::CHANNEL_TEN))
			continue;

		onTriggerReverse.Broadcast(i);
	}
}

void UBlueprintLinkComponent::BroadcastToReceiver(ATriggerReceiverActor* receiver, int channel)
{
	on_another_receiver_triggered_.Broadcast(receiver, channel);
}
