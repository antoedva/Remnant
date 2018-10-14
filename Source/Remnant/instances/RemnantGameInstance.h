#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RemnantGameInstance.generated.h"

class UInGameUI;

UCLASS()
class REMNANT_API URemnantGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	URemnantGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInGameUI* InGameUI;

};
