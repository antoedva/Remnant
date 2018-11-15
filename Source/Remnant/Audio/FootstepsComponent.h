#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootstepsComponent.generated.h"

class UCharacterMovementComponent;
class USoundBase;
class UWorld;
class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UFootstepsComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFootstepsComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetMovementComponent(UCharacterMovementComponent* newCharacterComponent) { characterMovement = newCharacterComponent; }

protected:

	virtual void BeginPlay() override;

private:

	bool IsPlayerMoving();
	bool IsPlayerGrounded();
	void HandleFootsteps();
	void PlayFootstepSound(USoundBase* footstepToPlay);

	UCharacterMovementComponent* characterMovement;

	UPROPERTY(EditAnywhere)
	USoundBase* footLeftSound;

	UPROPERTY(EditAnywhere)
	USoundBase* footRightSound;

	UPROPERTY(EditAnywhere)
	float velocityRequiredToConsideredMoving;

	UPROPERTY(EditAnywhere)
	float timeBetweenFootsteps;

	bool isNextFootLeft;
	float elapsedSinceFootstep;

	UWorld* world;

	AActor* playerActor;
};