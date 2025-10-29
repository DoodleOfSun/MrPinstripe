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

	if (ControlledEnemy)
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

	CurrentCoverActor = nullptr;
}


void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 플레이어가 일정 거리에 들어오면 IsDetectedPlayer가 True가 된다
	// 또 적이 플레이어에게 레이트레이스를 쏘았을때 충돌 여부에 따라 IsReadyToShot이 True가 되는데 이것으로 위치를 옮길지를 판단한다.
	// 단, 이 로직은 10초마다 검사하도록 한다. 적이 항상 이동을 하는 것은 부자연스럽기 때문이다.
	// 이동을 방지하겠다면 단순히 CoverMoveTime으로 판단하는 조건문을 없애고 1번만 실행시키게 할 것.
	
	// 객체의 이름에 따라 움직임 타입을 다르게 한다.
	// 만약 Expert라는 이름이 붙은 객체라면, 플레이어를 쫓아 움직이며, 월 러닝 플래그가 true가 되면 이 기능을 멈추어야 한다.
	if (ControlledEnemy->GetName().Contains("Expert"))
	{
		MoveTime += DeltaTime;

		// 월러닝
		if (ControlledEnemy->IsWallRunning) {
			MoveTime = 0.f;

			JumpToWall();

			return;
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




	if (!ControlledEnemy->MoveAudioComponent->IsPlaying() && ControlledEnemy->GetVelocity().Size() != 0) {

		ControlledEnemy->MoveAudioComponent->Play();
	}
	else if (ControlledEnemy->MoveAudioComponent->IsPlaying() && ControlledEnemy->GetVelocity().Size() == 0)
	{
		ControlledEnemy->MoveAudioComponent->Stop();
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
	MoveToActor(ControlledEnemy->TargetPlayerCharacter, 200.f);
}


// 주변 벽을 SphereTrace로 탐지하고 가장 가까운 벽을 향해 점프
void AEnemyAIController::JumpToWall()
{
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

	float NearestDistance = 9999.f;

	// 여러 개의 액터 중 가장 거리가 가까운 월 러닝 가능하며 이름이 WallRunningPlane을 포함하는 이름인 벽을 선택해 그 벽으로 점프
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor()->GetActorLabel().Contains("WallRunningPlane"))
		{
			float Distance = FVector::Dist(ControlledEnemy->GetActorLocation(), Hit.GetActor()->GetActorLocation());
			if (NearestDistance > Distance) {

			}
		}
	}


	// 추락 중이 아니고 지면에 서 있을때 딱 한번 이 액터를 향해 점프
	if (!ControlledEnemy->GetCharacterMovement()->IsFalling()) {

	}

}

void AEnemyAIController::WallRunning()
{
	UE_LOG(LogTemp,Warning,TEXT("안녕하세요저는월러닝"));
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
