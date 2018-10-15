#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "InGameUI.generated.h"

class UImage;

UCLASS()
class REMNANT_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	void SetInventoryIcon(FSlateBrush& brush);
	void SetCrosshairBrush(FSlateBrush& brush);

	void ResetInventoryIcon();

private:

	UPROPERTY()
	UImage* inventoryIcon;

	UPROPERTY()
	UImage* crosshair;

};
