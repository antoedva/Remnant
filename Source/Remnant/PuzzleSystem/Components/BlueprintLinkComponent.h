// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BlueprintLinkComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerOne);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerTwo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerThree);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UBlueprintLinkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBlueprintLinkComponent();

	void BroadcastToBlueprint(int channel);

	// The designers wanted an optional array of actors so that they could script objects depending on the actors in this arrays position or w/e.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueprintLink")
	TArray<AActor*> optionalActorArray;

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(BlueprintAssignable)
	FOnTriggerOne onTriggerOne;

	UPROPERTY(BlueprintAssignable)
	FOnTriggerOne onTriggerTwo;

	UPROPERTY(BlueprintAssignable)
	FOnTriggerOne onTriggerThree;
};
