#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class REMNANT_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* crosshairImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* inventoryImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* pickupText;

};
