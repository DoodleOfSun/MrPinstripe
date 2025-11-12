// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CheckPointPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class MRPINSTRIPE_API ACheckPointPlayerStart : public APlayerStart
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

private:
	FVector CheckPointLocation;
	void MovePostionToCheckPoint(FVector PosData);
};
