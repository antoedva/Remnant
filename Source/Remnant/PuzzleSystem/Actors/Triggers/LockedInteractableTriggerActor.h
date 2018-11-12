#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "LockedInteractableTriggerActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLookAt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLookAway);

class UTriggerComponent;
class UInventoryComponent;

UCLASS()
class REMNANT_API ALockedInteractableTriggerActor : public AInteractableActorBase
{
	GENERATED_BODY()
	
public:

	ALockedInteractableTriggerActor();

	void InteractWith(UInventoryComponent* inventory) override;
	
	UPROPERTY(BlueprintAssignable)
	FOnLookAt on_look_at_;
	UPROPERTY(BlueprintAssignable)
	FOnLookAway on_look_away_;

protected:

	void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UTriggerComponent* triggerComponent;

	UPROPERTY(EditAnywhere)
	FString requiredItemToTrigger;

};
