#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerActor.generated.h"

class ATriggerReciever;

UCLASS()
class REMNANT_API ATriggerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ATriggerActor();
	~ATriggerActor();

	// TODO:
	// This is the base class for TriggerActors, create additional classes, an example could be: VolumetricTriggerActor which is triggered when the actorThatTriggers walks into the volume.

protected:

	virtual void BeginPlay() override;

	void TriggerAllRecievers();

	UPROPERTY (Category = "TriggerActor", EditAnywhere)
	TArray<ATriggerReciever*> triggerRecievers; // An array of trigger recievers that'll be called when this actor is triggered.

	UPROPERTY(Category = "TriggerActor", EditAnywhere)
	AActor* actorThatTriggers; // The actor that interacts with the trigger, most likely the player.
};
