// Copyright Epic Games, Inc. All Rights Reserved.


#include "MrPinstripeWeaponComponent.h"
#include "MrPinstripeCharacter.h"
#include "MrPinstripeProjectile.h"
#include "MrPinstripeGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"


// Sets default values for this component's properties
UMrPinstripeWeaponComponent::UMrPinstripeWeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	WeaponThrowingOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UMrPinstripeWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

// TODO : 카메라의 GetComponentLocation이라는 위치를 사용하고 있는데, 이 위치가 잘못된 위치일 확률이 높음. 다시 확인해야 함
void UMrPinstripeWeaponComponent::Fire()
{
	AMrPinstripeGameMode* GM = Cast<AMrPinstripeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (Character == nullptr || Character->GetController() == nullptr || GM->MyCustomState == EGameState::GamePaused)
	{
		UE_LOG(LogTemp, Warning, TEXT("현재 발사 불가. 스테이트는 : %d, 1이면 Pause인 상태임."), GM->MyCustomState);
		return;
	}

	// 히트스캔 로직. 정조준과 지향사격으로 나뉜다.
	// 정조준의 경우
	if (IsAiming) {

		FHitResult Hit;
		UCameraComponent* cam = Character->GetFirstPersonCameraComponent();
		FVector StartTrace = cam->GetComponentLocation();
		FVector EndTrace = StartTrace + (cam->GetForwardVector() * 10000);
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 5.0f);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility);

		if (Hit.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());
		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("정조준을 하고 쏘았으나 GetActor가 nullptr임."));
		}
	}

	// 지향사격의 경우
	else {

		FHitResult Hit;
		UCameraComponent* cam = Character->GetFirstPersonCameraComponent();
		FVector StartTrace = cam->GetComponentLocation();


		float SpreadAngle = 5.0f; // 퍼짐 각도 (도 단위)
		FVector RandomSpread = FMath::VRandCone(cam->GetForwardVector(), FMath::DegreesToRadians(SpreadAngle));


		FVector EndTrace = StartTrace + (RandomSpread * 10000);
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 5.0f);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility);

		if (Hit.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());
		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("지향사격을 하고 쏘았으나 GetActor가 nullptr임."));
		}
	}
}

// 샷건 팰릿 구현
void UMrPinstripeWeaponComponent::ShotGunFire()
{
	TArray<FHitResult> HitResultList;
	HitResultList.Reserve(8);

	UCameraComponent* cam = Character->GetFirstPersonCameraComponent();
	FVector StartTrace = cam->GetComponentLocation();

	float SpreadAngle = 5.0f; // 퍼짐 각도 (도 단위)

	for (int i = 0; i < 8; i++)
	{
		FVector RandomSpread = FMath::VRandCone(cam->GetForwardVector(), FMath::DegreesToRadians(SpreadAngle));

		FVector EndTrace = StartTrace + (RandomSpread * 10000);
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 5.0f);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility);

		if (Hit.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("샷건 발사 : Hit Actor: %s"), *Hit.GetActor()->GetName());
		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("샷건 발사 : 지향사격을 하고 쏘았으나 GetActor가 nullptr임."));
		}
		HitResultList.Add(Hit);
	}
}

void UMrPinstripeWeaponComponent::Aiming()
{
	if (!IsAiming)
	{
		IsAiming = true;
	}
}

void UMrPinstripeWeaponComponent::HipFire()
{
	IsAiming = false;
}

AMrPinstripeCharacter* UMrPinstripeWeaponComponent::GetCharacterByFinding()
{
	// Try to find the character by getting the player character from the world
	AMrPinstripeCharacter* MyCharacter = Cast<AMrPinstripeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (MyCharacter != nullptr)
	{
		return MyCharacter;
	}
	return nullptr;
}

// 무기를 던진다. this->Destroy를 제외하면 언리얼에서 기본 제공하는 투사체 로직임.
// 이 함수 자체로는 어떤 Projectile을 발사하기만 하고, 무기 해제 로직은 블루프린트에 있다.
void UMrPinstripeWeaponComponent::ThrowingWeapon()
{
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(WeaponThrowingOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<AMrPinstripeProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			DestroyComponent();
		}
	}
}

void UMrPinstripeWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}

