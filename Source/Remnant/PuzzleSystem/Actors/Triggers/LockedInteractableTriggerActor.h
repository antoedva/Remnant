#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "LockedInteractableTriggerActor.generated.h"

class UTriggerComponent;
class UInventoryComponent;

UCLASS()
class REMNANT_API ALockedInteractableTriggerActor : public AInteractableActorBase
{
	GENERATED_BODY()
	
public:

	ALockedInteractableTriggerActor();

	void InteractWith(UInventoryComponent* inventory) override;

protected:

	void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UTriggerComponent* triggerComponent;

	UPROPERTY(EditAnywhere)
	FString requiredItemToTrigger;

};
