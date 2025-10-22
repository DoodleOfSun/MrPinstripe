// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"


UCLASS()
class MRPINSTRIPE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	AEnemy* ControlledEnemy = nullptr;


private:
	bool IsCover = false;

	AActor* CurrentCoverActor;

	float CoverMoveTime = 0.f;
	float CoverMoveInterval = 10.f;	// 10초마다 엄폐물 위치를 바꾼다

public:
	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnPossess(APawn* InPawn) override;


	/*
	virtual void OnUnPossess() override;
	void BeginAI();
	void StopAI();*/



	virtual void Tick(float DeltaTime) override;

	void MoveToNextCoverLocation();


};
