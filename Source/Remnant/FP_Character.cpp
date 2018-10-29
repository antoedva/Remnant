// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "FP_Character.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "PuzzleSystem/Components/InteractComponent.h"
#include "PuzzleSystem/Components/InventoryComponent.h"
#include "Public/TimerManager.h"

#include "Traverser/TraverseComponent.h"
#include "TimeClock/ClockComponent.h"

#include "UObject/ConstructorHelpers.h"

AFP_Character::AFP_Character()
: watchEnabled(false)
, timeSphereEnabled(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup Capsule
	const float capsule_radius = 55.0f;
	const float capsule_height = 96.0f;
	GetCapsuleComponent()->InitCapsuleSize(capsule_radius, capsule_height);

	// Setup camera
	const FVector cam_offset(-39.56, 1.75, 64.0f);
	camera_component_ = CreateDefaultSubobject<UCameraComponent>(TEXT("FP_Camera"));
	camera_component_->SetupAttachment(GetCapsuleComponent());
	camera_component_->RelativeLocation = cam_offset;

	// Setup movement
	movement_component_ = GetCharacterMovement();

	// Setup specific components
	traverse_component_ = CreateDefaultSubobject<UTraverseComponent>(TEXT("TraverseComponent"));
	clock_component_ = CreateDefaultSubobject<UClockComponent>(TEXT("ClockComponent"));
	interactComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	inventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AFP_Character::BeginPlay()
{
	Super::BeginPlay();

}

void AFP_Character::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	interactComponent->TickingRaycast();

	if (actor_to_lift_)
	{
		trace_start_ = camera_component_->GetComponentLocation();
		trace_end_ = trace_start_ + (camera_component_->GetForwardVector() * distance_);
		actor_to_lift_->SetActorLocation(trace_end_);
	}
}

void AFP_Character::SetupPlayerInputComponent(UInputComponent* input_component)
{
	check(input_component);

	// Common movement
	input_component->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	input_component->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	input_component->BindAxis("MoveForward", this, &AFP_Character::MoveForward);
	input_component->BindAxis("MoveRight", this, &AFP_Character::MoveRight);
	input_component->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	input_component->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Specific input
	input_component->BindAction("Traverse", IE_Pressed, this, &AFP_Character::TraverseDimension);
	input_component->BindAction("PlaceClock", IE_Pressed, this, &AFP_Character::PlaceClock);
	input_component->BindAction("Interact", IE_Pressed, this, &AFP_Character::Interact);
	input_component->BindAction("LiftObject", IE_Pressed, this, &AFP_Character::LiftObject);
	input_component->BindAction("ReleaseObject", IE_Released, this, &AFP_Character::ReleaseObject);
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

void AFP_Character::TraverseDimension()
{
	if (!developmentMode && !watchEnabled)
	{
		return;
	}

	if (!traverse_allowed_)
		return;

	traverse_component_->TraverseDimension();

	auto& tm = GetWorld()->GetTimerManager();
	tm.SetTimer(traverse_timer_handle_, this, &AFP_Character::TraverseTimerEndCB, traverse_cooldown_, false);
	traverse_allowed_ = false;
}

void AFP_Character::PlaceClock()
{
	if (!developmentMode && !timeSphereEnabled)
	{
		return;
	}

	// Do this here as well to not mess up the shader
	if (!traverse_allowed_)
		return;

	if (!clock_component_->ThrowClock())
		return;

	auto& tm = GetWorld()->GetTimerManager();
	if (tm.IsTimerActive(clock_timer_handle_))
		return;

	tm.SetTimer(clock_timer_handle_, this, &AFP_Character::ClockTimerEndCB, clock_timer_, false);
	traverse_allowed_ = false;
}

void AFP_Character::PickupClock()
{
	if (!developmentMode && !timeSphereEnabled)
	{
		return;
	}

	auto& tm = GetWorld()->GetTimerManager();
	if (!clock_timer_handle_.IsValid())
		return;

	clock_component_->PickUpClock(true);
	GetWorld()->GetTimerManager().ClearTimer(clock_timer_handle_);
	traverse_allowed_ = true;
}

void AFP_Character::Interact()
{
	// Return if the interact was successful
	if (interactComponent->AttemptInteract())
		return;

	PickupClock();
}

void AFP_Character::LiftObject()
{
	trace_start_ = camera_component_->GetComponentLocation();
	trace_end_ = trace_start_ + (camera_component_->GetForwardVector() * distance_);

	const FCollisionQueryParams query_params(TEXT(""), true, this);

	FHitResult result;
	GetWorld()->LineTraceSingleByChannel(result, trace_start_, trace_end_, ECC_GameTraceChannel1, query_params);

	if (!result.GetActor())
		return;
	if (!result.GetActor()->ActorHasTag("Pickupable"))
		return;

	actor_to_lift_ = result.GetActor();
	
	for (auto* component : actor_to_lift_->GetComponents())
	{
		auto* prim = Cast<UPrimitiveComponent>(component);
		if(!prim)
			continue;

		prim->SetSimulatePhysics(false);
	}
}

void AFP_Character::ReleaseObject()
{
	if (!actor_to_lift_)
		return;

	for (auto* component : actor_to_lift_->GetComponents())
	{
		auto* prim = Cast<UPrimitiveComponent>(component);
		if (!prim)
			continue;

		prim->SetSimulatePhysics(true);
	}
	actor_to_lift_ = nullptr;
}

void AFP_Character::EnableWatch()
{
	watchEnabled = true;
}

void AFP_Character::EnableTimeSphere()
{
	timeSphereEnabled = true;
}