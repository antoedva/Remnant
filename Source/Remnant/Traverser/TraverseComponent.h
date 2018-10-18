// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../LevelStreamManager.h"
#include "Components/TimelineComponent.h"

#include "TraverseComponent.generated.h"

class AFP_Character;
class UWorld;
class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;
class USphereComponent;

USTRUCT()
struct REMNANT_API FTraverseShader
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UMaterialParameterCollection* parameter_collection_;
	UMaterialParameterCollectionInstance* collection_instance_;

	FTraverseShader() 
		: parameter_collection_(nullptr)
		, collection_instance_(nullptr)
	{}
};

UCLASS()
class REMNANT_API UTraverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraverseComponent(const FObjectInitializer& init);

	void TraverseDimension();
	void SetTraverseAllowed(const bool state) { traverse_allowed_ = state; }
	bool GetTraverseAllowed() const { return traverse_allowed_; }
	
	UPROPERTY(EditAnywhere)
	FTraverseShader past_traverse_shader_;
	
	UPROPERTY(EditAnywhere)
	FTraverseShader present_traverse_shader_;

	UPROPERTY(EditDefaultsOnly, Category = "Traverse")
	TSubclassOf<AActor> sphere_bp_;

	void SpawnSphere();

protected:
	void BeginPlay() override;
	void TickComponent(float delta_time, enum ELevelTick tick_type, FActorComponentTickFunction* this_tick_function) override;
	
private:
	enum Dimension
	{
		PAST,
		PRESENT
	} dimension_;

	bool traverse_allowed_;
	
	UPROPERTY(EditDefaultsOnly, Category = "LevelManager")
	TSubclassOf<AActor> lsm_bp_;

	ALevelStreamManager* lsm_;
	FBox level_bounds_;
	float level_length_ = 0.0f;

	void ToggleObjectVisibility(AActor* actor);
	void InitializeShaders();
	void TraverseShaderStart(FTraverseShader shader);

	AActor* sphere_;

	TMap<LevelID, TArray<AActor*>> level_actor_arrays_;
	void SortActors(AActor* player, TArray<AActor*> array_to_sort, TArray<AActor*>& output);
	bool UpdateLevelObjects();
	bool ChangeActorCollision(const bool ignore_distance = false);

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool use_old_traverse_ = false;

	FTimeline timeline_;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* curve_;

	float curve_value_;
	float timeline_position_;

	void SetupTimeline();
	bool first_skipped_ = false;

	UFUNCTION()
	void TimelineCB();
	UFUNCTION()
	void TimelineEndCB();
};


