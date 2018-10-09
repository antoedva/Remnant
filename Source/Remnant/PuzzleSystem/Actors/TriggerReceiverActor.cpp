#include "TriggerReceiverActor.h"

#include "PuzzleSystem/Components/BlueprintLinkComponent.h"

ATriggerReceiverActor::ATriggerReceiverActor()
{
	blueprintLink = CreateDefaultSubobject<UBlueprintLinkComponent>("BlueprintLink");
}

ATriggerReceiverActor::~ATriggerReceiverActor()
{

}

void ATriggerReceiverActor::BeginPlay()
{
	Super::BeginPlay();
	blueprintLink->RegisterComponent();
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