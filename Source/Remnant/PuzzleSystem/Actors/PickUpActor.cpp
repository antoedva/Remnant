#include "PickUpActor.h"

#include "PuzzleSystem/Components/InventoryComponent.h"
#include "FPPlayerController.h"
#include "UI/InGameUI.h"
#include "Components/Image.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

APickUpActor::APickUpActor()
: AInteractableActorBase()
{
	audioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
}

void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

	audioComponent->SetSound(soundClip);
}

void APickUpActor::InteractWith(UInventoryComponent* inventory)
{
	inventory->AddItem(this);
	UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
	if (ui)
	{
		if (ui->inventoryImage)
		{
			audioComponent->Play();
			ui->inventoryImage->SetBrush(brush);
			ui->inventoryImage->SetVisibility(ESlateVisibility::Visible);
		}
	}

	Destroy();
}