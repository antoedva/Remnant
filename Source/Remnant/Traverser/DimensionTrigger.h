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
	virtual void BeginPlay() override;

public:
	ADimensionTrigger();

	// It's important that the linked trigger is the exact same size as this trigger
	UPROPERTY(EditAnywhere, Category = "Triggers")
	ATriggerBox* linked_trigger_;
	
	UFUNCTION()
	void OnOverlapBegin(AActor* overlapped_actor, AActor* other_actor);

	UFUNCTION()
	void OnOverlapEnd(AActor* overlapped_actor, AActor* other_actor);
};
