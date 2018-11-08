#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PuzzleSystem/TriggerBroadcastChannel.h"

#include "TriggerComponent.generated.h"

class ATriggerReceiverActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerReceivers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerReceiversReverse);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UTriggerComponent();

	void TriggerAllReceivers();
	void TriggerAllReceiversReverse();
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void TriggerReceiver(ATriggerReceiverActor* receiver);
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void ForceTriggerReceiver(ATriggerReceiverActor* receiver, const int channel);

	UFUNCTION(BlueprintCallable, Category = "Channel")
	void SetBroadcastChannel(const int channel) { broadcastChannel = static_cast<ETriggerBroadcastChannel>(channel); }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(Category = "Volume Trigger Actor", EditAnywhere, BlueprintReadOnly)
	TArray<ATriggerReceiverActor*> triggerReceivers;
private:
	
	UPROPERTY(BlueprintAssignable)
	FOnTriggerReceivers onTrigger;

	UPROPERTY(BlueprintAssignable)
	FOnTriggerReceiversReverse onTriggerReverse;

	UPROPERTY(EditAnywhere)
	ETriggerBroadcastChannel broadcastChannel;

};