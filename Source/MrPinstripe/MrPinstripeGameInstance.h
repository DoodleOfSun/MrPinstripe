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
};
