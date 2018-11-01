// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BlueprintLinkComponent.generated.h"

class ATriggerReceiverActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrigger, int, channel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerReverse, int, channel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnotherReceiverTriggered, ATriggerReceiverActor*, receiver, int, channel);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANT_API UBlueprintLinkComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UBlueprintLinkComponent();

	void BroadcastToBlueprint(int channel);
	void BroadcastToBlueprintReverse(int channel);
	void BroadcastToReceiver(ATriggerReceiverActor* receiver, int channel);

	// The designers wanted an optional array of actors so that they could script objects depending on the actors in this arrays position or w/e.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueprintLink")
		TArray<AActor*> optionalActorArray;

protected:

	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintAssignable)
		FOnTrigger on_trigger_;

	UPROPERTY(BlueprintAssignable)
		FOnTriggerReverse onTriggerReverse;

	UPROPERTY(BlueprintAssignable)
		FOnAnotherReceiverTriggered on_another_receiver_triggered_;
};
