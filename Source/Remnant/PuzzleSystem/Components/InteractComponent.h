// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Styling/SlateBrush.h"
#include "InteractComponent.generated.h"

#define OUT

class AFP_Character;
class UInventoryComponent;
class UInGameUI;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UInteractComponent();

	void TickingRaycast();
	void AttemptInteract();

protected:
	
	virtual void BeginPlay() override;

private:

	bool DoRaycast(FHitResult& hitResult);

	AFP_Character* ignoreActor;
	AActor* currentHitActor;

	UPROPERTY(EditAnywhere)
	float interactRange;

	// This right here is super ugly, I use it to set the crosshairs brush, havent found a way to drag it in to the ui class since its not a blueprint, and the image doesnt exist in the widget since its created in c++.
	UPROPERTY(EditAnywhere)
	FSlateBrush crosshairBrush;

	UInventoryComponent* cachedInventoryComponent;
	UInGameUI* cachedInGameUI;

};