#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"

#include "InteractableTriggerActor.generated.h"

class UTriggerComponent;
class UInventoryComponent;

UCLASS()
class REMNANT_API AInteractableTriggerActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:

	AInteractableTriggerActor();

	void InteractWith(UInventoryComponent* inventory) override;

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UTriggerComponent* triggerComponent;

};