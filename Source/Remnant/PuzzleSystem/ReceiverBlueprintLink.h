// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReceiverBlueprintLink.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrigger);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UReceiverBlueprintLink : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UReceiverBlueprintLink();

	void BroadcastToBlueprint();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(BlueprintAssignable)
	FOnTrigger onTrigger;

};
