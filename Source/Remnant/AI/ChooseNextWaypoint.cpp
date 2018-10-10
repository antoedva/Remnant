// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "ChooseNextWaypoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "PatrolRoute.h"

EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* /*node_memory*/)
{
	const APawn* controlled_pawn = owner_comp.GetAIOwner()->GetPawn();
	if (!ensure(controlled_pawn))
		return EBTNodeResult::Failed;

	const UPatrolRoute* patrol_route = controlled_pawn->FindComponentByClass<UPatrolRoute>();
	if (!ensure(patrol_route))
		return EBTNodeResult::Failed;

	UBlackboardComponent* blackboard_component = owner_comp.GetBlackboardComponent();
	if (!ensure(blackboard_component))
		return EBTNodeResult::Failed;

	const TArray<AActor*> waypoints = patrol_route->GetWaypoints();
	if (!(waypoints.Num() > 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("No waypoints added to actor %s"), *controlled_pawn->GetName());
		return EBTNodeResult::Failed;
	}

	// Set next waypoint
	const int32 index = blackboard_component->GetValueAsInt(key_index_.SelectedKeyName);
	blackboard_component->SetValueAsObject(waypoint_key_.SelectedKeyName, waypoints[index]);

	// Cycle index
	const int32 new_index = (index + 1) % waypoints.Num();
	blackboard_component->SetValueAsInt(key_index_.SelectedKeyName, new_index);

	return EBTNodeResult::Succeeded;
}
