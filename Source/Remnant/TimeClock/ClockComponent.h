// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Traverser/TraverseComponent.h"

#include "ClockComponent.generated.h"

class UStaticMeshComponent;
class AFP_Character;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANT_API UClockComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClockComponent();

	bool ThrowClock();
	bool PickUpClock(const bool ignore_linetrace = false);

protected:
	void BeginPlay() override;
	void TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	TSubclassOf<AActor> clock_bp_;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	TSubclassOf<AActor> base_item_;

	AFP_Character* player_;
	AActor* clock_;
	FVector spawn_location_;

	bool GetSpawnLocation(OUT FVector& location) const;
	bool LineTrace(OUT FHitResult& result) const;
	void ToggleObjectsInClock();
	bool StartShader(FTraverseShader shader);
	bool StopShader(FTraverseShader shader);

	TSet<AActor*> current_actors_in_clock_;
	bool GetOverlappingActors(TSet<AActor*>& out_actors, TSubclassOf<AActor> filter = nullptr) const;

	TSet<AActor*> actors_to_freeze_;
	void ToggleFrozenActors();
};
