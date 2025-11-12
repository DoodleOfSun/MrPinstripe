

#pragma once
#include "CoreMinimal.h"

#include "WeaponStructClass.generated.h"

/**
 * 
 */
class MRPINSTRIPE_API WeaponStructClass
{
public:
	WeaponStructClass();
	~WeaponStructClass();
};


USTRUCT(Atomic, BlueprintType)
struct FWeaponStruct
{
	GENERATED_BODY()

	// 플레이어가 사망해도 인스턴스에서 데이터를 갖고 있어야 하는 정보들
	// 1. 진짜로 이 무기를 가지고 있는지에 대한 bool 변수
	// 2. 각 무기들의 비축한 탄환 개수 Integer

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHavePistol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHaveSMG = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHaveRifle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerHaveShotgun = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_Pistol = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_SMG = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_Rifle = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GatheredAmmo_Shotgun = 0;
};
