#include "TriggerReceiver.h"

#include "ActorComponents/ReceiverBlueprintLink.h"

ATriggerReceiver::ATriggerReceiver()
{
	blueprintLink = CreateDefaultSubobject<UReceiverBlueprintLink>("BlueprintLink");
	blueprintLink->RegisterComponent();
}

ATriggerReceiver::~ATriggerReceiver()
{

}

void ATriggerReceiver::BeginPlay()
{
	Super::BeginPlay();
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