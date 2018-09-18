#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerReciever.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrigger);

UCLASS()
class REMNANT_API ATriggerReciever : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ATriggerReciever();

	void TriggerBlueprint();

	UPROPERTY(BlueprintAssignable)
	FOnTrigger onTrigger;

protected:
	
	virtual void BeginPlay() override;
	
};