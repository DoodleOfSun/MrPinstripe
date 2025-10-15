// Fill out your copyright notice in the Description page of Project Settings.


#include "MrPinstripeGameInstance.h"

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