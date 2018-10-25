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
class UInteractComponent;
class UInventoryComponent;
struct FTimerHandle;

UCLASS(config = Game)
class REMNANT_API AFP_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_Character();

	void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* input_component) override;

	virtual void Tick(float deltaTime) override;

	UCameraComponent* GetCameraComponent() const { return camera_component_; }
	UTraverseComponent* GetTraverseComponent() { return traverse_component_; }

protected:
	void MoveForward(float value);
	void MoveRight(float value);
	void CharacterCrouch();
	void CharacterUnCrouch();
	void CharacterCrouchToggle();
	void TraverseDimension();
	void PlaceClock();
	void PickupClock();
	void Interact();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	UCameraComponent* camera_component_;
	UCharacterMovementComponent* movement_component_;
	UPROPERTY(EditDefaultsOnly, Category = "Traverse")
	UTraverseComponent* traverse_component_;
	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	UClockComponent* clock_component_;
	UPROPERTY(EditAnywhere)
	UInteractComponent* interactComponent;

	UInventoryComponent* inventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	float clock_timer_ = 10.0f;
	FTimerHandle clock_timer_handle_;
	UFUNCTION()
	void ClockTimerEndCB() { PickupClock(); }
	
	bool traverse_allowed_ = true;
	UPROPERTY(EditInstanceOnly, Category = "Traverse")
	float traverse_cooldown_ = 5.0f;
	FTimerHandle traverse_timer_handle_;
	UFUNCTION()
	void TraverseTimerEndCB() { traverse_allowed_ = true; }
};
