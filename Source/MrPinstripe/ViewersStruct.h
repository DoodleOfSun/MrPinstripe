// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ViewersStruct.generated.h"

/**
 * 
 */
class MRPINSTRIPE_API ViewersStruct
{
public:
	ViewersStruct();
	~ViewersStruct();
};

USTRUCT(Atomic, BlueprintType)
struct FViewersStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentViewersCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsLiveStreaming = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle ViewerDecreaseTimerHandle;
};