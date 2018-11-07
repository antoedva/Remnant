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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTraverse);

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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANT_API UTraverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraverseComponent(const FObjectInitializer& init);

	void TraverseDimension();
	
	enum Dimension
	{
		PAST,
		PRESENT
	};

protected:
	void BeginPlay() override;
	void TickComponent(float delta_time, enum ELevelTick tick_type, FActorComponentTickFunction* this_tick_function) override;

private:
	Dimension dimension_;

	UPROPERTY(EditDefaultsOnly, Category = "LevelManager")
	TSubclassOf<AActor> lsm_bp_;

	ALevelStreamManager* lsm_;
	FBox level_bounds_;
	float level_length_ = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Traverse")
	FTraverseShader traverse_shader_;
	void InitializeShaders();
	void TraverseShaderStart(FTraverseShader shader);

	UPROPERTY(EditDefaultsOnly, Category = "Traverse")
	TSubclassOf<AActor> sphere_bp_;

	AActor* sphere_;
	void SpawnSphere();

	UPROPERTY(BlueprintAssignable)
	FOnTraverse on_traverse_;

private:
	TMap<LevelID, TArray<AActor*>> level_actor_arrays_;
	void SortActors(AActor* player, TArray<AActor*> array_to_sort, TArray<AActor*>& output);
	bool UpdateLevelObjects();
	bool ChangeActorCollision(const bool ignore_distance = false);

private:
	FTimeline timeline_;
	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	UCurveFloat* curve_;

	// Magic offset to make the sphere line up with the traverse shader
	UPROPERTY(EditDefaultsOnly)
	float magic_number_ = 50.0f;
	
	float curve_value_;
	float timeline_length_;
	float timeline_position_;
	bool first_skipped_ = false;

	void SetupTimeline();

	UFUNCTION()
	void TimelineCB();
	UFUNCTION()
	void TimelineEndCB();

public:
	Dimension GetCurrentDimension() const { return dimension_; }
	bool GetFirstSkipped() const { return first_skipped_; }
	FTraverseShader GetTraverseShader() { return traverse_shader_; }
};


