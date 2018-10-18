#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerReceiverActor.generated.h"

class UBlueprintLinkComponent;
class UReceiverTargets;

UCLASS()
class REMNANT_API ATriggerReceiverActor : public AActor
{
	GENERATED_BODY()

public:

	ATriggerReceiverActor();
	~ATriggerReceiverActor();

	bool TriggerThisReceiver(int channel);
	bool TriggerThisReceiverReverse(int channel);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triggers")
	UReceiverTargets* targets_;

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UBlueprintLinkComponent* blueprintLink;

};