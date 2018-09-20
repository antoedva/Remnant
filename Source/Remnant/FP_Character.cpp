// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "FP_Character.h"

#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Traverser/TraverseComponent.h"
#include "Traverser/DimensionTrigger.h"
#include "TimeClock/TimeCircle.h"

#include "UObject/ConstructorHelpers.h"

AFP_Character::AFP_Character()
{
	// Set up Capsule
	const float capsule_radius = 55.0f;
	const float capsule_height = 96.0f;
	GetCapsuleComponent()->InitCapsuleSize(capsule_radius, capsule_height);

	movement_component_ = GetCharacterMovement();
	traverse_component_ = CreateDefaultSubobject<UTraverseComponent>(TEXT("TraverseComponent"));
}

void AFP_Character::BeginPlay()
{
	Super::BeginPlay();

}

void AFP_Character::SetupPlayerInputComponent(UInputComponent* input_component)
{
	check(input_component);

	// Common movement
	input_component->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	input_component->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	input_component->BindAction("CrouchHold", IE_Pressed, this, &AFP_Character::CharacterCrouch);
	input_component->BindAction("CrouchHold", IE_Released, this, &AFP_Character::CharacterUnCrouch);
	input_component->BindAction("CrouchToggle", IE_Pressed, this, &AFP_Character::CharacterCrouchToggle);

	input_component->BindAxis("MoveForward", this, &AFP_Character::MoveForward);
	input_component->BindAxis("MoveRight", this, &AFP_Character::MoveRight);
	input_component->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	input_component->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Specific input
	input_component->BindAction("Traverse", IE_Pressed, this, &AFP_Character::TraverseDimension);
	input_component->BindAction("PlaceClock", IE_Pressed, this, &AFP_Character::PlaceClock);
	input_component->BindAction("PickupClock", IE_Pressed, this, &AFP_Character::PickupClock); // this should preferably be regular pickup input 

}

void AFP_Character::MoveForward(float value)
{
	if (value != 0.0f)
		AddMovementInput(GetActorForwardVector(), value);
}

void AFP_Character::MoveRight(float value)
{
	if (value != 0.0f)
		AddMovementInput(GetActorRightVector(), value);
}

void AFP_Character::CharacterCrouch()
{
	if (!movement_component_->IsCrouching())
		movement_component_->Crouch();
}

void AFP_Character::CharacterUnCrouch()
{
	if (movement_component_->IsCrouching())
		movement_component_->UnCrouch();
}

void AFP_Character::CharacterCrouchToggle()
{
	movement_component_->IsCrouching() ? movement_component_->Crouch() : movement_component_->UnCrouch();
}

void AFP_Character::TraverseDimension()
{
	traverse_component_->TraverseDimension();
}

void AFP_Character::PlaceClock()
{
	traverse_component_->SetTraverseAllowed(false);
}

void AFP_Character::PickupClock()
{
	traverse_component_->SetTraverseAllowed(true);
}
