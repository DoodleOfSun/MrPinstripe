// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Navigation/NavLinkProxy.h"
#include "GameFramework/CharacterMovementComponent.h"


#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavigationPath.h"
#include "AI/Navigation/NavigationTypes.h"


AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledEnemy = Cast<AEnemy>(InPawn);


	CurrentCoverActor = nullptr;
	CurrentWallRunningActor = nullptr;
	CurrentWallImpactNormal = FVector(0, 0, 0);

	// 딜레이를 주고 이동 명령 실행
	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DelayHandle,
		this,
		&AEnemyAIController::InitMoving,
		0.2f, // 딜레이 시간 (초)
		false
	);
}


void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ControlledEnemy->TargetPlayerCharacter->IsPlayerDead) {
		ControlledEnemy->MoveAudioComponent->Stop();
		return;
	}

	EnemyMovingLogic(DeltaTime);

	if (!ControlledEnemy->MoveAudioComponent->IsPlaying() && ControlledEnemy->GetVelocity().Size() != 0 && !ControlledEnemy->GetCharacterMovement()->IsFalling()) {

		ControlledEnemy->MoveAudioComponent->Play();
	}
	else if (ControlledEnemy->MoveAudioComponent->IsPlaying() && ControlledEnemy->GetVelocity().Size() == 0 || ControlledEnemy->GetCharacterMovement()->IsFalling())
	{
		ControlledEnemy->MoveAudioComponent->Stop();
	}
}

void AEnemyAIController::InitMoving() {

	if (ControlledEnemy && ControlledEnemy->EnemyState != EEnemyCombatState::Die && ControlledEnemy->EnemyState != EEnemyCombatState::Hit)
	{
		if (ControlledEnemy->GetName().Contains("Expert"))
		{
			MoveForExpert();
		}
		else
		{
			MoveToNextCoverLocation();
		}
	}
}

void AEnemyAIController::EnemyMovingLogic(float DeltaTime) {

	// 플레이어가 일정 거리에 들어오면 IsDetectedPlayer가 True가 된다
	// 또 적이 플레이어에게 레이트레이스를 쏘았을때 충돌 여부에 따라 IsReadyToShot이 True가 되는데 이것으로 위치를 옮길지를 판단한다.
	// 단, 이 로직은 10초마다 검사하도록 한다. 적이 항상 이동을 하는 것은 부자연스럽기 때문이다.
	// 이동을 방지하겠다면 단순히 CoverMoveTime으로 판단하는 조건문을 없애고 1번만 실행시키게 할 것.

	// 객체의 이름에 따라 움직임 타입을 다르게 한다.
	// 만약 Expert라는 이름이 붙은 객체라면, 플레이어를 쫓아 움직이며, 월 러닝 플래그가 true가 되면 이 기능을 멈추어야 한다.

	if (ControlledEnemy->IsExpert)
	{
		MoveTime += DeltaTime;

		// 월러닝
		if (ControlledEnemy->IsWallRunning && !IsJumpedForWallRunning) {
			MoveTime = 0.f;

			JumpToWall();


			return;
		}

		else if (IsJumpedForWallRunning) {
			WallRunning();
		}

		// 일반 이동
		else if (MoveTime >= ChaseMoveInterval)
		{
			MoveTime = 0.f;

			// 테스트용-----------

			if (ControlledEnemy->IsDetectedPlayer)
			{
				UE_LOG(LogTemp, Warning, TEXT("전문가형 적 위치 변경 중... 테스트용"));
				MoveForExpert();
			}

			// --------------------

			if (ControlledEnemy->IsDetectedPlayer && !ControlledEnemy->IsReadyToShot)
			{
				UE_LOG(LogTemp, Warning, TEXT("전문가형 적 위치 변경 중..."));
				MoveForExpert();
			}
		}
	}
	else
	{
		MoveTime += DeltaTime;

		if (MoveTime >= CoverMoveInterval)
		{
			MoveTime = 0.f;
			if (ControlledEnemy->IsDetectedPlayer && !ControlledEnemy->IsReadyToShot)
			{
				UE_LOG(LogTemp, Warning, TEXT("엄폐물 위치 변경 중..."));
				MoveToNextCoverLocation();
			}
		}
	}

}

void AEnemyAIController::MoveToNextCoverLocation()
{
	IsCover = true;

	TArray<FHitResult> HitResults;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		ControlledEnemy->GetActorLocation(),
		ControlledEnemy->GetActorLocation(),
		1000.f,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	AActor* NearestCoverActor = nullptr;
	float NearestDistance = 9999.f;

	// 여러 개의 엄폐물 중 하나 선택해서 이동
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor()->GetActorLabel().Contains("Cover") && Hit.GetActor() != CurrentCoverActor && Hit.GetActor() != NearestCoverActor)
		{
			float Distance = FVector::Dist(ControlledEnemy->GetActorLocation(), Hit.GetActor()->GetActorLocation());
			if (NearestDistance > Distance) {
				NearestCoverActor = Hit.GetActor();
				NearestDistance = Distance;
			}
		}
	}
	
	if (NearestCoverActor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("위치변경 NearestCoverActor: %s"), *NearestCoverActor->GetActorLabel());
		MoveToActor(NearestCoverActor, 65.f);

		DebugFindPath(ControlledEnemy, NearestCoverActor);

		CurrentCoverActor = NearestCoverActor;
	}

}

// Expert라는 이름이 붙은 객체는 플레이어를 쫓아 움직인다
void AEnemyAIController::MoveForExpert()
{
	MoveToActor(ControlledEnemy->TargetPlayerCharacter, 600.f);
}


// 주변 벽을 SphereTrace로 탐지하고 가장 가까운 벽을 향해 점프
void AEnemyAIController::JumpToWall()
{
	IsJumpedForWallRunning = true;

	TArray<FHitResult> HitResults;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		ControlledEnemy->GetActorLocation(),
		ControlledEnemy->GetActorLocation(),
		1000.f,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResults,
		true
	);


	AActor* NearestWallActor = nullptr;
	float NearestDistance = 9999.f;

	// 여러 개의 액터 중 가장 거리가 가까운 월 러닝 가능하며 이름이 WallRunningPlane을 포함하는 이름인 벽을 선택해 그 벽으로 점프
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor()->GetActorLabel().Contains("WallRunningPlane"))
		{
			float Distance = FVector::Dist(ControlledEnemy->GetActorLocation(), Hit.GetActor()->GetActorLocation());
			if (NearestDistance > Distance) {
				NearestWallActor = Hit.GetActor();
				CurrentWallImpactNormal = Hit.ImpactNormal;
				NearestDistance = Distance;
			}
		}
	}

	if (NearestWallActor != nullptr)
	{
		CurrentWallRunningActor = NearestWallActor;

		FVector DirectionWall = (CurrentWallRunningActor->GetActorLocation() - ControlledEnemy->GetActorLocation()).GetSafeNormal();

		float JumpPower = 550.f;

		float XDir = DirectionWall.X * JumpPower;
		float YDir = DirectionWall.Y * JumpPower;

		ControlledEnemy->LaunchCharacter(FVector(XDir, YDir, JumpPower), true, true);
	}

}

void AEnemyAIController::WallRunning()
{
	FHitResult HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		ControlledEnemy->GetActorLocation(),
		ControlledEnemy->GetActorLocation(),
		45.f,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResult,
		true
	);


	if (bHit && HitResult.GetActor()->GetActorLabel().Contains("WallRunning"))
	{
		FVector Forward = ControlledEnemy->GetActorForwardVector();
		ControlledEnemy->IsWallRunningForAnimation = true;
		CurrentWallImpactNormal = HitResult.ImpactNormal;
		FVector Cross = FVector::CrossProduct(Forward, CurrentWallImpactNormal);
		float DirectionSign = Cross.Z;

		// 이 벽은 왼쪽에 있다
		if (DirectionSign >= 0)
		{
			DetectedWallSign = 1;
			FRotator Rotation = FRotator(0.f, -90.f, 0.f);
			FVector WallRunDir = Rotation.RotateVector(CurrentWallImpactNormal);
			ControlledEnemy->LaunchCharacter(WallRunDir * 850.f, true, true);

		}

		// 이 벽은 오른쪽에 있다
		else
		{
			DetectedWallSign = -1;
			FRotator Rotation = FRotator(0.f, 90.f, 0.f);
			FVector WallRunDir = Rotation.RotateVector(CurrentWallImpactNormal);
			ControlledEnemy->LaunchCharacter(WallRunDir * 850.f, true, true);
		}
	}
}

void AEnemyAIController::WallJumping()
{
	UE_LOG(LogTemp, Warning, TEXT("월러닝을 그만 하고 싶습니다."));
	DetectedWallSign = 0;
	if (IsJumpedForWallRunning)
	{
		ControlledEnemy->IsWallRunning = false;
		IsJumpedForWallRunning = false;
		ControlledEnemy->IsWallRunningForAnimation = false;
		

		FVector Forward = ControlledEnemy->GetActorForwardVector();

		FVector Cross = FVector::CrossProduct(Forward, CurrentWallImpactNormal);
		float DirectionSign = Cross.Z;

		// 이 벽은 왼쪽에 있다
		if (DirectionSign >= 0)
		{
			FRotator Rotation = FRotator(0.f, -45.f, -50.f * CurrentWallImpactNormal.Y);
			FVector WallJumpingDir = Rotation.RotateVector(CurrentWallImpactNormal);
			ControlledEnemy->LaunchCharacter(WallJumpingDir * 1050.f, true, true);
		}
		// 이 벽은 오른쪽에 있다
		else
		{
			FRotator Rotation = FRotator(0.f, 45.f, -50.f * CurrentWallImpactNormal.Y);
			FVector WallJumpingDir = Rotation.RotateVector(CurrentWallImpactNormal);
			ControlledEnemy->LaunchCharacter(WallJumpingDir * 1050.f, true, true);
		}

	}
}

void AEnemyAIController::DebugFindPath(AActor* FromActor, AActor* ToActor)
{
	if (!FromActor || !ToActor) {
		UE_LOG(LogTemp, Warning, TEXT("DebugFindPath: null actor"));
		return;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) {
		UE_LOG(LogTemp, Error, TEXT("NavSys null"));
		return;
	}

	const FVector Start = FromActor->GetActorLocation();
	const FVector End = ToActor->GetActorLocation();

	FPathFindingQuery Query;
	const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	Query = FPathFindingQuery(this, *NavData, Start, End);

	FPathFindingResult PFResult = NavSys->FindPathSync(Query);

	UE_LOG(LogTemp, Warning, TEXT("FindPathSync Result: %d"), (int)PFResult.Result);
	if (PFResult.Path.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Path valid. Point count: %d, Path length: %f"), PFResult.Path->GetPathPoints().Num(), PFResult.Path->GetLength());
		for (int32 i = 0; i < PFResult.Path->GetPathPoints().Num(); ++i)
		{
			FVector P = PFResult.Path->GetPathPoints()[i].Location;
			UE_LOG(LogTemp, Warning, TEXT("Point %d: %s"), i, *P.ToCompactString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Path invalid or null"));
	}




	FNavLocation Projected;
	bool bProjected = NavSys->ProjectPointToNavigation(
		ToActor->GetActorLocation(),
		Projected,
		FVector(50, 50, 200)
	);

	UE_LOG(LogTemp, Warning, TEXT("Projected on Nav: %d, ProjectedLoc: %s"),
		bProjected, *Projected.Location.ToCompactString());


}
