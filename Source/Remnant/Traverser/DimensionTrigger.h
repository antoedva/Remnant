// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "DimensionTrigger.generated.h"

/**
 * 
 */
UCLASS()
class REMNANT_API ADimensionTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;

public:
	ADimensionTrigger();

	FVector GetNewPosition() const;

	// Can be used to override the current actor inside the trigger
	void SetActor(AActor* actor) { actor_ = actor; }

	// It's important that the linked trigger is the exact same size as this trigger
	UPROPERTY(EditAnywhere, Category = "Triggers")
	ATriggerBox* linked_trigger_;

private:
	UFUNCTION()
	void OnOverlapBegin(AActor* overlapped_actor, AActor* other_actor);

	UFUNCTION()
	void OnOverlapEnd(AActor* overlapped_actor, AActor* other_actor);

	AActor* actor_;
};
