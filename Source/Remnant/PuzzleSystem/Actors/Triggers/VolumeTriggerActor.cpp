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

	if (!actorThatTriggers)
	{
		actorThatTriggers = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
}

void AVolumeTriggerActor::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (actorThatTriggers)
	{
		if (otherActor && otherActor == actorThatTriggers)
		{
			isActorInsideVolume = true;
			triggerComponent->TriggerAllRecievers();
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