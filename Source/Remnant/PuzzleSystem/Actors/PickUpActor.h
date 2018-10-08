#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"
#include "PickUpActor.generated.h"

class UInventoryComponent;
class UGameInstance;

UCLASS()
class REMNANT_API APickUpActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:

	APickUpActor();

	FString GetName() { return itemName; }
	void InteractWith(UInventoryComponent* inventory) override;

protected:

	void BeginPlay() override;

private:

	//UPROPERTY(EditAnywhere)
	//UClass* gameInstance;

	//UGameInstance* gameInstanceActual;

	UPROPERTY(EditAnywhere)
	FString itemName;
};