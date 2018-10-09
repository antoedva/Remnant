#include "InteractableActorBase.h"

#include "Components/SphereComponent.h"

AInteractableActorBase::AInteractableActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	sphereComponent = CreateDefaultSubobject<USphereComponent>("RootComponent (sphere)");
	RootComponent = sphereComponent;
}

void AInteractableActorBase::BeginPlay()
{
	Super::BeginPlay();
}