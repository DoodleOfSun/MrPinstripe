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

	bool IsJumpedForWallRunning = false; // 점프 함수 여러번 호출되는 것을 방지

	AActor* CurrentCoverActor;
	AActor* CurrentWallRunningActor;
	FVector CurrentWallImpactNormal;

	float MoveTime = 0.f;
	float CoverMoveInterval = 10.f;	// 10초마다 엄폐물 위치를 바꾼다

	float ChaseMoveInterval = 3.f; // 전문가형 적이 플레이어를 추적할 때 이동하는 간격

	void EnemyMovingLogic(float DeltaTime);

public:
	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	void InitMoving();

	void MoveToNextCoverLocation();

	void MoveForExpert();

	void JumpToWall();
	
	void WallRunning();

	UFUNCTION(BlueprintCallable)
	void WallJumping();

	void DebugFindPath(AActor* FromActor, AActor* ToActor);


	UPROPERTY(BlueprintReadWrite)
	int DetectedWallSign = 0;

};
