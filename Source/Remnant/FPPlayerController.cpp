#include "FPPlayerController.h"

#include "UI/InGameUI.h"

AFPPlayerController::AFPPlayerController()
{

}

void AFPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (this == pc)
	{
		UE_LOG(LogTemp, Warning, TEXT("The same"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not the same"));
	}
}