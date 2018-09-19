// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TriggerActor.h"
#include "VolumeTriggerActor.generated.h"

UCLASS()
class REMNANT_API AVolumeTriggerActor : public ATriggerActor
{
	GENERATED_BODY()
	
public:

	AVolumeTriggerActor();
	~AVolumeTriggerActor();

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyId,
		bool bFromSweep,
		const FHitResult& sweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* overlappedComp,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex
	);

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(Category = "Volume Trigger Actor", EditAnywhere)
	FVector boxDimensions;
};
