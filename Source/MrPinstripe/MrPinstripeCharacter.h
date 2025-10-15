// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MrPinstripeCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMrPinstripeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:



	AMrPinstripeCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDashing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrouched;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsSliding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsEquipWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWallRunning;


	UCameraComponent* GetFPSCamera() const { return FPSCamera; }


protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Pause();


	UFUNCTION(BlueprintCallable)
	void CrouchToggle();

	void EnterCrouchState();

	void ExitCrouchState();

	void LerpForCrouch(float DeltaTime);


	UFUNCTION(BlueprintCallable)
	void StartSlide();

	void EndSlide();

	UFUNCTION(BlueprintCallable)
	void PhysSlide(float DeltaTime);

	void FindingWallForRunning(float DeltaTime);
	void WallRunning(FVector WallLocation);
	void TiltWhileWallRunning(float DeltaTime);
	void WallJumping(FVector WallNormal);

protected:
	/** Mesh for Physical Calculating */
	USkeletalMeshComponent* ArmMesh;

	/** Camera for Physical Calculating */
	UCameraComponent* FPSCamera;

public:

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	float CrouchInterpTime;

	float CrouchSpeed;
	float WalkSpeed;
	float DashSpeed;
	float SlideSpeed;

	float StandingZ;
	float CrouchingZ;
	float CurrentZ;

	FVector SlideDir;

	float DefaultFriction;
	float DefaultDeceleration;

	FVector CurrentWallNormal;
	float DetectedWallSign;



};

