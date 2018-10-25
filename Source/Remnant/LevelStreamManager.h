// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreaming.h"
#include "LevelStreamManager.generated.h"

UENUM()
enum class LevelID
{
	OBJECT = 0,
	PAST,
	PRESENT
};

struct FLevel
{
	FLevel() = default;
	FLevel(ULevelStreaming* stream, FBox box, LevelID id, ALevelStreamingVolume* volume) : level_stream_(stream), level_bounds_(box), id_(id), volume_(volume) {}
	~FLevel() { if(level_stream_) delete level_stream_; }

	ULevelStreaming* GetLevelStream() { return level_stream_; }
	
	void SetLevelBounds(const FBox& bounds) { level_bounds_ = bounds; }
	FBox GetLevelBounds() const { return level_bounds_; }

	void SetID(LevelID id) { id_ = id; }
	LevelID GetID() const { return id_; }

	ALevelStreamingVolume* GetVolume() const { if (!volume_) UE_LOG(LogTemp, Error, TEXT("Level volume is nullptr! You will crash if you traverse!")); return volume_; }

private:
	ULevelStreaming* level_stream_;
	FBox level_bounds_;
	LevelID id_;
	ALevelStreamingVolume* volume_;
};

UCLASS()
class REMNANT_API ALevelStreamManager : public AActor
{
	GENERATED_BODY()
	
public:
	ALevelStreamManager();
	void Destroyed() override;

	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
	bool LoadAllLevels();
	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
	void UnloadAllLevels();

	FORCEINLINE void HideLevel(ULevelStreaming* level) { level->SetShouldBeVisible(false); }
	FORCEINLINE void ShowLevel(ULevelStreaming* level) { level->SetShouldBeVisible(false); }

	FLevel* GetLevel(LevelID id);
	const TMap<LevelID, FLevel*> GetAllLevels() const { return level_streams_; }

private:
	TMap<LevelID, FLevel*> level_streams_;
};
