// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include "WeaponStructClass.h"
#include "ViewersStruct.h"

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

	FTimerHandle ChapterTimerHandler;

	float CurrentChapterTime;

	float ChapterTimeLimit;

	void ChapterTimerTick();

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

	// 챕터 타이머 시작
	UFUNCTION(BlueprintCallable)
	void StartChapterTimer(float TimeLimit);

	// 타이머 종료 체크
	UFUNCTION(BlueprintCallable)
	bool CheckingTimeOver();

	UPROPERTY(BlueprintReadWrite)
	int CurrentEnemyNumber;

	UFUNCTION(BlueprintCallable)
	bool DecreaseEnemyNumberAndCheckingGameDone();

	// 테스트가 필요한 구조체 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponStruct PlayerWeaponStruct;

	// 시청자 수 구조체 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FViewersStruct PlayerViewersStruct;

	// 각 구조체 변수들을 반환 및 초기화하는 함수
	// NOTE : 여기 항목들은 구조체에 멤버가 새로 추가될때마다 Get과 Set 함수를 매번 구현해야 한다. excel같은 db를 활용한다면 좀 더 간단하고 많은 용량을 저장할 수 있으나
	// 거기까지 할 건 없어 보이므로 이렇게만 구현하는 것으로 한다.
	// 또한 이렇게 하지 않으면 BP에서 구조체에 접근하게 되는데, BP에서 구조체로의 접근은 매우 복잡하며 Set을 구현할 때에는 매번 구조체를 덮어씌우므로 성능에 문제가 있다.
	// 또한 리팩토링도 매우 힘들다.

	// 권총의 소지 여부
	UFUNCTION(BlueprintCallable)
	bool GetIsPlayerHavePistol();

	UFUNCTION(BlueprintCallable)
	void SetIsPlayerHavePistol(bool Input);

	// SMG의 소지 여부
	UFUNCTION(BlueprintCallable)
	bool GetIsPlayerHaveSMG();

	UFUNCTION(BlueprintCallable)
	void SetIsPlayerHaveSMG(bool Input);

	// 소총의 소지 여부
	UFUNCTION(BlueprintCallable)
	bool GetIsPlayerHaveRifle();

	UFUNCTION(BlueprintCallable)
	void SetIsPlayerHaveRifle(bool Input);

	// 샷건의 소지 여부
	UFUNCTION(BlueprintCallable)
	bool GetIsPlayerHaveShotgun();

	UFUNCTION(BlueprintCallable)
	void SetIsPlayerHaveShotgun(bool Input);

	// 모은 권총의 탄환
	UFUNCTION(BlueprintCallable)
	int GetGatheredAmmo_Pistol();

	UFUNCTION(BlueprintCallable)
	void SetGatheredAmmo_Pistol(int data);

	// 모은 SMG의 탄환 
	UFUNCTION(BlueprintCallable)
	int GetGatheredAmmo_SMG();
	UFUNCTION(BlueprintCallable)
	void SetGatheredAmmo_SMG(int data);

	// 모은 소총의 탄환
	UFUNCTION(BlueprintCallable)
	int GetGatheredAmmo_Rifle();
	UFUNCTION(BlueprintCallable)
	void SetGatheredAmmo_Rifle(int data);

	// 모은 샷건의 탄환
	UFUNCTION(BlueprintCallable)
	int GetGatheredAmmo_Shotgun();
	UFUNCTION(BlueprintCallable)
	void SetGatheredAmmo_Shotgun(int data);

	// 현재 들고 있는 무기의 이름
	UFUNCTION(BlueprintCallable)
	FString GetWeaponTypeStr();
	UFUNCTION(BlueprintCallable)
	void SetWeaponTypeStr(FString data);


	// 시청자 시스템
	// 방송 시작 / 종료
	UFUNCTION(BlueprintCallable)
	void SetStreaming(bool data);

	// 시청자 수는 시간에 따라 점점 줄어듬
	UFUNCTION(BlueprintCallable)
	void DecreaseViewersNumbersByTime();

	// 특정한 값을 더해 시청자 수를 늘림
	UFUNCTION(BlueprintCallable)
	void IncreaseViewersNumbers(int data);

	// 특정한 값을 빼 시청자 수를 줄임
	UFUNCTION(BlueprintCallable)
	void DecreaseViewersNumbers(int data);

	// 특정한 값을 입력해 시청자 수를 설정
	UFUNCTION(BlueprintCallable)
	void SetViewersNumbers(int data);
	
	// getter
	UFUNCTION(BlueprintCallable)
	int GetViewersNumbers();
	UFUNCTION(BlueprintCallable)
	bool GetIsLiveStreaming();
};