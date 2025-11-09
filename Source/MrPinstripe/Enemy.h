// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Sound//SoundCue.h"
#include "GameFramework/Character.h"
#include "MrPinstripeCharacter.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyCombatState : uint8
{
	Scouting,
	Reloading,
	Jump,
	Falling,
	Crouch,
	WallRunning,
	Hit,
	Die
};

UCLASS()
class MRPINSTRIPE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	AMrPinstripeCharacter* TargetPlayerCharacter;

	UFUNCTION(BlueprintCallable)
	void Init();

	void AnimationControl();

	void Scouting();

	void Jump();

	UFUNCTION(BlueprintCallable)
	void Standing();

	UFUNCTION(BlueprintCallable)
	void CustomEnemyCrouch();

	UFUNCTION(BlueprintCallable)
	void Moving();

	UFUNCTION(BlueprintCallable)
	void FindingPlayerAndFocus();

	UFUNCTION(BlueprintCallable)
	void Firing(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	float GetCurrentVelocity();

	void Falling();

	void WallRunning();

	UFUNCTION(BlueprintCallable)
	void Damaged(float ReceivedDamage);

	UFUNCTION(BlueprintCallable)
	void Die();

	// 총구 화염 나이아가라 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* MuzzleFlameComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FindingCoverObject(float DeltaTime);

	void FindingWallForWallRunning(float DeltaTime);

	void DetectingPlayerByDistance(float DeltaTime);

	void CaculatingAimOffsetRotation(float DeltaTime);

	void TrackingPlayerByLineTrace();

	void FindingNiagara();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyCombatState EnemyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int OriginAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponTypeFStr;

	bool IsFindCoverObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWallRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDetectedPlayer;	// 플레이어가 일정 거리에 들어오면 true로 전환, 이동과 공격 여부 확인에 사용

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AimOffsetVector;

	bool IsReadyToShot;	// 플레이어와 적 사이에 장애물이 없으면 발포하게 해주는 플래그, IsDetectedPlayer가 True일 때만 이를 검사


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundCue* FireSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundCue* MoveSoundCue;

	UAudioComponent* MoveAudioComponent;

	bool IsWallRunningForAnimation;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* BulletLineComponent;


private:
	int CurrentAmmo;

	float FireRateTiming;	// FireTime이 이 변수 이상이 되면 총을 발사한다.

	AActor* CurrentCoverActor;

	int MissBullet;

	float TrackingTraceTime;

	EEnemyCombatState LastCombatState;

	bool IsPlayerBehind;
};
