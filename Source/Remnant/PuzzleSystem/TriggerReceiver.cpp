#include "TriggerReceiver.h"

#include "PuzzleSystem/ReceiverBlueprintLink.h"

ATriggerReceiver::ATriggerReceiver()
{
	blueprintLink = CreateDefaultSubobject<UReceiverBlueprintLink>("BlueprintLink");
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