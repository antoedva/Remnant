// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "TraverseComponent.h"
#include "../FP_Character.h"
#include "DimensionTrigger.h"
#include "Engine/World.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

UTraverseComponent::UTraverseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	player_ = Cast<AFP_Character>(GetOwner());
	if (!player_)
		UE_LOG(LogTemp, Error, TEXT("Failed to get player! That is bad"));


}

void UTraverseComponent::TraverseDimension()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADimensionTrigger::StaticClass(), actors);
	
	for (AActor* actor : actors)
	{
		ADimensionTrigger* trigger = Cast<ADimensionTrigger>(actor);
		if (!trigger)
			break;

		TSet<AActor*> overlapping_actors;
		trigger->GetOverlappingActors(overlapping_actors, AFP_Character::StaticClass());
		
		for (AActor* overlapped_actor : overlapping_actors)
		{
			AFP_Character* character = Cast<AFP_Character>(overlapped_actor);
			if (!overlapped_actor)
				break;

			if (character == player_)
			{
				player_->SetActorLocation(trigger->GetNewPosition());
				return;
			}
		}
	}
}

void UTraverseComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UTraverseComponent::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* this_tick_function)
{
	Super::TickComponent(delta_time, tick_type, this_tick_function);

}

