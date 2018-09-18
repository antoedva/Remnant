#include "TriggerReciever.h"

ATriggerReciever::ATriggerReciever()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATriggerReciever::TriggerBlueprint()
{
	onTrigger.Broadcast();
}

void ATriggerReciever::BeginPlay()
{
	Super::BeginPlay();
}