// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "LevelStreamManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"


ALevelStreamManager::ALevelStreamManager()
{
}

ULevelStreaming* ALevelStreamManager::GetLevel(LevelID id)
{
	for (const auto& level : level_streams_)
	{
		if (level.Key == id)
			return level.Value;
	}
	return nullptr;
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

		const FString level_name = FPackageName::GetShortName(world->IsPlayInEditor() ?
			level_stream->PackageNameToLoad.ToString() : level_stream->GetWorldAssetPackageName());

		if (level_name.Compare("ObjectLevel") == 0)
			level_streams_.Add(OBJECT, level_stream);
		else if (level_name.Compare("Past") == 0)
			level_streams_.Add(PAST, level_stream);
		else if (level_name.Compare("Present") == 0)
			level_streams_.Add(PRESENT, level_stream);
	}

	return true;
}

void ALevelStreamManager::UnloadAllLevels()
{
	UWorld* world = AActor::GetWorld();

	for (ULevelStreaming* level_stream : world->GetStreamingLevels())
	{
		if (!level_stream)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get level stream!"));
			return;
		}

		const FName full_level_name = world->IsPlayInEditor() ?
			level_stream->PackageNameToLoad : level_stream->GetWorldAssetPackageFName();

		FLatentActionInfo latent_info;
		UGameplayStatics::UnloadStreamLevel(world, full_level_name, latent_info, true);

		level_streams_.Empty();
	}
}