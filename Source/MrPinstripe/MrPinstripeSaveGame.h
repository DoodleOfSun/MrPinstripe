// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponStructClass.h"
#include "GameFramework/SaveGame.h"
#include "MrPinstripeSaveGame.generated.h"

/**
 * // 이윤형 : 구조체 사용하라
 */// C++ 교수님 : 
UCLASS()
class MRPINSTRIPE_API UMrPinstripeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

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

	// 테스트가 필요한 구조체 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponStruct SavedWeaponData;

};
