// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrolRoute.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANT_API UPatrolRoute : public UActorComponent
{
	GENERATED_BODY()

public:	
	TArray<AActor*> GetWaypoints() const { return waypoints_; }

private:
	UPROPERTY(EditInstanceOnly, Category = "AI")
	TArray<AActor*> waypoints_;
	
};
