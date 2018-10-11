#include "PickUpActor.h"

#include "PuzzleSystem/Components/InventoryComponent.h"
#include "instances/RemnantGameInstance.h"
#include "UI/InGameUI.h"

APickUpActor::APickUpActor()
: AInteractableActorBase()
{

}

void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
}

void APickUpActor::InteractWith(UInventoryComponent* inventory)
{
	inventory->AddItem(this);

	Cast<URemnantGameInstance>(GetGameInstance())->InGameUI->SetInventoryIcon(brush);

	Destroy();
}