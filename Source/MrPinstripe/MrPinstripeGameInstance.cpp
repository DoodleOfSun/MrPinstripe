// Fill out your copyright notice in the Description page of Project Settings.


#include "MrPinstripeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponDataStruct.h"
#include "MrPinstripeSaveGame.h"
#include "Kismet/GameplayStatics.h"

UMrPinstripeGameInstance::UMrPinstripeGameInstance()
{
	//// Initialize default values
	//BGMVolume = 0.5f; // Default background music volume
	//SFXVolume = 0.5f; // Default sound effects volume
	//BrightnessFloat = 1.0f; // Default brightness level
	//ResolutionStr = "1920x1080"; // Default resolution
	//ScreenModeStr = "Fullscreen"; // Default screen mode
}

void UMrPinstripeGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("UMrPinstripeGameInstance::Init() called"));
	//// Initialize default values
	BGMVolume = 0.5f; // Default background music volume
	SFXVolume = 0.5f; // Default sound effects volume
	BrightnessFloat = 0.0f; // Default brightness level
	ResolutionStr = "1600 X 900"; // Default resolution
	ScreenModeStr = "FULLSCREEN"; // Default screen mode

	isCrossHairVisible = true; // Default crosshair visibility
}

void UMrPinstripeGameInstance::SaveWeaponSetting()
{
	// SaveGame 오브젝트 생성
	UMrPinstripeSaveGame* SaveGameInstance = Cast<UMrPinstripeSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->SavedWeaponData = PlayerWeaponData;

		// 실제로 디스크에 저장
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0);
	}
}

void UMrPinstripeGameInstance::LoadWeaponSetting()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
	{
		UMrPinstripeSaveGame* Loaded = Cast<UMrPinstripeSaveGame>(
			UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

		if (Loaded)
		{
			// 구조체 전체 복사
			PlayerWeaponData = Loaded->SavedWeaponData;
		}
	}
}