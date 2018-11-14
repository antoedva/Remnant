#pragma once

#include "CoreMinimal.h"
#include "PuzzleSystem/Actors/InteractableActorBase.h"

#include "Styling/SlateBrush.h"

#include "PickUpActor.generated.h"

class UInventoryComponent;
class UInGameUI;
class UAudioComponent;
class USoundBase;

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

	UAudioComponent* audioComponent;

	UPROPERTY(EditAnywhere)
	FString itemName;

	UPROPERTY(EditAnywhere)
	FSlateBrush brush;

	UPROPERTY(EditAnywhere)
	USoundBase* soundClip;

};