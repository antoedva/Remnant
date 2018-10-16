// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreaming.h"
#include "LevelStreamManager.generated.h"

UCLASS()
class REMNANT_API ALevelStreamManager : public AActor
{
	GENERATED_BODY()
	
public:
	enum LevelID
	{
		OBJECT = 0,
		PAST,
		PRESENT
	};

	ALevelStreamManager();

	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
	bool LoadAllLevels();
	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
	void UnloadAllLevels();

	FORCEINLINE void HideLevel(ULevelStreaming* level) { level->SetShouldBeVisible(false); }
	FORCEINLINE void ShowLevel(ULevelStreaming* level) { level->SetShouldBeVisible(false); }

	ULevelStreaming* GetLevel(LevelID id);
	const TMap<LevelID, ULevelStreaming*> GetAllLevels() const { return level_streams_; }

private:
	TMap<LevelID, ULevelStreaming*> level_streams_;
};
