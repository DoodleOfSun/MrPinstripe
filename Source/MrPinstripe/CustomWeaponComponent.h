// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "CustomWeaponComponent.generated.h"

class AMrPinstripeCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MRPINSTRIPE_API UCustomWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCustomWeaponComponent();



	///** Projectile class to spawn */
	//UPROPERTY(EditDefaultsOnly, Category = Projectile)
	//TSubclassOf<class AMrPinstripeProjectile> ProjectileClass;

	// 스폰시킬 총구 화염 나이아가라 시스템
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* MuzzleFlameFX;

	// 총구 화염 나이아가라 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* MuzzleFlameComponent;

	// 무기를 던질 때 시작 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector WeaponThrowingOffset;

	UFUNCTION(BlueprintCallable)
	AMrPinstripeCharacter* GetCharacterByFinding();

	// 무기 발사 로직
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireLogic();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void NormalFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ShotGunFire();

	void RifleFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Aiming();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void HipFire();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EquipedWeaponString;

	UFUNCTION(BlueprintCallable)
	void SetThisWeaponDamage(float Damage);

private :

	float WeaponDamagePerBullet = 0;

	UNiagaraComponent* BulletHitComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CallingEnemyDamageFunc(FHitResult Hit);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Ends gameplay for this component. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void InitWeapon();

	//void ThrowingWeapon();

	void MuzzleFlame();

	void FindingNiagara();

	UFUNCTION(BlueprintCallable)
	void InitMuzzleFlameOffset(FVector OffsetData);

	/** The Character holding this weapon*/
	AMrPinstripeCharacter* Character;

};
