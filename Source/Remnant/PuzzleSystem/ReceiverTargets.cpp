// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "ReceiverTargets.h"

bool UReceiverTargets::IncrementID()
{
	if (!(current_id_ + 1 <= num_nodes_))
		return false;

	current_id_++;
	return true;
}

bool UReceiverTargets::DecrementID()
{
	if (!(current_id_ - 1 >= 0))
		return false;

	current_id_--;
	return true;
}

AActor* UReceiverTargets::GetCurrentNode()
{
	return receiver_targets_[current_id_];
}

void UReceiverTargets::BeginPlay()
{
	if (!(receiver_targets_.Num() > 0))
		return;

	num_nodes_ = receiver_targets_.Num() - 1;
}