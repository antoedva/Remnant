#include "InventoryComponent.h"

#include "PuzzleSystem/Actors/PickUpActor.h"
#include "UI/InGameUI.h"
#include "FPPlayerController.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Styling/SlateBrush.h"
#include "Public/TimerManager.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::DisplayPickedUpItem(const FString& text)
{
	const UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
	if (!ui)
		return;
	if (!ui->pickupText)
		return;

	const FText displayed_text = FText::FromString(text);
	ui->pickupText->SetText(displayed_text);

	auto& tm = GetWorld()->GetTimerManager();
	tm.SetTimer(text_handle_, this, &UInventoryComponent::TextEndCB, text_duration_, false);
}

void UInventoryComponent::TextEndCB()
{
	const UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
	if (!ui)
		return;

	const FText text = FText::FromString("");
	ui->pickupText->SetText(text);
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

	const FString text = FString("Picked up " + item->GetName());
	DisplayPickedUpItem(text);
}

void UInventoryComponent::ResetInventory()
{
	const FString text = FString("Used " + item->GetName());
	DisplayPickedUpItem(text);

	item->Destroy();
	item = nullptr;

	UInGameUI* ui = Cast<AFPPlayerController>(GetWorld()->GetFirstPlayerController())->inGameUI;
	ui->inventoryImage->SetVisibility(ESlateVisibility::Hidden);
}