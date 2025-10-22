// Copyright Epic Games, Inc. All Rights Reserved.

#include "MrPinstripeCharacter.h"
#include "MrPinstripeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "MrPinstripeGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMrPinstripeCharacter

AMrPinstripeCharacter::AMrPinstripeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void AMrPinstripeCharacter::BeginPlay()
{

	TArray<UCameraComponent*> CameraComponents;
	GetComponents<UCameraComponent>(CameraComponents);

	for (UCameraComponent* FindingCam : CameraComponents)
	{
		if (FindingCam->GetName() == TEXT("FPSCamera"))
		{
			FPSCamera = FindingCam;
			break;
		}
	}

	TArray<USkeletalMeshComponent*> MeshComponents;
	GetComponents<USkeletalMeshComponent>(MeshComponents);

	for (USkeletalMeshComponent* FindingMesh : MeshComponents)
	{
		if (FindingMesh->GetName() == TEXT("CharacterMesh0"))
		{
			ArmMesh = FindingMesh;
			break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("My Name is %s"), *FPSCamera->GetName());
	UE_LOG(LogTemp, Warning, TEXT("My Name is %s"), *ArmMesh->GetName());

	// Call the base class  
	Super::BeginPlay();

	IsCrouched = false;
	IsDashing = false;
	IsSliding = false;
	IsReloading = false;
	IsEquipWeapon = false;
	IsWallRunning = false;

	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	CrouchSpeed = GetCharacterMovement()->MaxWalkSpeed * 0.5f;
	DashSpeed = GetCharacterMovement()->MaxWalkSpeed * 2.f;
	SlideSpeed = 1000.f;

	CrouchInterpTime = 8.f;
	DefaultFriction = GetCharacterMovement()->GroundFriction;
	DefaultDeceleration = GetCharacterMovement()->BrakingDecelerationWalking;

	StandingZ = -90.f;
	CrouchingZ = -150.f;
	CurrentZ = StandingZ;

	CurrentWallNormal = FVector(0.f, 0.f, 0.f);
	DetectedWallSign = 0;
}


void AMrPinstripeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PhysSlide(DeltaTime);
	LerpForCrouch(DeltaTime);
	FindingWallForRunning(DeltaTime);
	TiltWhileWallRunning(DeltaTime);
}

void AMrPinstripeCharacter::Pause()
{
	// Pause the game

	UE_LOG(LogTemplateCharacter, Warning, TEXT("Pause action triggered. Implement pause functionality here."));

	AMrPinstripeGameMode* GM = Cast<AMrPinstripeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM != nullptr)
	{
		GM->Pause();
	}
}

void AMrPinstripeCharacter::CrouchToggle()
{
	if (IsCrouched && !IsSliding)
	{
		UE_LOG(LogTemp, Warning, TEXT("웅크리기 도중이고 슬라이딩이 아니므로 일어선다"));
		ExitCrouchState();
	}
	else if (!IsCrouched && !IsDashing)
	{
		UE_LOG(LogTemp, Warning, TEXT("일이선 상태고 대쉬 상태가 아니므로 웅크린다"));
		EnterCrouchState();
	}

	else if (!IsCrouched && IsDashing)
	{
		UE_LOG(LogTemp, Warning, TEXT("일이선 상태고 대쉬 상태가 아니므로 웅크린다"));
		StartSlide();
	}

	else if (!IsCrouched && IsDashing)
	{
		UE_LOG(LogTemp, Warning, TEXT("대쉬는 true인데 웅크리기는 false이다."));
		EnterCrouchState();
		//IsCrouched = true;
	}

	else if (IsSliding && IsCrouched)
	{
		UE_LOG(LogTemp, Warning, TEXT("일어서기 키로 슬라이드 도중 강제종료"));

		GetCharacterMovement()->GroundFriction = DefaultFriction;
		GetCharacterMovement()->BrakingDecelerationWalking = DefaultDeceleration;
		IsSliding = false;
		ExitCrouchState();
	}
}

void AMrPinstripeCharacter::EnterCrouchState()
{
	IsCrouched = true;
}

void AMrPinstripeCharacter::ExitCrouchState()
{
	IsCrouched = false;
}

void AMrPinstripeCharacter::LerpForCrouch(float DeltaTime)
{
	// 새 로직
	// 웅크리기가 True 일 때는 웅크려야 한다. 해당 위치까지 ArmMesh를 보간한다.
	FVector CurrentCamLocation = FVector(0, 0, CurrentZ);

	float TargetZ = IsCrouched ? CrouchingZ : StandingZ;

	FVector TargetLocation = FVector(0, 0, TargetZ);
	FVector InterpedLocation = FMath::VInterpTo(CurrentCamLocation, TargetLocation, DeltaTime, CrouchInterpTime);

	ArmMesh->SetRelativeLocation(InterpedLocation);
	CurrentZ = InterpedLocation.Z;
}

void AMrPinstripeCharacter::StartSlide()
{
	// 슬라이드 시작 시 호출되는 함수
	SlideDir = GetActorForwardVector(); // 슬라이드가 시작될 때 앞 방향
	EnterCrouchState();
	IsSliding = true;
}

void AMrPinstripeCharacter::PhysSlide(float DeltaTime)
{
	if (IsSliding)
	{
		UE_LOG(LogTemp,Warning, TEXT("현재 슬라이딩 발동중"));
		// 슬라이드 종료를 위한 검사용 변수
		float ProceedSpeed = GetVelocity().Size();

		FVector Start = GetActorLocation();
		FVector End = Start + GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
		FName ProfileName = TEXT("BlockAll");

		FHitResult Hit;

		FCollisionQueryParams Params(FName(TEXT("SlideTrace")), true, this);
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, Params);


		if (bHit)
		{
			FVector SurfaceNormal = Hit.ImpactNormal;

			// 경사도 계산
			float SurfaceAngleDegrees = FMath::RadiansToDegrees(
				FMath::Acos(FVector::DotProduct(SurfaceNormal, FVector::UpVector))
			);

			// 경사면 방향 (법선의 반대 방향을 수직으로 투영)
			FVector SlopeDirection = FVector::VectorPlaneProject(SurfaceNormal, FVector::UpVector).GetSafeNormal();

			// 캐릭터 이동 방향
			FVector MovementDirection = GetVelocity().GetSafeNormal();

			// 내적을 통해 방향 비교
			float Dot = FVector::DotProduct(SlopeDirection, MovementDirection);

			// 경사도 기준 + 내리막 방향일 때만 true
			// 내리막에서는 내리막을 탈출할 때 까지 슬라이드가 유지됨.
			if (SurfaceAngleDegrees > 15.f && Dot > 0.0f)
			{
				FVector Accel = SlideDir * SlideSpeed;	// 이거 좀 만져주면 처음에 덜튀어나감.

				float FrictionFloat = 0.001f;			// 감속을 위한 마찰계수

				GetCharacterMovement()->GroundFriction = 0.1f;	// 지면의 마찰력. DeltaTime으로 Velocity의 간접적인 보간을 위해 0.1로 설정. 높이면 들미끄러짐.
				GetCharacterMovement()->BrakingDecelerationWalking = 1500.f; // 지면의 감속력, 이게 2048에 가까워지거나 더 높아지면 Velocity 값이 줄어드는 속도가 빨라짐

				// 동작 방식은 앞으로 작동하는 힘과 뒤로 작용하는 힘이 같이 작동하고 있는 형태임. 여기서 뒤로 작동하는 힘이 더 강하므로 ProceedSpeed가 200 이하가 되어 멈추게 되는 구조
				GetCharacterMovement()->Velocity += Accel * DeltaTime;	// 앞으로 작동하는 힘
				GetCharacterMovement()->Velocity += SlideDir * 8.f;	// 앞으로 작동하는 힘
				GetCharacterMovement()->Velocity -= GetCharacterMovement()->Velocity * FrictionFloat * DeltaTime;	// 뒤로 작용하는 힘 (마찰력)
				GetCharacterMovement()->MoveUpdatedComponent(GetCharacterMovement()->Velocity * DeltaTime, GetCharacterMovement()->UpdatedComponent->GetComponentQuat(), true);

				UE_LOG(LogTemp, Warning, TEXT("%.2f"), ProceedSpeed);

				if (ProceedSpeed <= 30.f)
				{
					UE_LOG(LogTemp, Warning, TEXT("ProceedSpeed가 30이므로 슬라이드 종료"));
					EndSlide();
					return;
				}
				UE_LOG(LogTemp, Warning, TEXT("내리막"));
				return;
			}

			// 경사도는 15도 이상인데 오르막일 때
			else if (SurfaceAngleDegrees > 15.f && Dot< 0.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("오르막. 슬라이드를 할 이유가 없음."));
				EndSlide();
				return;
			}

			// 평지. 15도 이하의 내리막 또한 평지로 간주함.
			else
			{
				FVector Accel = SlideDir * SlideSpeed;

				float FrictionFloat = 0.001f;

				GetCharacterMovement()->GroundFriction = 0;
				GetCharacterMovement()->BrakingDecelerationWalking = 1600.f;  // 지면의 감속력, 이게 2048에 가까워지거나 더 높아지면 Velocity 값이 줄어드는 속도가 빨라짐
				GetCharacterMovement()->Velocity += Accel * DeltaTime;
				GetCharacterMovement()->Velocity -= SlideDir * FrictionFloat * DeltaTime;
				GetCharacterMovement()->MoveUpdatedComponent(GetCharacterMovement()->Velocity * DeltaTime, GetCharacterMovement()->UpdatedComponent->GetComponentQuat(), true);


				UE_LOG(LogTemp, Warning, TEXT("%.2f"), ProceedSpeed);

				if (ProceedSpeed <= 30.f && IsSliding)
				{
					UE_LOG(LogTemp, Warning, TEXT("평지, 속도 30이므로 슬라이드 종료"));
					EndSlide();
					return;
				}

				UE_LOG(LogTemp, Warning, TEXT("평지"));
			}
		}
	}
}

void AMrPinstripeCharacter::EndSlide()
{
	// 슬라이드 종료 시 호출되는 함수
	GetCharacterMovement()->GroundFriction = DefaultFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultDeceleration;
	IsSliding = false;
	EnterCrouchState();
}

void AMrPinstripeCharacter::FindingWallForRunning(float DeltaTime)
{
	if (GetCharacterMovement()->IsFalling())
	{
		FVector Start = FVector(GetCapsuleComponent()->GetComponentLocation().X,
								GetCapsuleComponent()->GetComponentLocation().Y,
								GetCapsuleComponent()->GetComponentLocation().Z + 40.f);

		FVector End = FVector(GetCapsuleComponent()->GetComponentLocation().X,
			GetCapsuleComponent()->GetComponentLocation().Y,
			GetCapsuleComponent()->GetComponentLocation().Z - 40.f);

		FHitResult HitResult;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			Start,
			End,
			70.f,
			ObjectTypes,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			HitResult,
			true
		);

		// 벽이 SphereTrace로 감지된 경우
		if (bHit && HitResult.GetActor()->GetActorLabel().Contains("WallRunning"))
		{
			// 내적을 통해 해당 벽이 플레이어와 수직인지를 비교
			float Dot = FVector::DotProduct(HitResult.Normal, GetActorForwardVector());

			if (Dot <= 0.5f && Dot >= -0.5f && IsWallRunning)
			{
				UE_LOG(LogTemp, Warning, TEXT("월러닝, 현재 월러닝 중인 벽의 법선벡터 %s 이고, CurrentWallNoraml에 저장한다."), *HitResult.Normal.ToString());
				WallRunning(HitResult.ImpactNormal);
				CurrentWallNormal = HitResult.Normal;
			}

			else if(!IsWallRunning && CurrentWallNormal == HitResult.Normal)
			{
				WallJumping(HitResult.ImpactNormal);
				UE_LOG(LogTemp, Warning, TEXT("월점프, 현재 월러닝 중인 벽의 법선벡터는 %s 이다. 법선 벡터가 CurrentWallNormal과 다른데 이게 발동되어선 안된다."), *HitResult.Normal.ToString());
			}
		}
		else
		{
			DetectedWallSign = 0;
		}
	}
}

void AMrPinstripeCharacter::WallRunning(FVector WallLocation)
{
	FVector Forward = GetActorForwardVector(); // 플레이어 전방
	FVector WallNormal = WallLocation; // 벽의 법선 벡터

	FVector Cross = FVector::CrossProduct(Forward, WallNormal);
	float DirectionSign = Cross.Z;

	// 이 벽은 왼쪽에 있다
	if (DirectionSign >= 0)
	{
		DetectedWallSign = 1;
		FRotator Rotation = FRotator(0.f, -90.f, 0.f);
		FVector WallRunDir = Rotation.RotateVector(WallNormal);
		LaunchCharacter(WallRunDir * 850.f, true, true);

	}

	// 이 벽은 오른쪽에 있다
	else
	{
		DetectedWallSign = -1;
		FRotator Rotation = FRotator(0.f, 90.f, 0.f);
		FVector WallRunDir = Rotation.RotateVector(WallNormal);
		LaunchCharacter(WallRunDir * 850.f, true, true);
	}
}

//void AMrPinstripeCharacter::WallJumping(FVector WallNormal)
//{
//	if (GetCharacterMovement()->IsFalling())
//	{
//		FVector Forward = GetActorForwardVector(); // 플레이어 전방
//
//		FVector Cross = FVector::CrossProduct(Forward, WallNormal);
//		float DirectionSign = Cross.Z;
//
//		// 이 벽은 왼쪽에 있다
//		if (DirectionSign >= 0)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("%.2f 왼쪽"), WallNormal.Y);
//
//			FRotator Rotation = FRotator(0.f, 0.f, 0.f);
//			if (WallNormal.Y >= 0) {
//				Rotation = FRotator(0.f, -45.f, -50.f * 1);
//			}
//			else {
//				Rotation = FRotator(0.f, -45.f, -50.f * -1);
//			}
//
//			FVector WallJumpingDir = Rotation.RotateVector(WallNormal);
//			LaunchCharacter(WallJumpingDir * 850.f, true, true);
//		}
//		// 이 벽은 오른쪽에 있다
//		else
//		{
//			UE_LOG(LogTemp, Warning, TEXT("%.2f 오른쪽"), WallNormal.Y);
//
//			FRotator Rotation = FRotator(0.f, 0.f, 0.f);
//			if (WallNormal.Y >= 0) {
//				Rotation = FRotator(0.f, -45.f, -50.f * 1);
//			}
//			else {
//				Rotation = FRotator(0.f, -45.f, -50.f * -1);
//			}
//
//
//
//			FRotator Rotation = FRotator(0.f, 45.f, -50.f * WallNormal.Y);
//			FVector WallJumpingDir = Rotation.RotateVector(WallNormal);
//			LaunchCharacter(WallJumpingDir * 850.f, true, true);
//		}
//	}
//
//}



void AMrPinstripeCharacter::WallJumping(FVector WallNormal)
{
	if (GetCharacterMovement()->IsFalling())
	{
		FVector Forward = GetActorForwardVector(); // 플레이어 전방

		FVector Cross = FVector::CrossProduct(Forward, WallNormal);
		float DirectionSign = Cross.Z;

		// 이 벽은 왼쪽에 있다
		if (DirectionSign >= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("%.2f 왼쪽"), WallNormal.Y);
			FRotator Rotation = FRotator(0.f, -45.f, -50.f * WallNormal.Y);
			FVector WallJumpingDir = Rotation.RotateVector(WallNormal);
			LaunchCharacter(WallJumpingDir * 1050.f, true, true);
		}
		// 이 벽은 오른쪽에 있다
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%.2f 오른쪽"), WallNormal.Y);
			FRotator Rotation = FRotator(0.f, 45.f, -50.f * WallNormal.Y);
			FVector WallJumpingDir = Rotation.RotateVector(WallNormal);
			LaunchCharacter(WallJumpingDir * 1050.f, true, true);
		}
	}
}

void AMrPinstripeCharacter::TiltWhileWallRunning(float DeltaTime)
{
	if (IsWallRunning)
	{
		// 이 벽은 왼쪽에 있다
		if (DetectedWallSign > 0)
		{
			float TargetPitch = 25.f;
			float InterpedPitch = FMath::FInterpTo(ArmMesh->GetRelativeRotation().Pitch, TargetPitch, DeltaTime, CrouchInterpTime);
			

			ArmMesh->SetRelativeRotation(FRotator(InterpedPitch, -89.999999f, 0.f));
		}

		else if (DetectedWallSign == 0)
		{
			float TargetPitch = 0.f;
			float InterpedPitch = FMath::FInterpTo(ArmMesh->GetRelativeRotation().Pitch, TargetPitch, DeltaTime, CrouchInterpTime);
			ArmMesh->SetRelativeRotation(FRotator(InterpedPitch, -89.999999f, 0.f));
		}

		// 이 벽은 오른쪽에 있다
		else
		{
			float TargetPitch = -25.f;
			float InterpedPitch = FMath::FInterpTo(ArmMesh->GetRelativeRotation().Pitch, TargetPitch, DeltaTime, CrouchInterpTime);
			ArmMesh->SetRelativeRotation(FRotator(InterpedPitch, -89.999999f, 0.f));
		}
	}

	else
	{
		float TargetPitch = 0.f;
		float InterpedPitch = FMath::FInterpTo(ArmMesh->GetRelativeRotation().Pitch, TargetPitch, DeltaTime, CrouchInterpTime);
		ArmMesh->SetRelativeRotation(FRotator(InterpedPitch, -89.999999f, 0.f));
	}
}

