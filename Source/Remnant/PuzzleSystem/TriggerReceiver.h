#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerReceiver.generated.h"

class UReceiverBlueprintLink;

UCLASS()
class REMNANT_API ATriggerReceiver : public AActor
{
	GENERATED_BODY()

public:

	ATriggerReceiver();
	~ATriggerReceiver();

	bool TriggerThisReceiver();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UReceiverBlueprintLink* blueprintLink;

};
