// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

#define OUT

class AFP_Character;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UInteractComponent();

	void AttemptInteract();

protected:
	
	virtual void BeginPlay() override;

private:

	bool DoRaycast(FHitResult& hitResult);

	AFP_Character* ignoreActor;

	UPROPERTY(EditAnywhere)
	float interactRange;

};