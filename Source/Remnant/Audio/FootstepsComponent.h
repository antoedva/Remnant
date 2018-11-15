#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootstepsComponent.generated.h"

class UCharacterMovementComponent;
class USoundBase;
class UWorld;
class AActor;
class AFP_Character;

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
	float velocityRequiredToConsideredMoving = 200.0f;

	UPROPERTY(EditAnywhere)
	float timeBetweenFootsteps = 0.605f;

	UPROPERTY(EditAnywhere)
	float timeToFirstFootstepAfterStopping = 0.3f;

	bool firstStepAfterStopping;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float footstepsVolume = 0.25f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float pitchMultiplierMin = 0.9825f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float pitchMultiplierMax = 1.0175f;

	bool isNextFootLeft;
	float elapsedSinceFootstep;

	UWorld* world;
	AFP_Character* playerActor;
};