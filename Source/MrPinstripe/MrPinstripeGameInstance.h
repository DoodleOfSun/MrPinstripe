// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MrPinstripeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MRPINSTRIPE_API UMrPinstripeGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	UMrPinstripeGameInstance();

	virtual void Init() override;

public:

	UFUNCTION(BlueprintCallable)
	void SaveWeaponSetting();

	UFUNCTION(BlueprintCallable)
	void LoadWeaponSetting();

	UPROPERTY(BlueprintReadWrite)
	float BGMVolume;

	UPROPERTY(BlueprintReadWrite)
	float SFXVolume;

	UPROPERTY(BlueprintReadWrite)
	float BrightnessFloat;

	UPROPERTY(BlueprintReadWrite)
	FString ResolutionStr;

	UPROPERTY(BlueprintReadWrite)
	FString ScreenModeStr;

	UPROPERTY(BlueprintReadWrite)
	bool isCrossHairVisible;

	// 플레이어가 사망해도 인스턴스에서 데이터를 갖고 있어야 하는 정보들
	// 1. 진짜로 이 무기를 가지고 있는지에 대한 bool 변수
	// 2. 각 무기들의 비축한 탄환 개수 Integer
	
	UPROPERTY(BlueprintReadWrite)
	bool IsPlayerHavePistol = false;

	UPROPERTY(BlueprintReadWrite)
	bool IsPlayerHaveSMG = false;

	UPROPERTY(BlueprintReadWrite)
	bool IsPlayerHaveRifle = false;

	UPROPERTY(BlueprintReadWrite)
	bool IsPlayerHaveShotgun = false;

	UPROPERTY(BlueprintReadWrite)
	int GatheredAmmo_Pistol = 0;
	UPROPERTY(BlueprintReadWrite)
	int GatheredAmmo_SMG = 0;
	UPROPERTY(BlueprintReadWrite)
	int GatheredAmmo_Rifle = 0;
	UPROPERTY(BlueprintReadWrite)
	int GatheredAmmo_Shotgun = 0;


};
