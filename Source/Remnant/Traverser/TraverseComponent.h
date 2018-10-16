// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TraverseComponent.generated.h"

class AFP_Character;
class UWorld;
class ULevelStreaming;
class ALevelStreamManager;
class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPastShaderStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPresentShaderStart);

USTRUCT()
struct REMNANT_API FTraverseShader
{
	GENERATED_BODY()
public:
	UMaterialParameterCollectionInstance* collection_instance_;
	float current_distance_;
	
	UPROPERTY(EditDefaultsOnly)
	UMaterialParameterCollection* parameter_collection_;
	UPROPERTY(EditDefaultsOnly)
	float duration_;
	UPROPERTY(EditDefaultsOnly)
	float max_distance_;

	FTraverseShader() 
		: parameter_collection_(nullptr)
		, collection_instance_(nullptr)
		, duration_(0.0f), current_distance_(0.0f)
		, max_distance_(5000.0f)
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

	UPROPERTY(BlueprintAssignable)
	FOnPastShaderStart past_start_;
	UPROPERTY(BlueprintAssignable)
	FOnPresentShaderStart present_start_;

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

	void ToggleObjectVisibility(AActor* actor);
	void UpdateTraverseShaders();
	void StartShaderTimer(FTraverseShader shader);

	AActor* sphere_;
	USphereComponent* sphere_collision_;
	float current_distance_ = 0.0f;

	TMap<int, TArray<AActor*>> level_actor_arrays_;

	void SortActors(AActor* player, TArray<AActor*> array_to_sort, TArray<AActor*>& output);
	bool UpdateLevelObjects();
	bool ChangeActorCollision();

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool use_old_traverse_ = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	float sphere_scale_scale_ = 0.02f;
};


