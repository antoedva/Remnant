#include "TriggerReceiver.h"

#include "PuzzleSystem/ReceiverBlueprintLink.h"
#include "ReceiverTargets.h"

ATriggerReceiver::ATriggerReceiver()
{
	blueprintLink = CreateDefaultSubobject<UReceiverBlueprintLink>("BlueprintLink");
	targets_ = CreateDefaultSubobject<UReceiverTargets>("Receiver target");
}

ATriggerReceiver::~ATriggerReceiver()
{

}

void ATriggerReceiver::BeginPlay()
{
	Super::BeginPlay();
	blueprintLink->RegisterComponent();
}

bool ATriggerReceiver::TriggerThisReceiver()
{
	if (blueprintLink)
	{
		blueprintLink->BroadcastToBlueprint();
		return true;
	}

	return false;
}