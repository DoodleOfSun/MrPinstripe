// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation() {
	EnemyInstance = Cast<AEnemy>(TryGetPawnOwner());
	UE_LOG(LogTemp,Warning, TEXT("애님실행됨"));
}

float UEnemyAnimInstance::GetCharacterVelocity() {
	if (EnemyInstance) {
		return EnemyInstance->GetCurrentVelocity();
	}

	else {
		return 0;
	}
}

EEnemyCombatState UEnemyAnimInstance::GetEnemyCombatState() {

	if (EnemyInstance) {
		return EnemyInstance->EnemyState;
	}

	else {
		return EEnemyCombatState::Scouting;
	}
}

void UEnemyAnimInstance::SetCharacterCombatState(EEnemyCombatState StateData)
{
	EnemyInstance->EnemyState = StateData;
}