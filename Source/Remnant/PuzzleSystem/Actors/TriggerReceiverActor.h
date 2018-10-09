#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerReceiverActor.generated.h"

class UBlueprintLinkComponent;

UCLASS()
class REMNANT_API ATriggerReceiverActor : public AActor
{
	GENERATED_BODY()

public:

	ATriggerReceiverActor();
	~ATriggerReceiverActor();

	bool TriggerThisReceiver(int channel);

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UBlueprintLinkComponent* blueprintLink;

};