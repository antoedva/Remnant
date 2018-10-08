#include "PickUpActor.h"

#include "PuzzleSystem/Components/InventoryComponent.h"

APickUpActor::APickUpActor()
: AInteractableActorBase()
{

}

void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
	//gameInstanceActual = Cast<BP_Remannat_GI>(gameInstance);
}

void APickUpActor::InteractWith(UInventoryComponent* inventory)
{
	inventory->AddItem(this);

	//gameinstance->Getw/eWidget->GetInventoryImage->setinventoryimage.
	Destroy();
}