// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponStructClass.h"
#include "GameFramework/SaveGame.h"
#include "ViewersStruct.h"
#include "MrPinstripeSaveGame.generated.h"

/**
 * // 동업자 : 구조체 사용하라
 */// 교수님 : 전역 변수 / 공유 메모리를 사용하라
UCLASS()
class MRPINSTRIPE_API UMrPinstripeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	// 테스트가 필요한 구조체 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponStruct SavedWeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FViewersStruct SavedViewersData;
};
