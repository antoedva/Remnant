#include "VolumeTriggerActor.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
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

void AVolumeTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	if (!actorThatTriggers)
	{
		actorThatTriggers = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
}

void AVolumeTriggerActor::TriggerAllRecievers()
{
	for (ATriggerReceiver* receiver : triggerReceivers)
	{
		if (receiver)
		{
			bool result;

			if (broadcastChannel == EBroadcastChannel::CHANNEL_ONE)
			{
				result = receiver->TriggerThisReceiver(static_cast<int>(EBroadcastChannel::CHANNEL_ONE));
				UE_LOG(LogTemp, Warning, TEXT("1"));
			}
			else if (broadcastChannel == EBroadcastChannel::CHANNEL_TWO)
			{
				result = receiver->TriggerThisReceiver(static_cast<int>(EBroadcastChannel::CHANNEL_TWO));
			}
			else if (broadcastChannel == EBroadcastChannel::CHANNEL_THREE)
			{
				result = receiver->TriggerThisReceiver(static_cast<int>(EBroadcastChannel::CHANNEL_THREE));
			}

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