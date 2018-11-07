#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"

#include "Styling/SlateBrush.h"

#include "PickUpActor.generated.h"

class UInventoryComponent;
class UInGameUI;

UCLASS()
class REMNANT_API APickUpActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:

	APickUpActor();

	UFUNCTION()
	FString& GetName() { return itemName; }
	void InteractWith(UInventoryComponent* inventory) override;
	FSlateBrush GetBrush() const { return brush; }

protected:

	void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	FString itemName;

	UPROPERTY(EditAnywhere)
	FSlateBrush brush;

};