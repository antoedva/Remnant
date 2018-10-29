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

void ALevelStreamManager::CreateLevel(TArray<ULevelStreaming*> streams, LevelID id)
{
	FBox box;
	FVector origin(0.0f);
	FVector extent(0.0f);
	ALevelStreamingVolume* volume = nullptr;

	for (auto* stream : streams)
	{
		if (!stream)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get level stream!"));
			return;
		}

		const FName full_level_name = GetWorld()->IsPlayInEditor() ?
			stream->PackageNameToLoad : stream->GetWorldAssetPackageFName();

		FLatentActionInfo latent_info;
		UGameplayStatics::LoadStreamLevel(GetWorld(), full_level_name, true, true, latent_info);

		for (auto* actor : stream->GetLoadedLevel()->Actors)
		{
			volume = Cast<ALevelStreamingVolume>(actor);
			if (!volume)
				continue;

			volume->GetActorBounds(false, origin, extent);
			box = ALevelBounds::CalculateLevelBounds(stream->GetLoadedLevel()).BuildAABB(origin, extent);
			break;
		}
	}

	level_streams_.Add(id, new FLevel(streams, box, id, volume));
}

void ALevelStreamManager::Destroyed()
{
	UnloadAllLevels();
}

bool ALevelStreamManager::LoadAllLevels()
{
	TArray<ULevelStreaming*> past_streams;
	TArray<ULevelStreaming*> present_streams;
	TArray<ULevelStreaming*> obj_streams;

	const TArray<ULevelStreaming*> level_streams = GetWorld()->GetStreamingLevels();
	for (auto* level_stream : level_streams)
	{
		const FString level_name = FPackageName::GetShortName(GetWorld()->IsPlayInEditor() ?
			level_stream->PackageNameToLoad.ToString() : level_stream->GetWorldAssetPackageName());

		if (level_name.Compare("ObjectLevel") == 0 || level_name.Compare("ObjectLevel_Room02") == 0 || level_name.Compare("Level_04_Puzzle_B") == 0)
			obj_streams.Add(level_stream);
		else if (level_name.Compare("Level_01_Past") == 0 || level_name.Compare("Level_02_Past") == 0 || level_name.Compare("Level_03_Past") == 0 || level_name.Compare("Level_04_Past") == 0)
			past_streams.Add(level_stream);
		else if (level_name.Compare("Level_01_Present") == 0 || level_name.Compare("Level_02_Present") == 0 || level_name.Compare("Level_03_Present") == 0 || level_name.Compare("Level_04_Present") == 0)
			present_streams.Add(level_stream);
	}

	CreateLevel(past_streams, LevelID::PAST);
	CreateLevel(present_streams, LevelID::PRESENT);
	CreateLevel(obj_streams, LevelID::OBJECT);

	return true;
}

void ALevelStreamManager::UnloadAllLevels()
{
	UWorld* world = AActor::GetWorld();

	for (auto& level_map : level_streams_)
	{
		TArray<ULevelStreaming*> streams = level_map.Value->GetLevelStreams();

		for (auto* stream : streams)
		{

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
}