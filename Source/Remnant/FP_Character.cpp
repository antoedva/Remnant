// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "FP_Character.h"

#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

AFP_Character::AFP_Character()
{
	// Set up Capsule
	const float capsule_radius = 55.0f;
	const float capsule_height = 96.0f;
	GetCapsuleComponent()->InitCapsuleSize(capsule_radius, capsule_height);

	// Set up Camera Component
	const FVector camera_offset(-39.56f, 1.75f, 64.0f);

	camera_component_ = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	camera_component_->SetupAttachment(GetCapsuleComponent());
	camera_component_->RelativeLocation = camera_offset;

	// Set up Movement Component
	movement_component_ = GetCharacterMovement();
}

void AFP_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFP_Character::SetupPlayerInputComponent(UInputComponent* input_component)
{
	check(input_component);

	input_component->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	input_component->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	input_component->BindAction("CrouchHold", IE_Pressed, this, &AFP_Character::CharacterCrouch);
	input_component->BindAction("CrouchHold", IE_Released, this, &AFP_Character::CharacterUnCrouch);
	input_component->BindAction("CrouchToggle", IE_Pressed, this, &AFP_Character::CharacterCrouchToggle);

	input_component->BindAxis("MoveForward", this, &AFP_Character::MoveForward);
	input_component->BindAxis("MoveRight", this, &AFP_Character::MoveRight);
	input_component->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	input_component->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
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

