#include "ReceiverBlueprintLink.h"

UReceiverBlueprintLink::UReceiverBlueprintLink()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UReceiverBlueprintLink::BeginPlay()
{
	Super::BeginPlay();
}

void UReceiverBlueprintLink::BroadcastToBlueprint(int channel)
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