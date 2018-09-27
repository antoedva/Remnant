#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableTriggerActor.generated.h"

class ATriggerReceiver;

UCLASS()
class REMNANT_API AInteractableTriggerActor : public AActor
{
	GENERATED_BODY()

public:

	AInteractableTriggerActor();

	void InteractWith();

protected:

	virtual void BeginPlay() override;

private:

	void TriggerAllRecievers();

	UPROPERTY(Category = "Interactable Trigger Actor", EditAnywhere)
	TArray<ATriggerReceiver*> triggerReceivers;



};