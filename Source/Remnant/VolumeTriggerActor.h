#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "VolumeTriggerActor.generated.h"

class ATriggerReceiver;

UCLASS()
class REMNANT_API AVolumeTriggerActor : public ATriggerBox
{
	GENERATED_BODY()
	
public:

	AVolumeTriggerActor();
	~AVolumeTriggerActor();

	UFUNCTION()
	void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);

private:

	void TriggerAllRecievers();

	UPROPERTY(Category = "Volume Trigger Actor", EditAnywhere)
	TArray<ATriggerReceiver*> triggerReceivers;

	UPROPERTY(Category = "Volume Trigger Actor", EditAnywhere)
	AActor* actorThatTriggers; // Pointer to the actor that has the ability to trigger this volume. Will most likely be the player in most instances.

};
