// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "WallRunningProxy.generated.h"

/**
 * 
 */
UCLASS()
class MRPINSTRIPE_API AWallRunningProxy : public ANavLinkProxy
{
	GENERATED_BODY()

private:
	AWallRunningProxy();

public:

	UFUNCTION()
	void StopAndReadyForWallRunning(AActor* Agent, const FVector& Destination);

};
