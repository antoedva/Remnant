#include "InGameUI.h"

#include "Components/Image.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	crosshairImage->SetOpacity(0.4f);
}