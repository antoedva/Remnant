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

USTRUCT()
struct REMNANT_API FTraverseShader
{
	GENERATED_BODY()
public:
	UMaterialParameterCollectionInstance* material_instance_;
	float current_distance_;
	FTimerHandle shader_timer_handle_;

	UPROPERTY(EditDefaultsOnly)
	UMaterialParameterCollection* parameter_collection_;
	UPROPERTY(EditDefaultsOnly)
	float duration_;
	UPROPERTY(EditDefaultsOnly)
	float max_distance_;

	FTraverseShader() : parameter_collection_(nullptr), material_instance_(nullptr), duration_(5.0f), current_distance_(0.0f), max_distance_(5000.0f), shader_timer_handle_() {}
};

UCLASS()
class REMNANT_API UTraverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraverseComponent();
	
	void TraverseDimension();
	void SetTraverseAllowed(const bool state) { traverse_allowed_ = state; }
	bool GetTraverseAllowed() const { return traverse_allowed_; }
	
	UPROPERTY(EditAnywhere)
	FTraverseShader past_traverse_shader_;
	
	UPROPERTY(EditAnywhere)
	FTraverseShader present_traverse_shader_;

protected:
	void BeginPlay() override;
	
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
	//void UpdateTraverseShaders();

	//void StartShaderTimer(FTraverseShader shader);
	//void OnShaderTimerEnd();
};