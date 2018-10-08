#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerReceiver.generated.h"

class UReceiverBlueprintLink;
class UReceiverTargets;

UCLASS()
class REMNANT_API ATriggerReceiver : public AActor
{
	GENERATED_BODY()

public:

	ATriggerReceiver();
	~ATriggerReceiver();

	bool TriggerThisReceiver();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targets")
	UReceiverTargets* targets_;

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UReceiverBlueprintLink* blueprintLink;
};
