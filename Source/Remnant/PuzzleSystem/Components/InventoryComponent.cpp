#include "InventoryComponent.h"

#include "PuzzleSystem/Actors/PickUpActor.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UInventoryComponent::HasItem(FString itemName)
{
	if (item && item->GetName() == itemName)
	{
		return true;
	}

	return false;
}

bool UInventoryComponent::HasItem(APickUpActor* pickUp)
{
	if (item && pickUp == item)
	{
		return true;
	}

	return false;
}

void UInventoryComponent::AddItem(APickUpActor* pickUp)
{
	item = pickUp;
}