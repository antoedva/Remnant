// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Traverser/TraverseComponent.h"
#include "Components/TimelineComponent.h"

#include "ClockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClockPlaced, FVector, spawn_location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClockPickedUp);

class UStaticMeshComponent;
class AFP_Character;
class UTraverseComponent;
class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

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
	UPROPERTY(BlueprintAssignable)
	FOnClockPlaced on_clock_placed_;
	UPROPERTY(BlueprintAssignable)
	FOnClockPickedUp on_clock_picked_up_;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	TSubclassOf<AActor> clock_bp_;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	TSubclassOf<AActor> base_item_;

	UPROPERTY(EditDefaultsOnly)
	UMaterialParameterCollection* parameter_collection_;
	UMaterialParameterCollectionInstance* collection_instance_;

	AFP_Character* player_;
	AActor* clock_;
	FVector spawn_location_;
	float clock_length_;
	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	float throw_distance_ = 1000.0f;

	// THIS FUNCTION IS UGLY!!
	void ResetTaggedActors();
	void SwapTaggedActors();

	// Is only used to get the length of the clock in BeginPlay
	void SetupClock();

	bool GetSpawnLocation(OUT FVector& location) const;
	bool LineTrace(OUT FHitResult& result) const;
	bool StartShader(FTraverseShader shader);
	bool StopShader(FTraverseShader shader);
	float last_distance_;
	float outline_width_;
	float outline_strength_;

	TSet<AActor*> current_actors_in_clock_;
	void ToggleObjectsInClock(TSet<AActor*> actor_set, bool force = false, ECollisionResponse response = ECollisionResponse::ECR_Block);
	bool GetOverlappingActors(TSet<AActor*>& out_actors, TSubclassOf<AActor> filter = nullptr, const FName& optional_tag = FName("")) const;

	TSet<AActor*> actors_to_freeze_;
	void ToggleFrozenActors();
	TSet<AActor*> tagged_actors_;

	UTraverseComponent* traverse_component_;

	// Timeline
	bool has_reversed_;

	FTimeline timeline_;
	UPROPERTY()
	UCurveFloat* curve_;

	void SetupTimeline();

	UFUNCTION()
	void TimelineCB();
	UFUNCTION()
	void TimelineEndCB();
};
