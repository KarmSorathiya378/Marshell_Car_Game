// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VELOCITYXGameInstance.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"

UVELOCITYXGameInstance::UVELOCITYXGameInstance()
{
	ActiveCarType = ECarType::Supercar;

	GraphicsQuality = EGraphicsQuality::High;
	ShadowResolution = 2048;
	TextureQuality = 2;
	bMotionBlur = true;
	bAntiAliasing = true;
	FieldOfView = 90.0f;

	MasterVolume = 1.0f;
	SFXVolume = 1.0f;
	MusicVolume = 0.8f;

	TotalPlayTime = 0.0f;
	CompletedRaces = 0;
	BestLapTimes.Init(0.0f, 3);
}

void UVELOCITYXGameInstance::Init()
{
	Super::Init();

	if (!LoadGameData())
	{
		ApplyDefaultSettings();
	}
}

void UVELOCITYXGameInstance::Shutdown()
{
	SaveGameData();
	Super::Shutdown();
}

void UVELOCITYXGameInstance::SetSelectedCar(ECarType NewCarType)
{
	ActiveCarType = NewCarType;
	UE_LOG(LogTemp, Display, TEXT("Active car set in GameInstance: %d"), static_cast<int32>(ActiveCarType));
}

void UVELOCITYXGameInstance::ApplyDefaultSettings()
{
	GraphicsQuality = EGraphicsQuality::High;
	ShadowResolution = 2048;
	TextureQuality = 2;
	bMotionBlur = true;
	bAntiAliasing = true;
	FieldOfView = 90.0f;

	MasterVolume = 1.0f;
	SFXVolume = 1.0f;
	MusicVolume = 0.8f;

	UE_LOG(LogTemp, Display, TEXT("VelocityX default settings applied."));
}

FString UVELOCITYXGameInstance::GetSaveFilePath() const
{
	return FPaths::ProjectSavedDir() / TEXT("SaveGames") / TEXT("VelocityX_Save.json");
}

bool UVELOCITYXGameInstance::SaveGameData()
{
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();

	// Settings sub-object
	TSharedRef<FJsonObject> SettingsObject = MakeShared<FJsonObject>();
	SettingsObject->SetNumberField(TEXT("GraphicsQuality"), static_cast<int32>(GraphicsQuality));
	SettingsObject->SetNumberField(TEXT("ShadowResolution"), ShadowResolution);
	SettingsObject->SetNumberField(TEXT("TextureQuality"), TextureQuality);
	SettingsObject->SetBoolField(TEXT("MotionBlur"), bMotionBlur);
	SettingsObject->SetBoolField(TEXT("AntiAliasing"), bAntiAliasing);
	SettingsObject->SetNumberField(TEXT("FieldOfView"), FieldOfView);
	SettingsObject->SetNumberField(TEXT("MasterVolume"), MasterVolume);
	SettingsObject->SetNumberField(TEXT("SFXVolume"), SFXVolume);
	SettingsObject->SetNumberField(TEXT("MusicVolume"), MusicVolume);
	RootObject->SetObjectField(TEXT("Settings"), SettingsObject);

	// Progress sub-object
	TSharedRef<FJsonObject> ProgressObject = MakeShared<FJsonObject>();
	ProgressObject->SetNumberField(TEXT("SelectedCarIndex"), static_cast<int32>(ActiveCarType));
	ProgressObject->SetNumberField(TEXT("TotalPlayTime"), TotalPlayTime);
	ProgressObject->SetNumberField(TEXT("CompletedRaces"), CompletedRaces);

	TArray<TSharedPtr<FJsonValue>> LapTimesArray;
	for (float Time : BestLapTimes)
	{
		LapTimesArray.Add(MakeShared<FJsonValueNumber>(Time));
	}
	ProgressObject->SetArrayField(TEXT("BestLapTimes"), LapTimesArray);
	RootObject->SetObjectField(TEXT("Progress"), ProgressObject);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (!FJsonSerializer::Serialize(RootObject, Writer))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to serialize save game json."));
		return false;
	}

	FString SavePath = GetSaveFilePath();
	bool bSuccess = FFileHelper::SaveStringToFile(OutputString, *SavePath);
	if (bSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Save game written successfully to %s"), *SavePath);
	}
	return bSuccess;
}

bool UVELOCITYXGameInstance::LoadGameData()
{
	FString SavePath = GetSaveFilePath();
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *SavePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("No save file located at %s, using defaults."), *SavePath);
		return false;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize save game json from %s."), *SavePath);
		return false;
	}

	if (RootObject->HasTypedField<EJson::Object>(TEXT("Settings")))
	{
		TSharedPtr<FJsonObject> SettingsObject = RootObject->GetObjectField(TEXT("Settings"));
		GraphicsQuality = static_cast<EGraphicsQuality>(SettingsObject->GetIntegerField(TEXT("GraphicsQuality")));
		ShadowResolution = SettingsObject->GetIntegerField(TEXT("ShadowResolution"));
		TextureQuality = SettingsObject->GetIntegerField(TEXT("TextureQuality"));
		bMotionBlur = SettingsObject->GetBoolField(TEXT("MotionBlur"));
		bAntiAliasing = SettingsObject->GetBoolField(TEXT("AntiAliasing"));
		FieldOfView = SettingsObject->GetNumberField(TEXT("FieldOfView"));
		MasterVolume = SettingsObject->GetNumberField(TEXT("MasterVolume"));
		SFXVolume = SettingsObject->GetNumberField(TEXT("SFXVolume"));
		MusicVolume = SettingsObject->GetNumberField(TEXT("MusicVolume"));
	}

	if (RootObject->HasTypedField<EJson::Object>(TEXT("Progress")))
	{
		TSharedPtr<FJsonObject> ProgressObject = RootObject->GetObjectField(TEXT("Progress"));
		ActiveCarType = static_cast<ECarType>(ProgressObject->GetIntegerField(TEXT("SelectedCarIndex")));
		TotalPlayTime = ProgressObject->GetNumberField(TEXT("TotalPlayTime"));
		CompletedRaces = ProgressObject->GetIntegerField(TEXT("CompletedRaces"));

		const TArray<TSharedPtr<FJsonValue>>* LapTimesJson;
		if (ProgressObject->TryGetArrayField(TEXT("BestLapTimes"), LapTimesJson))
		{
			BestLapTimes.Empty();
			for (const TSharedPtr<FJsonValue>& Value : *LapTimesJson)
			{
				BestLapTimes.Add(Value->AsNumber());
			}
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Save game successfully loaded from %s"), *SavePath);
	return true;
}
