#include "InGameUI.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"

void UInGameUI::NativeConstruct()
{
	UPanelWidget* rootWidget = Cast<UPanelWidget>(GetRootWidget());
	if (!rootWidget || !WidgetTree)
	{
		return;
	}

	// Setup inventory icon
	{
		inventoryIcon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Inventory Icon"));
		rootWidget->AddChild(inventoryIcon);

		UCanvasPanelSlot* inventoryIconSlot = CastChecked<UCanvasPanelSlot>(inventoryIcon->Slot);
		FAnchors inventoryAnchor(1.0f, 1.0f);
		FVector2D inventorySize(100.0f, 100.0f);
		FVector2D inventoryPosition = -inventorySize * 1.5f;

		inventoryIcon->SetVisibility(ESlateVisibility::Hidden);
		inventoryIconSlot->SetAnchors(inventoryAnchor);
		inventoryIconSlot->SetPosition(inventoryPosition);
		inventoryIconSlot->SetSize(inventorySize);
	}

	// Setup crosshair
	{
		crosshair = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Crosshair"));
		rootWidget->AddChild(inventoryIcon);

		UCanvasPanelSlot* crosshairSlot = CastChecked<UCanvasPanelSlot>(crosshair->Slot);
		FAnchors crosshairAnchor(0.5f, 0.5f);
		FVector2D crosshairSize(65.0f, 65.0f);
		FVector2D crosshairPosition(0.0f, 0.0f);

		crosshairSlot->SetAnchors(crosshairAnchor);
		crosshairSlot->SetPosition(crosshairPosition);
		crosshairSlot->SetSize(crosshairSize);
	}
	
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

void UInGameUI::SetCrosshairBrush(FSlateBrush& brush)
{
	crosshair->SetBrush(brush);
}