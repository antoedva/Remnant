// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "LevelStreamManager.h"
#include "Engine/LevelBounds.h"
#include "Engine/World.h"
#include "Engine/LevelStreamingVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"


ALevelStreamManager::ALevelStreamManager()
{
}

FLevel* ALevelStreamManager::GetLevel(LevelID id)
{
	for (const auto& level : level_streams_)
	{
		if (level.Key == id)
			return level.Value;
	}
	return nullptr;
}

void ALevelStreamManager::Destroyed()
{
	UnloadAllLevels();
}

bool ALevelStreamManager::LoadAllLevels()
{
	UWorld* world = GetWorld();

	const TArray<ULevelStreaming*> level_streams = world->GetStreamingLevels();
	for (ULevelStreaming* level_stream : level_streams)
	{
		if (!level_stream)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get level stream!"));
			return false;
		}

		const FName full_level_name = world->IsPlayInEditor() ?
			level_stream->PackageNameToLoad : level_stream->GetWorldAssetPackageFName();

		FLatentActionInfo latent_info;
		UGameplayStatics::LoadStreamLevel(world, full_level_name, true, true, latent_info);

		FVector origin(0.0f);
		FVector extent(0.0f);
		FBox box;
		ALevelStreamingVolume* volume = nullptr;

		for (auto* actor : level_stream->GetLoadedLevel()->Actors)
		{
			volume = Cast<ALevelStreamingVolume>(actor);
			if (!volume)
				continue;

			volume->GetActorBounds(false, origin, extent);
			box = ALevelBounds::CalculateLevelBounds(level_stream->GetLoadedLevel()).BuildAABB(origin, extent);
			break;
		}

		const FString level_name = FPackageName::GetShortName(world->IsPlayInEditor() ?
			level_stream->PackageNameToLoad.ToString() : level_stream->GetWorldAssetPackageName());

		LevelID id = LevelID::OBJECT;
		if (level_name.Compare("ObjectLevel") == 0)
			id = LevelID::OBJECT;
		else if (level_name.Compare("ObjectLevel_Room02") == 0)
			id = LevelID::OBJECT2;
		else if (level_name.Compare("Level_04_Puzzle_B") == 0)
			id = LevelID::OBJECT1;
		else if (level_name.Compare("Level_01_Past") == 0)
			id = LevelID::PAST;
		else if (level_name.Compare("Level_02_Past") == 0)
			id = LevelID::PAST1;
		else if (level_name.Compare("Level_03_Past") == 0)
			id = LevelID::PAST2;
		else if (level_name.Compare("Level_04_Past") == 0)
			id = LevelID::PAST3;
		else if (level_name.Compare("Level_01_Present") == 0)
			id = LevelID::PRESENT;
		else if (level_name.Compare("Level_02_Present") == 0)
			id = LevelID::PRESENT1;
		else if (level_name.Compare("Level_03_Present") == 0)
			id = LevelID::PRESENT2;
		else if (level_name.Compare("Level_04_Present") == 0)
			id = LevelID::PRESENT3;
	
		level_streams_.Add(id, new FLevel(level_stream, box, id, volume));
	}

	return true;
}

void ALevelStreamManager::UnloadAllLevels()
{
	UWorld* world = AActor::GetWorld();

	for (auto& level_map : level_streams_)
	{
		ULevelStreaming* stream = level_map.Value->GetLevelStream();

		if (!stream)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get level stream!"));
			return;
		}

		const FName full_level_name = world->IsPlayInEditor() ?
			stream->PackageNameToLoad : stream->GetWorldAssetPackageFName();

		FLatentActionInfo latent_info;
		UGameplayStatics::UnloadStreamLevel(world, full_level_name, latent_info, true);

		level_streams_.Empty();
	}
}