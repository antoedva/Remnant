#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ETriggerBroadcastChannel : uint8
{
	CHANNEL_ONE = 1 UMETA(DisplayName = "Channel One"),
	CHANNEL_TWO UMETA(DisplayName = "Channel Two"),
	CHANNEL_THREE UMETA(DisplayName = "Channel Three")
};