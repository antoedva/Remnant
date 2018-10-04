#include "InventoryComponent.h"

#include "PickUpActor.h"

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
	return items.Contains(itemName);
}

bool UInventoryComponent::HasItem(APickUpActor* pickUp)
{
	for (const TPair<FString, APickUpActor*> pair : items)
	{
		if (pair.Value == pickUp)
		{
			return true;
		}
	}
	
	return false;
}

void UInventoryComponent::AddItem(APickUpActor* pickUp)
{
	items.Emplace(pickUp->GetName(), pickUp);
}