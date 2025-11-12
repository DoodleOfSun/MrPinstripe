// Fill out your copyright notice in the Description page of Project Settings.


#include "MrPinstripeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponStructClass.h"

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

}

void UMrPinstripeGameInstance::LoadWeaponSetting() {

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