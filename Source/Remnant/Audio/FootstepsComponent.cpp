#include "FootstepsComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UFootstepsComponent::UFootstepsComponent()
: footLeftSound(nullptr)
, footRightSound(nullptr)
, characterMovement(nullptr)
, world(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

	isNextFootLeft = true;
}

void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();
	firstStepAfterStopping = true;
}

void UFootstepsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsPlayerMoving() && IsPlayerGrounded())
	{
		elapsedSinceFootstep += DeltaTime;

		HandleFootsteps();
	}
	else
	{
		elapsedSinceFootstep = 0.0f;
		isNextFootLeft = true;
		firstStepAfterStopping = true;
	}
}

bool UFootstepsComponent::IsPlayerMoving()
{
	if (characterMovement == nullptr)
	{
		characterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
		if (characterMovement == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("characterMovement pointer in FootstepsComponent is nullptr"));
			return false;
		}
	}

	const float velocity = characterMovement->Velocity.Size();
	return (velocity >= velocityRequiredToConsideredMoving);
}

bool UFootstepsComponent::IsPlayerGrounded()
{
	if (characterMovement == nullptr)
	{
		characterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
		if (characterMovement == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("characterMovement pointer in FootstepsComponent is nullptr"));
			return false;
		}
	}

	return !characterMovement->IsFalling();
}

void UFootstepsComponent::HandleFootsteps()
{
	float actualTimeBetweenFootsteps;

	if (firstStepAfterStopping)
	{
		actualTimeBetweenFootsteps = timeToFirstFootstepAfterStopping;
	}
	else
	{
		actualTimeBetweenFootsteps = timeBetweenFootsteps;
	}

	if (elapsedSinceFootstep >= actualTimeBetweenFootsteps)
	{
		if (isNextFootLeft)
		{
			PlayFootstepSound(footLeftSound);
		}
		else
		{
			PlayFootstepSound(footRightSound);
		}

		isNextFootLeft = !isNextFootLeft;
		elapsedSinceFootstep = 0.0f;
		firstStepAfterStopping = false;
	}
}

void UFootstepsComponent::PlayFootstepSound(USoundBase* footstepToPlay)
{
	characterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	if (characterMovement == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("characterMovement pointer in FootstepsComponent is nullptr"));
		return;
	}

	if (footstepToPlay == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("footstepToPlay pointer in FootstepsComponent is nullptr, can be either left or right"));
		return;
	}

	if (world == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("world pointer in FootstepsComponent is nullptr"));
		return;
	}

	// I want to cache this below, but Unreal is weird :S
	const FVector playerLocation = characterMovement->GetOwner()->GetTransform().GetLocation();
	const float pitch = FMath::RandRange(pitchMultiplierMin, pitchMultiplierMax);

	UE_LOG(LogTemp, Warning, TEXT("%f"), pitch);

	UGameplayStatics::PlaySoundAtLocation(world, footstepToPlay, playerLocation, footstepsVolume, pitch, 0.0f);
}