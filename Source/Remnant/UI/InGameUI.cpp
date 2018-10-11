#include "InGameUI.h"

#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"

void UInGameUI::NativeConstruct()
{
	UPanelWidget* rootWidget = Cast<UPanelWidget>(GetRootWidget());

	inventoryIcon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Inventory Icon"));
	
	inventoryIcon->SetVisibility(ESlateVisibility::Hidden);
	rootWidget->AddChild(inventoryIcon);
}

void UInGameUI::SetInventoryIcon(FSlateBrush& brush)
{
	inventoryIcon->SetBrush(brush);
	inventoryIcon->SetVisibility(ESlateVisibility::Visible);
}

void UInGameUI::ResetInventoryIcon()
{
	inventoryIcon->SetVisibility(ESlateVisibility::Hidden);
}