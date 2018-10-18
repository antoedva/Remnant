#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PuzzleSystem/TriggerBroadcastChannel.h"

#include "TriggerComponent.generated.h"

class ATriggerReceiverActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerReceivers);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UTriggerComponent();

	void TriggerAllRecievers();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(BlueprintAssignable)
	FOnTriggerReceivers onTrigger;

	UPROPERTY(Category = "Volume Trigger Actor", EditAnywhere)
	TArray<ATriggerReceiverActor*> triggerReceivers;

	UPROPERTY(EditAnywhere)
	ETriggerBroadcastChannel broadcastChannel;

};