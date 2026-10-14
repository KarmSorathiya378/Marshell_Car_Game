// Copyright VELOCITY X Racing. All Rights Reserved.

#include "GameInstance/VelocityGameInstance.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"

UVelocityGameInstance::UVelocityGameInstance()
{
	CurrentScore = 0;
	MaxSavedHighScores = 10;
	TelemetryTracker = nullptr;
}

void UVelocityGameInstance::Init()
{
	Super::Init();

	// Instantiate telemetry tracker
	TelemetryTracker = NewObject<UVelocityTelemetryTracker>(this, TEXT("VelocityTelemetryTracker"));

	// Load high scores
	LoadHighScoresFromDisk();

	UE_LOG(LogTemp, Display, TEXT("VelocityGameInstance initialized with TelemetryTracker and HighScores system."));
}

void UVelocityGameInstance::Shutdown()
{
	SaveHighScoresToDisk();
	Super::Shutdown();
}

// =============================================================================
// TELEMETRY METHODS
// =============================================================================

void UVelocityGameInstance::UpdateTelemetry(float InSpeedMPH, float InSpeedKPH, float InRPM, int32 InGear, float DeltaTime)
{
	if (TelemetryTracker)
	{
		TelemetryTracker->UpdateTelemetry(InSpeedMPH, InSpeedKPH, InRPM, InGear, DeltaTime);
	}
}

FTelemetrySnapshot UVelocityGameInstance::GetTelemetrySnapshot() const
{
	if (TelemetryTracker)
	{
		return TelemetryTracker->GetCurrentSnapshot();
	}
	return FTelemetrySnapshot();
}

float UVelocityGameInstance::GetCurrentSpeedMPH() const
{
	return TelemetryTracker ? TelemetryTracker->GetCurrentSpeedMPH() : 0.0f;
}

float UVelocityGameInstance::GetCurrentSpeedKPH() const
{
	return TelemetryTracker ? TelemetryTracker->GetCurrentSpeedKPH() : 0.0f;
}

float UVelocityGameInstance::GetCurrentRPM() const
{
	return TelemetryTracker ? TelemetryTracker->GetCurrentRPM() : 0.0f;
}

int32 UVelocityGameInstance::GetCurrentGear() const
{
	return TelemetryTracker ? TelemetryTracker->GetCurrentGear() : 0;
}

float UVelocityGameInstance::GetCurrentLapTime() const
{
	return TelemetryTracker ? TelemetryTracker->GetCurrentLapTime() : 0.0f;
}

float UVelocityGameInstance::GetBestLapTime() const
{
	return TelemetryTracker ? TelemetryTracker->GetBestLapTime() : 0.0f;
}

float UVelocityGameInstance::GetLastLapTime() const
{
	return TelemetryTracker ? TelemetryTracker->GetLastLapTime() : 0.0f;
}

int32 UVelocityGameInstance::GetCurrentLapNumber() const
{
	return TelemetryTracker ? TelemetryTracker->GetCurrentLapNumber() : 1;
}

void UVelocityGameInstance::StartNewLap()
{
	if (TelemetryTracker)
	{
		TelemetryTracker->StartNewLap();
	}
}

void UVelocityGameInstance::CompleteCurrentLap()
{
	if (TelemetryTracker)
	{
		TelemetryTracker->CompleteCurrentLap();
	}
}

void UVelocityGameInstance::RecordLapTime(float LapTime)
{
	if (TelemetryTracker)
	{
		TelemetryTracker->RecordLapTime(LapTime);
	}
}

void UVelocityGameInstance::ResetTelemetry()
{
	if (TelemetryTracker)
	{
		TelemetryTracker->ResetTelemetry();
	}
}

// =============================================================================
// SCORE & HIGH SCORE SYSTEM
// =============================================================================

void UVelocityGameInstance::AddScore(int64 Points)
{
	if (Points <= 0)
	{
		return;
	}

	CurrentScore += Points;
	OnScoreChanged.Broadcast(CurrentScore, Points);
}

void UVelocityGameInstance::ResetScore()
{
	CurrentScore = 0;
	OnScoreChanged.Broadcast(0, 0);
}

bool UVelocityGameInstance::IsHighScore(int64 InScore) const
{
	if (InScore <= 0)
	{
		return false;
	}

	if (HighScores.Num() < MaxSavedHighScores)
	{
		return true;
	}

	return InScore > HighScores.Last().Score;
}

int64 UVelocityGameInstance::GetTopScore() const
{
	if (HighScores.Num() > 0)
	{
		return HighScores[0].Score;
	}
	return 0;
}

bool UVelocityGameInstance::SubmitHighScore(const FString& InPlayerName, const FString& InCarUsed, float InBestLap)
{
	if (!IsHighScore(CurrentScore))
	{
		return false;
	}

	FHighScoreEntry NewEntry;
	NewEntry.PlayerName = InPlayerName.IsEmpty() ? TEXT("Driver") : InPlayerName;
	NewEntry.Score = CurrentScore;
	NewEntry.CarUsed = InCarUsed.IsEmpty() ? TEXT("Supercar") : InCarUsed;
	NewEntry.BestLapTime = InBestLap > 0.0f ? InBestLap : GetBestLapTime();
	NewEntry.Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M"));

	HighScores.Add(NewEntry);

	// Sort high scores descending by score
	HighScores.Sort([](const FHighScoreEntry& A, const FHighScoreEntry& B) {
		return A.Score > B.Score;
	});

	// Find assigned rank (1-indexed)
	int32 Rank = HighScores.IndexOfByPredicate([&](const FHighScoreEntry& Item) {
		return Item.Score == NewEntry.Score && Item.Timestamp == NewEntry.Timestamp;
	}) + 1;

	// Trim to max limit
	if (HighScores.Num() > MaxSavedHighScores)
	{
		HighScores.SetNum(MaxSavedHighScores);
	}

	SaveHighScoresToDisk();

	OnNewHighScore.Broadcast(NewEntry, Rank);
	UE_LOG(LogTemp, Display, TEXT("HIGH SCORE RECORDED! Rank #%d: %s with %lld points"), Rank, *NewEntry.PlayerName, NewEntry.Score);

	return true;
}

void UVelocityGameInstance::ClearHighScores()
{
	HighScores.Empty();
	SaveHighScoresToDisk();
}

FString UVelocityGameInstance::GetHighScoreFilePath() const
{
	return FPaths::ProjectSavedDir() / TEXT("SaveGames") / TEXT("HighScores.json");
}

bool UVelocityGameInstance::SaveHighScoresToDisk()
{
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> ScoresArray;

	for (const FHighScoreEntry& Entry : HighScores)
	{
		TSharedRef<FJsonObject> EntryObject = MakeShared<FJsonObject>();
		EntryObject->SetStringField(TEXT("PlayerName"), Entry.PlayerName);
		EntryObject->SetNumberField(TEXT("Score"), Entry.Score);
		EntryObject->SetNumberField(TEXT("BestLapTime"), Entry.BestLapTime);
		EntryObject->SetStringField(TEXT("CarUsed"), Entry.CarUsed);
		EntryObject->SetStringField(TEXT("Timestamp"), Entry.Timestamp);

		ScoresArray.Add(MakeShared<FJsonValueObject>(EntryObject));
	}

	RootObject->SetArrayField(TEXT("HighScores"), ScoresArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (!FJsonSerializer::Serialize(RootObject, Writer))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to serialize HighScores JSON."));
		return false;
	}

	FString SavePath = GetHighScoreFilePath();
	bool bSuccess = FFileHelper::SaveStringToFile(OutputString, *SavePath);
	if (bSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("HighScores saved successfully to %s"), *SavePath);
	}
	return bSuccess;
}

bool UVelocityGameInstance::LoadHighScoresFromDisk()
{
	FString SavePath = GetHighScoreFilePath();
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *SavePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("No HighScores file at %s, initializing defaults."), *SavePath);
		return false;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize HighScores JSON."));
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* ScoresJson;
	if (RootObject->TryGetArrayField(TEXT("HighScores"), ScoresJson))
	{
		HighScores.Empty();
		for (const TSharedPtr<FJsonValue>& Value : *ScoresJson)
		{
			TSharedPtr<FJsonObject> EntryObject = Value->AsObject();
			if (EntryObject.IsValid())
			{
				FHighScoreEntry Entry;
				Entry.PlayerName = EntryObject->GetStringField(TEXT("PlayerName"));
				Entry.Score = static_cast<int64>(EntryObject->GetNumberField(TEXT("Score")));
				Entry.BestLapTime = EntryObject->GetNumberField(TEXT("BestLapTime"));
				Entry.CarUsed = EntryObject->GetStringField(TEXT("CarUsed"));
				Entry.Timestamp = EntryObject->GetStringField(TEXT("Timestamp"));
				HighScores.Add(Entry);
			}
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Loaded %d high scores from %s"), HighScores.Num(), *SavePath);
	return true;
}
