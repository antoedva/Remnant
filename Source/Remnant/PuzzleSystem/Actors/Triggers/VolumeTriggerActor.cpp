#include "VolumeTriggerActor.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PuzzleSystem/Components/TriggerComponent.h"

AVolumeTriggerActor::AVolumeTriggerActor()
{
	triggerComponent = CreateDefaultSubobject<UTriggerComponent>("Trigger Component");

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

	if (!actorsThatTriggers.Num() == 0)
	{
		actorsThatTriggers.Add(GetWorld()->GetFirstPlayerController()->GetPawn());
	}
}

void AVolumeTriggerActor::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	for (auto* actor : actorsThatTriggers)
	{
		if (actor)
		{
			if (otherActor && otherActor == actor)
			{
				amount_of_actors_inside_++;

				isActorInsideVolume = true;
				triggerComponent->TriggerAllReceivers();
			}
		}
	}
}

void AVolumeTriggerActor::OnOverlapEnd(AActor* overlappedActor, AActor* otherActor)
{
	for (auto* actor : actorsThatTriggers)
	{
		if (actor)
		{
			if (otherActor && otherActor == actor)
			{
				amount_of_actors_inside_--;

				// We don't want to do anything if an object is still inside
				if (amount_of_actors_inside_ > 0)
					return;

				isActorInsideVolume = false;
				triggerComponent->TriggerAllReceiversReverse();
			}
		}
	}
}