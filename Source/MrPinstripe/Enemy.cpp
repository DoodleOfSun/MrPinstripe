// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "MrPinstripeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MoveAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MoveAudioComponent"));
	MoveAudioComponent->SetupAttachment(RootComponent);
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
	IsReadyToShot = false;
	IsWallRunningForAnimation = false;

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


	// 총알을 초기화한다
	// 총알의 장전 수는 BP 이름에 따라 다르다.
	if (GetName().Contains("Pistol")) {
		OriginAmmo = 9;
		CurrentAmmo = OriginAmmo;
	}
	else {
		UE_LOG(LogTemp,Warning,TEXT("허용되지 않은 무기 이름입니다. BP 이름을 제대로 확인하셨나요?"));
	}

	CurrentCoverActor = nullptr;

	MissBullet = 0;


	TrackingTraceTime = 0.f;

	// 발사 속도를 초기화
	if (GetName().Contains("Pistol")) {
		FireRateTiming = 1.2f;
	}

	FireTime = 0.f;

	FindingNiagara();
	MuzzleFlameComponent->Deactivate();

	MoveAudioComponent->SetSound(MoveSoundCue);
	MoveAudioComponent->bAutoActivate = false;

	MoveAudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MoveAudioComponent->RegisterComponent();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetectingPlayerByDistance(DeltaTime);
	CaculatingAimOffsetRotation(DeltaTime);
	Firing(DeltaTime);
	AnimationControl();
}

void AEnemy::AnimationControl()
{
	if (GetCharacterMovement()->IsFalling()) {
		if (IsWallRunningForAnimation) {
			WallRunning();
		}
		else
		{
			Jump();
		}
	}
	else
	{
		Scouting();
	}
}


void AEnemy::Scouting(){
	EnemyState = EEnemyCombatState::Scouting;
}

void AEnemy::Jump() {
	EnemyState = EEnemyCombatState::Jump;
}

void AEnemy::WallRunning() {
	EnemyState = EEnemyCombatState::WallRunning;
}

void AEnemy::Falling() {
	EnemyState = EEnemyCombatState::Falling;
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

void AEnemy::Firing(float DeltaTime) {

	if (IsReadyToShot && !IsWallRunning && !GetCharacterMovement()->IsFalling() && GetVelocity().Size() == 0) {

		FireTime += DeltaTime;

		if (FireTime >= FireRateTiming) {

			UE_LOG(LogTemp, Warning, TEXT("플레이어를 발견하여 사격 중. 파이어타임 : %.2f"), FireTime);
			FireTime = 0.f;

			// 발사 로직
			// 처음 3발은 빗나가게 한다
			if (MissBullet < 3) {
				MissBullet++;
				FHitResult Hit;
				FVector StartTrace = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50.f);
	

				float SpreadAngle = 35.0f; // 퍼짐 각도 (도 단위)

				FVector RandomSpread = FMath::VRandCone(
					(TargetPlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal(),
					FMath::DegreesToRadians(SpreadAngle)
				);

				FVector EndTrace = StartTrace + (RandomSpread * 10000);
				DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 5.0f);

				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
				ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);

				GetWorld()->LineTraceSingleByObjectType(Hit, StartTrace, EndTrace, ObjectQueryParams, QueryParams);

				if (Hit.GetActor() != nullptr) {
					UE_LOG(LogTemp, Warning, TEXT("Hit Actor: 적의 공격 %s"), *Hit.GetActor()->GetName());
				}
				else if (Hit.GetActor() == nullptr) {

					UE_LOG(LogTemp, Warning, TEXT("적의 공격, GetActor가 nullptr임."));
				}

				// 총구 화염 활성화
				MuzzleFlameComponent->Activate(true);
			}

			// 플레이어가 위험해질 수 있도록 각도를 조정한 로직
			else
			{
				FHitResult Hit;
				FVector StartTrace = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50.f);

				float SpreadAngle = 15.0f; // 퍼짐 각도 (도 단위)

				FVector RandomSpread = FMath::VRandCone(
					(TargetPlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal(),
					FMath::DegreesToRadians(SpreadAngle)
				);

				FVector EndTrace = StartTrace + (RandomSpread * 10000);


				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
				ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);

				DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 5.0f);
				GetWorld()->LineTraceSingleByObjectType(Hit, StartTrace, EndTrace, ObjectQueryParams, QueryParams);

				if (Hit.GetActor() != nullptr) {
					UE_LOG(LogTemp, Warning, TEXT("Hit Actor, 적의 공격: %s"), *Hit.GetActor()->GetName());
				}
				else if (Hit.GetActor() == nullptr) {

					UE_LOG(LogTemp, Warning, TEXT("적의 공격, GetActor가 nullptr임."));
				}


			}



			// 효과 재생
			MuzzleFlameComponent->Activate(true);
			UGameplayStatics::PlaySoundAtLocation(this, FireSoundCue, GetActorLocation());
		}

		else
		{
			MuzzleFlameComponent->Deactivate();
		}
	}
}

float AEnemy::GetCurrentVelocity()
{
	return GetVelocity().Size();
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

	if (FVector::Dist(TargetPlayerCharacter->GetActorLocation(), GetActorLocation()) <= 1500.f) {
		IsDetectedPlayer = true;

		// 플레이어가 거리 내에 들어오면 지금 위치에서 총을 발사했을 때 적중이 가능한지 라인트레이스로 판단을 한다.
		// 런타임에서 이것을 실행시키면 지나치게 많이 검사를 하므로 3초에 한번씩 실행
		TrackingTraceTime += DeltaTime;
		if (TrackingTraceTime >= 0.3f) {
			TrackingTraceTime = 0.f;
			TrackingPlayerByLineTrace();
		}
	}
	else
	{
		IsDetectedPlayer = false;
	}
}

// 플레이어와 가까워졌다고 해서 바로 공격하지 않는다 플레이어가 엄폐를 할 수도 있기 때문. 
// 확실하게 공격이 가능한지 라인트레이스로 판단하고 플래그를 변경
// 또한 이 함수는 IsDetectedPlayer가 true가 되었을 때부터 라인트레이스를 시작한다
void AEnemy::TrackingPlayerByLineTrace()
{
	FVector MyLoc = GetActorLocation();
	FVector TargetLoc = TargetPlayerCharacter->GetActorLocation();

	FHitResult Hit;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByObjectType(Hit, MyLoc, TargetLoc, ObjectQueryParams, QueryParams);

	if (Hit.GetActor() != nullptr) {
		// 플레이어에게 사격이 가능한 각도임.
		if (Hit.GetActor()->GetName().Contains("Viewmodel")) {
			//DrawDebugLine(GetWorld(), MyLoc, Hit.Location, FColor::Green, false, 5.0f);
			IsReadyToShot = true;	
		}

		// 플레이어를 감지했으나 플레이어가 엄폐함.
		else {
			DrawDebugLine(GetWorld(), MyLoc, Hit.Location, FColor::Red, false, 5.0f);
			IsReadyToShot = false;
		}
	}
}

void AEnemy::CaculatingAimOffsetRotation(float DeltaTime) {

	if (IsDetectedPlayer && !IsWallRunning && !GetCharacterMovement()->IsFalling())

	//if (IsDetectedPlayer)
	{
		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = TargetPlayerCharacter->GetActorLocation();

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);

		// 내적 계산
		FVector ForwardVector = GetActorForwardVector(); // 몬스터가 바라보는 방향

		FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal(); // 방향 벡터
		float Dot = FVector::DotProduct(ForwardVector, ToTarget);

		if (Dot > 0)
		{

			FVector Cross = FVector::CrossProduct(ForwardVector, ToTarget);
			float Sign = FMath::Sign(Cross.Z);

			float CaculatedYaw = 0.f;
			// 왼쪽일때 외적이 -1, 오른쪽일때 1

			// 왼쪽
			if (Sign < 0) {
				CaculatedYaw = (1 - Dot) * -90.f;
			}

			// 오른쪽
			else {
				CaculatedYaw = (1 - Dot) * 90.f;
			}

			AimOffsetVector.X = FMath::FInterpTo(AimOffsetVector.X, CaculatedYaw, DeltaTime, 2.8f);
			AimOffsetVector.Y = FMath::FInterpTo(AimOffsetVector.Y, LookAtRot.Pitch, DeltaTime, 2.8f);
			AimOffsetVector.Z = FMath::FInterpTo(AimOffsetVector.Z, LookAtRot.Roll, DeltaTime, 2.8f);

		}



		// 플레이어는 감지되었으나 후방으로 벗어난 경우 (Dot이 0 이하)
		// 객체를 천천히 플레이어를 향해 돌린다.
		else if(!GetCharacterMovement()->IsFalling())
		{
			SetActorRelativeRotation(FMath::RInterpTo(GetActorRotation(), LookAtRot, DeltaTime, 1.2f));
		}



		// 플레이어가 웅크렸을 때 Y축 오프셋을 하향 조정한다.
		if (TargetPlayerCharacter->IsCrouched) {
			AimOffsetVector.Y = FMath::FInterpTo(AimOffsetVector.Y, LookAtRot.Pitch - 50.f, DeltaTime, 2.2f);
		}

		// 플레이어가 떨어지는 중이 아니라면 Y축 오프셋을 기본값으로 복귀
		else if (!TargetPlayerCharacter->GetCharacterMovement()->IsFalling()) {
			AimOffsetVector.Y = FMath::FInterpTo(AimOffsetVector.Y, LookAtRot.Pitch, DeltaTime, 2.2f);
		}

	}
	else {
		AimOffsetVector.X = FMath::FInterpTo(AimOffsetVector.X, 0.f, DeltaTime, 2.2f);
		AimOffsetVector.Y = FMath::FInterpTo(AimOffsetVector.Y, 0.5f, DeltaTime, 2.2f);
		AimOffsetVector.Z = FMath::FInterpTo(AimOffsetVector.Z, 0.f, DeltaTime, 2.2f);

	}
}


void AEnemy::FindingNiagara()
{
	if (!this) return;

	// CharacterMesh0 찾기
	USkeletalMeshComponent* CharacterMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (!CharacterMesh) return;

	// CharacterMesh의 자식 컴포넌트 중에서 Niagara 찾기
	TArray<USceneComponent*> ChildrenOfCharacterMesh;

	CharacterMesh->GetChildrenComponents(true, ChildrenOfCharacterMesh);

	for (USceneComponent* Child : ChildrenOfCharacterMesh)
	{
		if (Child->GetName() == TEXT("Niagara")) // 블루프린트에서 이름 확인 필요
		{
			UNiagaraComponent* MuzzleFlashComp = Cast<UNiagaraComponent>(Child);
			if (MuzzleFlashComp)
			{
				MuzzleFlameComponent = MuzzleFlashComp;
				MuzzleFlameComponent->Deactivate();
			}
			break;
		}
	}
}