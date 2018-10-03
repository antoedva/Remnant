#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/InteractableActorBase.h"
#include "PickUpActor.generated.h"

UCLASS()
class REMNANT_API APickUpActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:

	APickUpActor();

	FString GetName() { return itemName; }
	void InteractWith() override;

private:

	UPROPERTY(EditAnywhere)
	FString itemName;
};