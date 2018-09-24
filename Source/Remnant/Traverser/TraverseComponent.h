// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TraverseComponent.generated.h"

class AFP_Character;
class UWorld;
class ULevelStreaming;

UCLASS()
class REMNANT_API UTraverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraverseComponent();

	void TraverseDimension();
	void SetTraverseAllowed(const bool state) { traverse_allowed_ = state; }
	bool GetTraverseAllowed() const { return traverse_allowed_; }

protected:
	virtual void BeginPlay() override;
	
private:
	enum Dimension
	{
		PAST,
		PRESENT
	} dimension_;

	bool traverse_allowed_;

	void ToggleObjectVisibility(AActor* actor);
};
