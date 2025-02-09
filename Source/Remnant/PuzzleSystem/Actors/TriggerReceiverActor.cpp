#include "TriggerReceiverActor.h"

#include "PuzzleSystem/Components/BlueprintLinkComponent.h"
#include "../ReceiverTargets.h"

ATriggerReceiverActor::ATriggerReceiverActor()
{
	blueprintLink = CreateDefaultSubobject<UBlueprintLinkComponent>("BlueprintLink");
	targets_ = CreateDefaultSubobject<UReceiverTargets>("Receiver Targets");
}

ATriggerReceiverActor::~ATriggerReceiverActor()
{

}

void ATriggerReceiverActor::BeginPlay()
{
	Super::BeginPlay();
}

bool ATriggerReceiverActor::TriggerThisReceiver(int channel)
{
	if (blueprintLink)
	{
		blueprintLink->BroadcastToBlueprint(channel);
		return true;
	}

	return false;
}

bool ATriggerReceiverActor::TriggerThisReceiverReverse(int channel)
{
	if (blueprintLink)
	{
		blueprintLink->BroadcastToBlueprintReverse(channel);
		return true;
	}

	return false;
}

bool ATriggerReceiverActor::TriggerAnotherReceiver(int channel, ATriggerReceiverActor* other_receiver)
{
	if (other_receiver->blueprintLink)
	{
		other_receiver->blueprintLink->BroadcastToReceiver(other_receiver, channel);
		return true;
	}
	return false;
}
