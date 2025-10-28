// Fill out your copyright notice in the Description page of Project Settings.


#include "WallRunningProxy.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.h"

AWallRunningProxy::AWallRunningProxy()
{
	OnSmartLinkReached.AddDynamic(this, &AWallRunningProxy::StopAndReadyForWallRunning);
}

void AWallRunningProxy::StopAndReadyForWallRunning(AActor* MovingActor, const FVector& DestinationPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("월 러닝 프록시 발동, IsWallRunning을 true로 전환"));
	// AI가 링크에 도달했을 때 벽 달리기 준비
	ACharacter* Character = Cast<ACharacter>(MovingActor);

	// 플래그 변경용
	AEnemy* EnemyCharacter = Cast<AEnemy>(MovingActor);

	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("월 러닝 프록시 발동, IsWallRunning을 true로 전환"));
		Character->GetCharacterMovement()->StopMovementImmediately();
		EnemyCharacter->IsWallRunning = true;
	}
}
