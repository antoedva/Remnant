#include "LockedInteractableTriggerActor.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "PuzzleSystem/Components/InventoryComponent.h"
#include "PuzzleSystem/Components/TriggerComponent.h"

ALockedInteractableTriggerActor::ALockedInteractableTriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	triggerComponent = CreateDefaultSubobject<UTriggerComponent>("Trigger Component");
	audioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
}

void ALockedInteractableTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	audioComponent->SetSound(soundClip);
	audioComponent->VolumeMultiplier = volumeMultiplier;
}

void ALockedInteractableTriggerActor::InteractWith(UInventoryComponent* inventory)
{
	if (inventory->HasItem(requiredItemToTrigger))
	{
		triggerComponent->TriggerAllReceivers();
		inventory->ResetInventory();
	}
	else
	{
		audioComponent->Play();
	}
}