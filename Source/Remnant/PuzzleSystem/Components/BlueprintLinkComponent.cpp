#include "BlueprintLinkComponent.h"
#include "../TriggerBroadcastChannel.h"

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

	for (unsigned i = 0; i < (int)ETriggerBroadcastChannel::ALL; ++i)
		on_trigger_.Broadcast(i);
}