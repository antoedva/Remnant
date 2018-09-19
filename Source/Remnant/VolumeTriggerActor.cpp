#include "VolumeTriggerActor.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AVolumeTriggerActor::AVolumeTriggerActor()
: ATriggerActor()
{
	UBoxComponent* box = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = box;
	box->InitBoxExtent(boxDimensions);
	box->SetGenerateOverlapEvents(true);
	box->OnComponentBeginOverlap.AddDynamic(this, &AVolumeTriggerActor::OnOverlapBegin);
	box->OnComponentEndOverlap.AddDynamic(this, &AVolumeTriggerActor::OnOverlapEnd);
}

AVolumeTriggerActor::~AVolumeTriggerActor()
{

}

void AVolumeTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Green, true, -1, 0, 5);
	UE_LOG(LogTemp, Warning, TEXT("I hate Unreal Engine."));
}

void AVolumeTriggerActor::OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyId, bool bFromSweep, const FHitResult& sweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor entered trigger."));
}

void AVolumeTriggerActor::OnOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor left trigger."));
}