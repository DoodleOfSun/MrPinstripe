// Fill out your copyright notice in the Description page of Project Settings.


#include "MrPinstripeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponStructClass.h"
#include "MrPinstripeSaveGame.h"

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

// 현재 무기 상태를 저장
void UMrPinstripeGameInstance::SaveWeaponSetting()
{
	UMrPinstripeSaveGame* SaveGameInstance = Cast<UMrPinstripeSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMrPinstripeSaveGame::StaticClass()));

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveWeaponSetting: SaveGameInstance 생성 실패"));
		return;
	}

	SaveGameInstance->SavedWeaponData = PlayerWeaponStruct;

	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Log, TEXT("SaveWeaponSetting: 저장 성공"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveWeaponSetting: 저장 실패"));
	}
}

// 현재 무기 상태를 불러옴
void UMrPinstripeGameInstance::LoadWeaponSetting()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadWeaponSetting: 저장 슬롯이 없습니다."));
		return;
	}

	UMrPinstripeSaveGame* LoadedGame = Cast<UMrPinstripeSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

	if (!LoadedGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadWeaponSetting: Load 실패"));
		return;
	}

	PlayerWeaponStruct = LoadedGame->SavedWeaponData;

	UE_LOG(LogTemp, Log, TEXT("LoadWeaponSetting: 데이터 로드 완료"));
}



// 권총의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHavePistol() {
	return PlayerWeaponStruct.IsPlayerHavePistol;
}
void UMrPinstripeGameInstance::SetIsPlayerHavePistol(bool Input) {
	PlayerWeaponStruct.IsPlayerHavePistol = Input;
}

// SMG의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHaveSMG() {
	return PlayerWeaponStruct.IsPlayerHaveSMG;
}

void UMrPinstripeGameInstance::SetIsPlayerHaveSMG(bool Input) {
	PlayerWeaponStruct.IsPlayerHaveSMG = Input;
}

// 소총의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHaveRifle() {
	return PlayerWeaponStruct.IsPlayerHaveRifle;
}

void UMrPinstripeGameInstance::SetIsPlayerHaveRifle(bool Input) {
	PlayerWeaponStruct.IsPlayerHaveRifle = Input;
}

// 샷건의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHaveShotgun() {
	return PlayerWeaponStruct.IsPlayerHaveShotgun;
}

void UMrPinstripeGameInstance::SetIsPlayerHaveShotgun(bool Input) {
	PlayerWeaponStruct.IsPlayerHaveShotgun = Input;
}

// 모은 권총의 탄환
int UMrPinstripeGameInstance::GetGatheredAmmo_Pistol() {
	return PlayerWeaponStruct.GatheredAmmo_Pistol;
}

void UMrPinstripeGameInstance::SetGatheredAmmo_Pistol(int data) {
	PlayerWeaponStruct.GatheredAmmo_Pistol = data;
}

// 모은 SMG의 탄환 
int UMrPinstripeGameInstance::GetGatheredAmmo_SMG() {
	return PlayerWeaponStruct.GatheredAmmo_SMG;
}
void UMrPinstripeGameInstance::SetGatheredAmmo_SMG(int data) {
	PlayerWeaponStruct.GatheredAmmo_SMG = data;
}

// 모은 소총의 탄환
int UMrPinstripeGameInstance::GetGatheredAmmo_Rifle() {
	return PlayerWeaponStruct.GatheredAmmo_Rifle;
}
void UMrPinstripeGameInstance::SetGatheredAmmo_Rifle(int data) {
	PlayerWeaponStruct.GatheredAmmo_Rifle = data;
}

// 모은 샷건의 탄환
int UMrPinstripeGameInstance::GetGatheredAmmo_Shotgun() {
	return PlayerWeaponStruct.GatheredAmmo_Shotgun;
}
void UMrPinstripeGameInstance::SetGatheredAmmo_Shotgun(int data) {
	PlayerWeaponStruct.GatheredAmmo_Shotgun = data;
}