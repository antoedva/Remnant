// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TraverseComponent.generated.h"

class AFP_Character;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UTraverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraverseComponent();

	void TraverseDimension();
	void SetTraverseAllowed(bool state) { traverse_allowed_ = state; }
	bool GetTraverseAllowed() const { return traverse_allowed_; }

protected:
	virtual void BeginPlay() override;
	
private:
	bool traverse_allowed_;
};
