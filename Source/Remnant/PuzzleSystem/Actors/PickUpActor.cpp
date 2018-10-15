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

	cachedInGameUI = Cast<URemnantGameInstance>(GetGameInstance())->InGameUI;
}

void APickUpActor::InteractWith(UInventoryComponent* inventory)
{
	inventory->AddItem(this);

	cachedInGameUI->SetInventoryIcon(brush);

	Destroy();
}