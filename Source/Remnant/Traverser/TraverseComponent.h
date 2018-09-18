// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TraverseComponent.generated.h"

//class AFP_Character;
//class ADimensionTrigger;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UTraverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraverseComponent();

	void TraverseDimension();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function) override;
	
private:
	//AFP_Character* player_;
	//ADimensionTrigger* trigger_;
};
