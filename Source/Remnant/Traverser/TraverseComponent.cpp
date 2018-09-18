// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"

UTraverseComponent::UTraverseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//player_ = Cast<AFP_Character>(GetOwner());
	//if(!player_)
	//	UE_LOG(LogTemp, Error, TEXT("Failed to get player! That is bad"));
}

void UTraverseComponent::TraverseDimension()
{
}

void UTraverseComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UTraverseComponent::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function)
{
	Super::TickComponent(delta_time, tick_type, this_tick_function);
}

