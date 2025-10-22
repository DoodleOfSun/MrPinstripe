// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"


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
		// Initialize AI behavior here, e.g., start behavior tree
	}

	CurrentCoverActor = nullptr;

	MoveToNextCoverLocation();

}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Implement AI behavior per frame here


	// 플레이어가 일정 거리에 들어오면 IsDetectedPlayer가 True가 된다
	// 또 적이 플레이어에게 레이트레이스를 쏘았을때 충돌 여부에 따라 IsReadyToShot이 True가 되는데 이것으로 위치를 옮길지를 판단한다.
	// 단, 이 로직은 10초마다 검사하도록 한다. 적이 항상 이동을 하는 것은 부자연스럽기 때문이다.

	CoverMoveTime += DeltaTime;
	
	if (CoverMoveTime >= CoverMoveInterval)
	{
		CoverMoveTime = 0.f;
		if (ControlledEnemy->IsDetectedPlayer && !ControlledEnemy->IsReadyToShot)
		{
			MoveToNextCoverLocation();
		}
	}

}


void AEnemyAIController::MoveToNextCoverLocation()
{
	IsCover = true;

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

	AActor* NearestCoverActor = nullptr;
	float NearestDistance = 9999.f;

	// 여러 개의 엄폐물 중 하나 선택해서 이동
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor()->GetActorLabel().Contains("Cover") && Hit.GetActor() != CurrentCoverActor)
		{
			float Distance = FVector::Dist(ControlledEnemy->GetActorLocation(), Hit.GetActor()->GetActorLocation());
			if (NearestDistance > Distance) {
				NearestCoverActor = Hit.GetActor();
				NearestDistance = Distance;
				MoveToLocation(NearestCoverActor->GetActorLocation(), 100.f);
				CurrentCoverActor = Hit.GetActor();
			}
		}
	}



}