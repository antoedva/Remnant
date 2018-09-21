#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerReceiver.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrigger);

UCLASS(BlueprintType, Blueprintable)
class REMNANT_API ATriggerReceiver : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ATriggerReceiver();

	void TriggerBlueprint();

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="EventDispatchers")
	FOnTrigger onTrigger;

protected:
	
	virtual void BeginPlay() override;
	
};