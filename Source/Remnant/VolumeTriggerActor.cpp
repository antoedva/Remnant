#include "VolumeTriggerActor.h"

#include "TriggerReceiver.h"

AVolumeTriggerActor::AVolumeTriggerActor()
{
	OnActorBeginOverlap.AddDynamic(this, &AVolumeTriggerActor::OnOverlapBegin);
}

AVolumeTriggerActor::~AVolumeTriggerActor()
{

}

void AVolumeTriggerActor::TriggerAllRecievers()
{
	for (ATriggerReceiver* receiver : triggerReceivers)
	{
		if (receiver)
		{
			receiver->TriggerBlueprint();
		}
	}
}

void AVolumeTriggerActor::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (actorThatTriggers)
	{
		if (otherActor && otherActor == actorThatTriggers)
		{
			TriggerAllRecievers();
		}
	}
}