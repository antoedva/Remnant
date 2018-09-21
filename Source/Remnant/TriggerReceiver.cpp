#include "TriggerReceiver.h"

ATriggerReceiver::ATriggerReceiver()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATriggerReceiver::TriggerBlueprint()
{
	onTrigger.Broadcast();
}

void ATriggerReceiver::BeginPlay()
{
	Super::BeginPlay();
}