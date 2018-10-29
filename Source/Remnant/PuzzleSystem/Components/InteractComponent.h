// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

#define OUT

class AFP_Character;
class UInventoryComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UInteractComponent();

	void TickingRaycast();
	bool AttemptInteract();

protected:
	
	virtual void BeginPlay() override;

private:

	bool DoRaycast(FHitResult& hitResult);

	AFP_Character* ignoreActor;
	AActor* currentHitActor;

	UPROPERTY(EditAnywhere)
	float interactRange;

	UInventoryComponent* cachedInventoryComponent;

};