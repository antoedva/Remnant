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

	UPROPERTY(EditAnywhere)
	float timeToFirstFootstepAfterStopping;

	bool firstStepAfterStopping;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float footstepsVolume;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float pitchMultiplierMin;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float pitchMultiplierMax;

	bool isNextFootLeft;
	float elapsedSinceFootstep;

	UWorld* world;
};