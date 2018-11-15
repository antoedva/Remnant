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
class UFootstepsComponent;
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
	UFUNCTION(BlueprintCallable)
	UTraverseComponent* GetTraverseComponent() { return traverse_component_; }
	UInventoryComponent* GetInventoryComponent() const { return inventoryComponent; }

	UFUNCTION(BlueprintCallable)
	void EnableWatchPickup();
	UFUNCTION(BlueprintCallable)
	void EnableTimeSpherePickup();

	UPROPERTY(BlueprintReadOnly)
	bool watchEnabled;
	UPROPERTY(BlueprintReadOnly)
	bool timeSphereEnabled;

protected:
	void MoveForward(float value);
	void MoveRight(float value);
	void TraverseDimension();
	void PlaceClock();
	void PickupClock();
	void UseClock();
	void Interact();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* camera_component_;
private:
	UCharacterMovementComponent* movement_component_;
	UPROPERTY(EditDefaultsOnly, Category = "Traverse")
	UTraverseComponent* traverse_component_;
	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	UClockComponent* clock_component_;
	UPROPERTY(EditAnywhere)
	UInteractComponent* interactComponent;

	UInventoryComponent* inventoryComponent;

	UPROPERTY(EditAnywhere)
	UFootstepsComponent* footstepsComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	float clock_timer_ = 10.0f;
	FTimerHandle clock_timer_handle_;
	UFUNCTION()
	void ClockTimerEndCB() { PickupClock(); }
	bool clock_grounded_;
	
	bool traverse_allowed_ = true;
	UPROPERTY(EditDefaultsOnly, Category = "Traverse")
	float traverse_cooldown_ = 2.0f;
	FTimerHandle traverse_timer_handle_;
	UFUNCTION()
	void TraverseTimerEndCB() { traverse_allowed_ = true; }

	// Move this later on, just temporary stuff
protected:
	void LiftObject();
	void ReleaseObject();
private:
	AActor* actor_to_lift_;
	FVector trace_start_;
	FVector trace_end_;
	float distance_ = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Clock")
	bool developmentMode;
};