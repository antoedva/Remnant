#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/InteractableActorBase.h"
#include "InteractableTriggerActor.generated.h"

class ATriggerReceiver;

UCLASS()
class REMNANT_API AInteractableTriggerActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:

	AInteractableTriggerActor();

	void InteractWith() override;

protected:

	virtual void BeginPlay() override;

private:

	void TriggerAllRecievers();

	UPROPERTY(Category = "Interactable Trigger Actor", EditAnywhere)
	TArray<ATriggerReceiver*> triggerReceivers;

};