#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class APickUpActor;
struct FTimerHandle;

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
	void DisplayPickedUpItem(const FString& text);

protected:

	virtual void BeginPlay() override;

private:
	APickUpActor* item;

	// Display text stuff
	UPROPERTY(EditDefaultsOnly, Category = "Display")
	float text_duration_ = 2.0f;

	UFUNCTION()
	void TextEndCB();
	FTimerHandle text_handle_;


};