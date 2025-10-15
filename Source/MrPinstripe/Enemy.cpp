// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "MrPinstripeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void AEnemy::Init() {

	EnemyState = EEnemyCombatState::Scouting;
	IsFindCoverObject = false;
	IsWallRunning = false;
	IsCrouching = false;
	IsDetectedPlayer = false;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMrPinstripeCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AMrPinstripeCharacter* MyChar = Cast<AMrPinstripeCharacter>(Actor);
		if (MyChar && MyChar->GetName().Contains("Viewmodel"))
		{
			TargetPlayerCharacter = MyChar;
			break;
		}
	}

	AimOffsetVector = FVector(0.f, 5.f, 0.f);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetectingPlayerByDistance(DeltaTime);
	CaculatingAimOffsetRotation(DeltaTime);
}

void AEnemy::Scouting(FVector ScoutPos){
	EnemyState = EEnemyCombatState::Scouting;
}

void AEnemy::Standing() {

}

void AEnemy::CustomEnemyCrouch() {

}

void AEnemy::Moving() {
	EnemyState = EEnemyCombatState::Scouting;
}

void AEnemy::FindingPlayerAndFocus() {

}

void AEnemy::Firing() {

}

float AEnemy::GetCurrentVelocity() {
	return FMath::Clamp(GetVelocity().Size() / GetCharacterMovement()->MaxWalkSpeed, 0.0f, 1.0f);
}

void AEnemy::WallRunning() {

}

void AEnemy::Damaged() {

}

void AEnemy::Die() {

}

void AEnemy::FindingCoverObject(float DeltaTime) {

}

void AEnemy::FindingWallForWallRunning(float DeltaTime) {

}

// 플레이어를 거리에 따라 감지하고 플래그를 전환
// 전환되는 플래그는 에임오프셋 계산여부를 결정
void AEnemy::DetectingPlayerByDistance(float DeltaTime) {

	if (FVector::Dist(TargetPlayerCharacter->GetActorLocation(), GetActorLocation()) <= 300.f) {
		IsDetectedPlayer = true;
	}
	else {
		IsDetectedPlayer = false;
	}
}

void AEnemy::CaculatingAimOffsetRotation(float DeltaTime) {

	if (IsDetectedPlayer) {


		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = TargetPlayerCharacter->GetActorLocation();

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);



		// 내적 계산
		FVector ForwardVector = GetActorForwardVector(); // 몬스터가 바라보는 방향

		FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal(); // 방향 벡터
		float Dot = FVector::DotProduct(ForwardVector, ToTarget);

		if (Dot > 0) {

			UE_LOG(LogTemp,Warning,TEXT("정면에 있음, 내적은 : %.2f"), Dot);

			FVector Cross = FVector::CrossProduct(ForwardVector, ToTarget);
			float Sign = FMath::Sign(Cross.Z);

			float CaculatedYaw = 0.f;


			// 왼쪽일때 외적이 -1, 오른쪽일때 1

			// 왼쪽
			if (Sign < 0) {
				UE_LOG(LogTemp, Warning, TEXT("왼쪽 Pitch 값 : %.2f"), (1 - Dot) * -90.f );
				CaculatedYaw = (1 - Dot) * -90.f;
			}

			// 오른쪽
			else {

				UE_LOG(LogTemp, Warning, TEXT("오른쪽 Pitch 값 : %.2f"), (1 - Dot) * 90.f);
				CaculatedYaw = (1 - Dot) * 90.f;
			}

			UE_LOG(LogTemp, Warning, TEXT("Pitch : %.2f"), LookAtRot.Pitch);
			UE_LOG(LogTemp, Warning, TEXT("Yaw : %.2f"), CaculatedYaw);
			UE_LOG(LogTemp, Warning, TEXT("Roll : %.2f"), LookAtRot.Roll);

			AimOffsetVector.X = CaculatedYaw;
			AimOffsetVector.Y = LookAtRot.Pitch;
			AimOffsetVector.Z = LookAtRot.Roll;




		}
		else {

			UE_LOG(LogTemp, Warning, TEXT("정면에 있지 않음."));

			AimOffsetVector.X = 0.f;
			AimOffsetVector.Y = 0.5f;
			AimOffsetVector.Z = 0.f;

		}
		

	}
}