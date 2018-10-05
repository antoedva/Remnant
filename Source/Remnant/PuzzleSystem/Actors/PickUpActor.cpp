#include "PickUpActor.h"

APickUpActor::APickUpActor()
: AInteractableActorBase()
{

}

void APickUpActor::InteractWith()
{
	// Gets added to the players inventory and somehow dissapears, either deallocated, moved, visibility altered or something else, not yet decided.
	UE_LOG(LogTemp, Warning, TEXT("Just added to your inventory."));
}