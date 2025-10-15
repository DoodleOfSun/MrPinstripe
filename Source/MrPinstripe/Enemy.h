// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MrPinstripeCharacter.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyCombatState : uint8
{
	Scouting,
	Reloading,
	Jump,
	Crouch,
	WallRunning,
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

	UFUNCTION(BlueprintCallable)
	void Scouting(FVector ScoutPos);

	UFUNCTION(BlueprintCallable)
	void Standing();

	UFUNCTION(BlueprintCallable)
	void CustomEnemyCrouch();

	UFUNCTION(BlueprintCallable)
	void Moving();

	UFUNCTION(BlueprintCallable)
	void FindingPlayerAndFocus();

	UFUNCTION(BlueprintCallable)
	void Firing();

	UFUNCTION(BlueprintCallable)
	float GetCurrentVelocity();

	UFUNCTION(BlueprintCallable)
	void WallRunning();

	UFUNCTION(BlueprintCallable)
	void Damaged();

	UFUNCTION(BlueprintCallable)
	void Die();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FindingCoverObject(float DeltaTime);

	void FindingWallForWallRunning(float DeltaTime);

	void DetectingPlayerByDistance(float DeltaTime);

	void CaculatingAimOffsetRotation(float DeltaTime);

	// 멤버 변수는 여기에 선언
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyCombatState EnemyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponTypeFStr;

	bool IsFindCoverObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWallRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDetectedPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AimOffsetVector;

};
