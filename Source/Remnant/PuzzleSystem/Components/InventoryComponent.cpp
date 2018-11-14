#include "InventoryComponent.h"

#include "PuzzleSystem/Actors/PickUpActor.h"
#include "UI/InGameUI.h"
#include "FPPlayerController.h"
#include "Components/Image.h"
#include "Styling/SlateBrush.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UInventoryComponent::HasItem(const FString& itemName)
{
	if (!item)
		return false;

	if (!(item->GetName().Len() > 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("0"));
		return false;
	}
	if (item->GetName().Compare(itemName) == 0)
	{
		return true;
	}

	return false;
}

bool UInventoryComponent::HasItem(APickUpActor* pickUp)
{
	if (item == pickUp)
	{
		return true;
	}

	return false;
}

void UInventoryComponent::AddItem(APickUpActor* pickUp)
{
	item = pickUp;
	UE_LOG(LogTemp, Warning, TEXT("%s added to inventory"), *item->GetName());
}

void UInventoryComponent::ResetInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("%s used"), *item->GetName());

	item->Destroy();
	item = nullptr;

	UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
	ui->inventoryImage->SetVisibility(ESlateVisibility::Hidden);
}