// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	bool PickUpClock();

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

	bool GetSpawnLocation(OUT FVector& location) const;
	bool LineTrace(OUT FHitResult& result) const;
	TSet<AActor*> GetOverlappingActors() const;
};
