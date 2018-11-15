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
, playerActor(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

	isNextFootLeft = true;
}

void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();
	playerActor = characterMovement->GetOwner();
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
	}
}

bool UFootstepsComponent::IsPlayerMoving()
{
	if (characterMovement == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("characterMovement pointer in FootstepsComponent is nullptr"));
		return false;
	}

	return (characterMovement->Velocity.Size() >= velocityRequiredToConsideredMoving);
}

bool UFootstepsComponent::IsPlayerGrounded()
{
	if (characterMovement == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("characterMovement pointer in FootstepsComponent is nullptr"));
		return false;
	}

	return !characterMovement->IsFalling();
}

void UFootstepsComponent::HandleFootsteps()
{
	if (elapsedSinceFootstep >= timeBetweenFootsteps)
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
	}
}

void UFootstepsComponent::PlayFootstepSound(USoundBase* footstepToPlay)
{
	if (footRightSound == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("footRightSound pointer in FootstepsComponent is nullptr"));
		return;
	}

	if (world == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("world pointer in FootstepsComponent is nullptr"));
		return;
	}

	if (playerActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("playerActor pointer in FootstepsComponent is nullptr"));
		return;
	}

	FVector playerLocation = playerActor->GetTransform().GetLocation();
	UGameplayStatics::PlaySoundAtLocation(world, footstepToPlay, playerLocation);
}