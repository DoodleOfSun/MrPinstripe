// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WeaponDataStruct.h"
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
	// TODO : 개별 전역 변수 선언 남발은 확장성과 가독성이 매우 떨어짐. 구조체로 변경하여 리팩토링 할 필요 있음.
	// 또한 리팩토링 시 반드시 각 구조체 멤버를 각각 초기화해주는 함수를 선언해야 함
	// 이유는 BP에서 노드 연결이 굉장히 복잡하고 불편하기 때문. - 오징어같은 BP는 최대한 피해야 한다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHavePistol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHaveSMG = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHaveRifle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHaveShotgun = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_Pistol = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_SMG = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_Rifle = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_Shotgun = 0;

	// 테스트가 필요한 구조체 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponStruct PlayerWeaponStruct;

	// 각 구조체 변수들을 반환 및 초기화하는 함수
	// NOTE : 여기 항목들은 구조체에 멤버가 새로 추가될때마다 Get과 Set 함수를 매번 구현해야 한다. excel같은 db를 활용한다면 좀 더 간단하고 많은 용량을 저장할 수 있으나
	// 거기까지 할 건 없어 보이므로 이렇게만 구현하는 것으로 한다.
	// 또한 이렇게 하지 않으면 BP에서 구조체에 접근하게 되는데, BP에서 구조체로의 접근은 매우 복잡하며 Set을 구현할 때에는 매번 구조체를 덮어씌우므로 성능에 문제가 있다.
	// 또한 리팩토링도 매우 힘들다.
	UFUNCTION(BlueprintCallable)
	bool GetIsPlayerHavePistol();

	UFUNCTION(BlueprintCallable)
	bool GetIsPlayerHaveSMG();

	UFUNCTION(BlueprintCallable);
	bool GetIsPlayerHaveRifle();

	UFUNCTION(BlueprintCallable)
	bool GetIsPlayerHaveShotgun();

};
