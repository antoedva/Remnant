#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"

#include "VolumeTriggerActor.generated.h"

class UTriggerComponent;

UCLASS()
class REMNANT_API AVolumeTriggerActor : public ATriggerBox
{
	GENERATED_BODY()
	
public:

	AVolumeTriggerActor();
	~AVolumeTriggerActor();

	UFUNCTION()
	void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION()
	void OnOverlapEnd(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION(BlueprintCallable, Category = "Volume Trigger")
	bool IsActorInsideVolume() { return isActorInsideVolume; }

protected:

	void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UTriggerComponent* triggerComponent;

	UPROPERTY(Category = "Volume Trigger Actor", EditAnywhere)
	AActor* actorThatTriggers; // Pointer to the actor that has the ability to trigger this volume. Will most likely be the player in most instances.

	bool isActorInsideVolume;
};