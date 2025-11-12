// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MRPINSTRIPE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:

	virtual void NativeInitializeAnimation() override;


public :

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AEnemy* EnemyInstance;

	UFUNCTION(BlueprintCallable)
	float GetCharacterVelocity();

	UFUNCTION(BlueprintCallable)
	void SetCharacterCombatState(EEnemyCombatState StateData);

	UFUNCTION(BlueprintCallable)
	EEnemyCombatState GetEnemyCombatState();

};
