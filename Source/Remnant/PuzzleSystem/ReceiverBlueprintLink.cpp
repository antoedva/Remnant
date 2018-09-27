#include "ReceiverBlueprintLink.h"

UReceiverBlueprintLink::UReceiverBlueprintLink()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UReceiverBlueprintLink::BeginPlay()
{
	Super::BeginPlay();
}

void UReceiverBlueprintLink::BroadcastToBlueprint()
{
	onTrigger.Broadcast();
}