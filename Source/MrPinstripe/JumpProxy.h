// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "JumpProxy.generated.h"

/**
 * 
 */
UCLASS()
class MRPINSTRIPE_API AJumpProxy : public ANavLinkProxy
{
	GENERATED_BODY()

private:
	AJumpProxy();

public:

	UFUNCTION()
	void Jump(AActor* Agent, const FVector& Destination);

};
