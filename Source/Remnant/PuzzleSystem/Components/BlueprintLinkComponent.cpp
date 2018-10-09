#include "BlueprintLinkComponent.h"

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
	if (channel == 1)
	{
		onTriggerOne.Broadcast();
	}
	else if (channel == 2)
	{
		onTriggerTwo.Broadcast();
	}
	else if (channel == 3)
	{
		onTriggerThree.Broadcast();
	}
}