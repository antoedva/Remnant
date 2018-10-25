// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();
	FindInputComponent();
	
}

FVector UGrabber::GetLineTraceEnd()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	//if physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		//move object that we're holding
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}

	//DrawDebugLine(
	//	GetWorld(),
	//	GetOwner()->GetActorLocation(),
	//	GetLineTraceEnd(),
	//	FColor(255, 0, 0),
	//	false, -1, 0, 15.f);
}

void UGrabber::Grab()
{
	GetFirstPhysicsBodyInReach();


	//try reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();

	auto ActorHit = HitResult.GetActor();
	auto ComponentToGrab = ActorHit->GetRootComponent();
	//TODO if we hit something, attach a physics handle
	if (ActorHit)
	{
		UE_LOG(LogTemp, Error, TEXT("Grab attempted"));
		if (!PhysicsHandle) { return; }
		//PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation());
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle Component not found on %s"), *GetOwner()->GetName());
	}
}

void UGrabber::FindInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Interact", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Interact", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InputComponent Component not found on %s"), *GetOwner()->GetName());
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	///Set up query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	///ray-cast out to reach distance, store reach
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByObjectType(OUT LineTraceHit, PlayerViewPointLocation, GetLineTraceEnd(), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);



	///see what we hit
	AActor* ActorHit = LineTraceHit.GetActor();

	return LineTraceHit;
}