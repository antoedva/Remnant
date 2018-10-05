#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActorBase.generated.h"

class USphereComponent;

UCLASS()
class REMNANT_API AInteractableActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AInteractableActorBase();

	virtual void InteractWith() { UE_LOG(LogTemp, Error, TEXT("You're suppose to override this in subclass, because UE doesn't support pure virtual functions ........................................... three dots are not enough ...........")) }

protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* sphereComponent;
};
