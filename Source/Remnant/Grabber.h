// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector GetLineTraceEnd();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPhysicsHandleComponent* PhysicsHandle;
	UInputComponent* InputComponent;
	const FHitResult GetFirstPhysicsBodyInReach();

	void Grab();
	void Release();

	void FindPhysicsHandleComponent();
	void FindInputComponent();
	
	float Reach = 100.f;

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
};
