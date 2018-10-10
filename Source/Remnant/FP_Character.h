// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "FP_Character.generated.h"

class UInputComponent;
class UCameraComponent;
class UCharacterMovementComponent;
class UTraverseComponent;

UCLASS(config=Game)
class REMNANT_API AFP_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_Character();

	//FORCEINLINE UCameraComponent* GetCameraComponent() const { return camera_component_; }

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* input_component) override;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Types")
		//UClass* class_;
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera"/*, meta = (AllowPrivateAccess = "true")*/)
		//UCameraComponent* camera_component_;
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

	UCharacterMovementComponent* movement_component_;
	UTraverseComponent* traverse_component_;
	//TSubclassOf<ATimeCircle> time_circle_;
};
