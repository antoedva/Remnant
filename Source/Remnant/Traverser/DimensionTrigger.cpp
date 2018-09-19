// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "DimensionTrigger.h"

ADimensionTrigger::ADimensionTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ADimensionTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ADimensionTrigger::OnOverlapEnd);
}

void ADimensionTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (!linked_trigger_)
		UE_LOG(LogTemp, Error, TEXT("Trigger %s failed to link other trigger!"), *GetName());
}

FVector ADimensionTrigger::GetNewPosition() const
{
	if (!actor_)
		return FVector(0.0f);

	const FVector offset = actor_->GetActorLocation() - GetActorLocation();
	const FVector new_position = linked_trigger_->GetActorLocation() + offset;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *new_position.ToString());
	return new_position;
}

void ADimensionTrigger::OnOverlapBegin(AActor* /*overlapped_actor*/, AActor* other_actor)
{
	if (other_actor && (other_actor != this))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s entered %s"), *other_actor->GetName(), *GetName());
		actor_ = other_actor;
	}
}

void ADimensionTrigger::OnOverlapEnd(AActor* /*overlapped_actor*/, AActor* other_actor)
{
	if (other_actor && (other_actor != this))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s exited %s"), *other_actor->GetName(), *GetName());
		actor_ = nullptr; // clear the actor 
	}
}