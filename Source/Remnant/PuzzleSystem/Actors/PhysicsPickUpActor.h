// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "PhysicsPickUpActor.generated.h"


class UInventoryComponent;

/**
 * 
 */
UCLASS()
class REMNANT_API APhysicsPickUpActor : public AInteractableActorBase
{
	GENERATED_BODY()

		void InteractWith(UInventoryComponent* optionalInventory) override;
	
private:

};
