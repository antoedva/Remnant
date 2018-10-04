#include "VolumeTriggerActor.h"

#include "PuzzleSystem/TriggerReceiver.h"

AVolumeTriggerActor::AVolumeTriggerActor()
{
	OnActorBeginOverlap.AddDynamic(this, &AVolumeTriggerActor::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AVolumeTriggerActor::OnOverlapEnd);

	isActorInsideVolume = false;
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
			bool result = receiver->TriggerThisReceiver();
			if (!result)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to trigger this receiver in VolumeTriggerActor."));
			}
		}
	}
}

void AVolumeTriggerActor::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (actorThatTriggers)
	{
		if (otherActor && otherActor == actorThatTriggers)
		{
			isActorInsideVolume = true;
			TriggerAllRecievers();
		}
	}
}

void AVolumeTriggerActor::OnOverlapEnd(AActor* overlappedActor, AActor* otherActor)
{
	if (actorThatTriggers)
	{
		if (otherActor && otherActor == actorThatTriggers)
		{
			isActorInsideVolume = false;
		}
	}
}