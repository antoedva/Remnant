#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class APickUpActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventoryComponent();

	bool HasItem(const FString& itemName);
	bool HasItem(APickUpActor* pickUp);
	void AddItem(APickUpActor* pickUp);
	void ResetInventory();

protected:

	virtual void BeginPlay() override;

private:

	APickUpActor* item;

};