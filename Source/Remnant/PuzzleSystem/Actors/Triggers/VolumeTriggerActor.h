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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Volume Trigger")
	bool IsActorInsideVolume() { return isActorInsideVolume; }

protected:

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTriggerComponent* triggerComponent;
private:
	UPROPERTY(Category = "Volume Trigger Actor", EditAnywhere)
	TArray<AActor*> actorsThatTriggers; // Pointer to the actor that has the ability to trigger this volume. Will most likely be the player in most instances.

	bool isActorInsideVolume;

	unsigned amount_of_actors_inside_;
};