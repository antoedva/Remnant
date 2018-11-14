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
		// TO DO
		// Send an event - "Missing required item."
		// Could be used for a voiceline, locked item sfx, a ui feedback or something else
		UE_LOG(LogTemp, Warning, TEXT("Missing required item"));
		audioComponent->Play();
	}
}