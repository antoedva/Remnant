#include "TriggerActor.h"

#include "TriggerReciever.h"

ATriggerActor::ATriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

ATriggerActor::~ATriggerActor()
{
	triggerRecievers.Empty();
}

void ATriggerActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATriggerActor::TriggerAllRecievers()
{
	for (ATriggerReciever* reciever : triggerRecievers)
	{
		if (reciever)
		{
			reciever->TriggerBlueprint();
		}
	}
}