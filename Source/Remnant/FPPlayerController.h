#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPPlayerController.generated.h"

class UInGameUI;

UCLASS()
class REMNANT_API AFPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AFPPlayerController();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInGameUI* inGameUI;

protected:

	void BeginPlay() override;

};
