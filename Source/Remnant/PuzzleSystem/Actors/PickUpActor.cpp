#include "PickUpActor.h"

#include "PuzzleSystem/Components/InventoryComponent.h"
#include "FPPlayerController.h"
#include "UI/InGameUI.h"
#include "Components/Image.h"

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
	UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
	if (ui)
	{
		if (ui->inventoryImage)
		{
			ui->inventoryImage->SetBrush(brush);
			ui->inventoryImage->SetVisibility(ESlateVisibility::Visible);
		}
	}

	SetActorHiddenInGame(true);
	SetActorLocation(FVector(0.0f));
}