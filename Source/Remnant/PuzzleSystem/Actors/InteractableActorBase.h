#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActorBase.generated.h"

class USphereComponent;
class UInventoryComponent;

UCLASS()
class REMNANT_API AInteractableActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AInteractableActorBase();

	// This function only takes an inventory pointer IN CASE the interact object is a pick up (so it can be added to the inventory).
	// Another solution would be to figure out whether or not you're interacting with a pickup or an plain interactable, but that would cost more performance and overhead than just sending inventory pointer every time.
	// Since you will override this method anyway, you can just choose to use or not to use inventory pointer in sub-class.
	virtual void InteractWith(UInventoryComponent* optionalInventory) { UE_LOG(LogTemp, Error, TEXT("You're suppose to override this in subclass, because UE doesn't support pure virtual functions ... three dots are not enough ...")) }

protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* sphereComponent;
};
