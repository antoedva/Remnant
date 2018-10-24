#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETriggerBroadcastChannel : uint8
{
	CHANNEL_ZERO = 0 UMETA(DisplayName = "Channel Zero"),
	CHANNEL_ONE UMETA(DisplayName = "Channel One"),
	CHANNEL_TWO UMETA(DisplayName = "Channel Two"),
	CHANNEL_THREE UMETA(DisplayName = "Channel Three"),
	CHANNEL_FOUR UMETA(DisplayName = "Channel Four"),
	CHANNEL_FIVE UMETA(DisplayName = "Channel Five"),
	CHANNEL_SIX UMETA(DisplayName = "Channel Six"),
	CHANNEL_SEVEN UMETA(DisplayName = "Channel Seven"),
	CHANNEL_EIGHT UMETA(DisplayName = "Channel Eight"),
	CHANNEL_NINE UMETA(DisplayName = "Channel Nine"),
	CHANNEL_TEN UMETA(DisplayName = "Channel Ten (Freeze)"),
	CHANNEL_ELEVEN UMETA(DisplayName = "Channel Eleven (Unfreeze)"),
	ALL UMETA(DisplayName = "All Channels")
};