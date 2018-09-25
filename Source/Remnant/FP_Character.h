// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "FP_Character.generated.h"

class UInputComponent;
class UCameraComponent;
class UCharacterMovementComponent;
class UTraverseComponent;
class UClockComponent;

UCLASS(config = Game)
class REMNANT_API AFP_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_Character();

	void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* input_component) override;

	UCameraComponent* GetCameraComponent() const { return camera_component_; }


protected:
	void MoveForward(float value);
	void MoveRight(float value);
	void CharacterCrouch();
	void CharacterUnCrouch();
	void CharacterCrouchToggle();
	void TraverseDimension();
	void PlaceClock();
	void PickupClock();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	UCameraComponent* camera_component_;
	UCharacterMovementComponent* movement_component_;
	UTraverseComponent* traverse_component_;
	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	UClockComponent* clock_component_;
};
