// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomWeaponComponent.h"
#include "MrPinstripeCharacter.h"
#include "MrPinstripeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Enemy.h"
#include "MrPinstripeGameInstance.h"


// Sets default values for this component's properties
UCustomWeaponComponent::UCustomWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCustomWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitWeapon();
}

void UCustomWeaponComponent::InitWeapon()
{
	Character = GetCharacterByFinding();
	EquipedWeaponString = "None";
	FindingNiagara();
}


// Called every frame
void UCustomWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UCustomWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}

// 타입에 따라 발사 로직을 다르게 한다
void UCustomWeaponComponent::FireLogic()
{
	if (EquipedWeaponString.Equals("Shotgun"))
	{
		UE_LOG(LogTemp, Warning, TEXT("샷건 발사 로직 실행."));
		ShotGunFire();
	}
	else if (EquipedWeaponString.Equals("Rifle"))
	{
		UE_LOG(LogTemp, Warning, TEXT("라이플 발사 로직 실행."));
		RifleFire();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("권총 혹은 SMG 발사 로직 실행."));
		NormalFire();
	}
	MuzzleFlame();
}


// 권총, 돌격소총 등 기본 발사 로직 (단발, 연사 등) 조준과 지향사격이 구현되어 있음
// NOTE : 카메라의 GetComponentLocation이라는 위치를 사용하고 있는데, 이 위치로 LineTrace 하면 반동이 적용이 된다.
// 반대로 하지 않으면 반동 없이 정 중앙에서 나가게 할 수는 있다. 다른 오브젝트를 추가해서 거기서 나가게 한다던가.
// 그런데 총에는 반동이 있어야 되므로 카메라의 컴포넌트 위치를 올바른 위치로 판단하여 적용하였다.
void UCustomWeaponComponent::NormalFire()
{
	AMrPinstripeGameMode* GM = Cast<AMrPinstripeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (Character == nullptr || Character->GetController() == nullptr || GM->MyCustomState == EGameState::GamePaused)
	{
		UE_LOG(LogTemp, Warning, TEXT("현재 발사 불가. 스테이트는 : %d, 1이면 Pause인 상태임. 아니면 GetController가 nullptr이거나? Character가 nullptr일수도 있다. 추가 출력한다."), GM->MyCustomState);

		UE_LOG(LogTemp, Warning, TEXT("Character 주소 : %p"), Character);
		UE_LOG(LogTemp, Warning, TEXT("GetController 주소 : %p"), Character != nullptr ? Character->GetController() : nullptr);

		return;
	}

	// 히트스캔 로직. 정조준과 지향사격으로 나뉜다.
	// 정조준의 경우
	if (IsAiming) {

		FHitResult Hit;
		UCameraComponent* cam = Character->GetFPSCamera();
		FVector StartTrace = cam->GetComponentLocation();
		FVector EndTrace = StartTrace + (cam->GetForwardVector() * 10000);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character); // 자기 자신 무시

		CloneNiagaraAndFire(StartTrace, EndTrace);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, Params);

		if (Hit.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());

			// 해당 지점에 피격 효과
			BulletHitComponent->Activate(true);

			// 랜덤 회전값 생성
			FRotator RandomRotationYaw = FRotator(
				0.0F,
				FMath::FRandRange(0.0f, 180.F), // Yaw
				0.0F
			);

			// 회전 적용
			BulletHitComponent->SetWorldRotation(RandomRotationYaw);

			BulletHitComponent->SetWorldLocation(Hit.ImpactPoint);


			CallingEnemyDamageFunc(Hit);
		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("정조준을 하고 쏘았으나 GetActor가 nullptr임."));
		}
	}

	// 지향사격의 경우
	else {

		FHitResult Hit;
		UCameraComponent* cam = Character->GetFPSCamera();
		FVector StartTrace = cam->GetComponentLocation();


		float SpreadAngle = 5.0f; // 퍼짐 각도 (도 단위)
		FVector RandomSpread = FMath::VRandCone(cam->GetForwardVector(), FMath::DegreesToRadians(SpreadAngle));

		FVector EndTrace = StartTrace + (RandomSpread * 10000);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character); // 자기 자신 무시

		CloneNiagaraAndFire(StartTrace, EndTrace);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, Params);

		if (Hit.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());

			// 해당 지점에 피격 효과
			BulletHitComponent->Activate(true);

			// 랜덤 회전값 생성
			FRotator RandomRotationYaw = FRotator(
				0.0F,
				FMath::FRandRange(0.0f, 180.F), // Yaw
				0.0F
			);

			// 회전 적용
			BulletHitComponent->SetWorldRotation(RandomRotationYaw);
			BulletHitComponent->SetWorldLocation(Hit.ImpactPoint);

			CallingEnemyDamageFunc(Hit);
		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("지향사격을 하고 쏘았으나 GetActor가 nullptr임."));
		}
	}
}

void UCustomWeaponComponent::RifleFire() {
	AMrPinstripeGameMode* GM = Cast<AMrPinstripeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (Character == nullptr || Character->GetController() == nullptr || GM->MyCustomState == EGameState::GamePaused)
	{
		UE_LOG(LogTemp, Warning, TEXT("현재 발사 불가. 스테이트는 : %d, 1이면 Pause인 상태임. 아니면 GetController가 nullptr이거나? Character가 nullptr일수도 있다. 추가 출력한다."), GM->MyCustomState);

		UE_LOG(LogTemp, Warning, TEXT("Character 주소 : %p"), Character);
		UE_LOG(LogTemp, Warning, TEXT("GetController 주소 : %p"), Character != nullptr ? Character->GetController() : nullptr);

		return;
	}

	// 히트스캔 로직. 정조준과 지향사격으로 나뉜다.
	// 정조준의 경우
	if (IsAiming) {

		FHitResult Hit;
		UCameraComponent* cam = Character->GetFPSCamera();
		FVector StartTrace = cam->GetComponentLocation();
		FVector EndTrace = StartTrace + (cam->GetForwardVector() * 10000);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character); // 자기 자신 무시

		CloneNiagaraAndFire(StartTrace, EndTrace);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, Params);

		if (Hit.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());

			// 해당 지점에 피격 효과
			BulletHitComponent->Activate(true);

			// 랜덤 회전값 생성
			FRotator RandomRotationYaw = FRotator(
				0.0F,
				FMath::FRandRange(0.0f, 180.F), // Yaw
				0.0F
			);

			// 회전 적용
			BulletHitComponent->SetWorldRotation(RandomRotationYaw);

			BulletHitComponent->SetWorldLocation(Hit.ImpactPoint);


			CallingEnemyDamageFunc(Hit);
		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("정조준을 하고 쏘았으나 GetActor가 nullptr임."));
		}
	}

	// 지향사격의 경우
	else {

		FHitResult Hit;
		UCameraComponent* cam = Character->GetFPSCamera();
		FVector StartTrace = cam->GetComponentLocation();


		float SpreadAngle = 1.f; // 퍼짐 각도 (도 단위)
		FVector RandomSpread = FMath::VRandCone(cam->GetForwardVector(), FMath::DegreesToRadians(SpreadAngle));

		FVector EndTrace = StartTrace + (RandomSpread * 10000);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character); // 자기 자신 무시

		CloneNiagaraAndFire(StartTrace, EndTrace);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, Params);

		if (Hit.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());

			// 해당 지점에 피격 효과
			BulletHitComponent->Activate(true);

			// 랜덤 회전값 생성
			FRotator RandomRotationYaw = FRotator(
				0.0F,
				FMath::FRandRange(0.0f, 180.F), // Yaw
				0.0F
			);

			// 회전 적용
			BulletHitComponent->SetWorldRotation(RandomRotationYaw);
			BulletHitComponent->SetWorldLocation(Hit.ImpactPoint);

			CallingEnemyDamageFunc(Hit);
		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("지향사격을 하고 쏘았으나 GetActor가 nullptr임."));
		}
	}
}

// 샷건 팰릿 구현
// 아직 미완성
void UCustomWeaponComponent::ShotGunFire()
{
	/*TArray<FHitResult> HitResultList;
	HitResultList.Reserve(8);*/

	UCameraComponent* cam = Character->GetFPSCamera();
	FVector StartTrace = cam->GetComponentLocation();

	float SpreadAngle; // 퍼짐 각도 (도 단위)


	// 히트스캔 로직. 정조준과 지향사격으로 나뉜다.
	// 정조준의 경우
	if (IsAiming) {
		SpreadAngle = 5.0f;
	}

	// 지향사격의 경우
	else {
		SpreadAngle = 15.0f;
	}

	for (int i = 0; i < 8; i++)
	{
		FVector RandomSpread = FMath::VRandCone(cam->GetForwardVector(), FMath::DegreesToRadians(SpreadAngle));

		FVector EndTrace = StartTrace + (RandomSpread * 10000);

		FHitResult Hit; 
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character); // 자기 자신 무시

		CloneNiagaraAndFire(StartTrace, EndTrace);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, Params);

		if (Hit.GetActor() != nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("샷건 발사 : Hit Actor: %s"), *Hit.GetActor()->GetName());

			// 해당 지점에 피격 효과
			BulletHitComponent->Activate(true);

			// 랜덤 회전값 생성
			FRotator RandomRotationYaw = FRotator(
				0.0F,
				FMath::FRandRange(0.0f, 180.F), // Yaw
				0.0F
			);

			// 회전 적용
			BulletHitComponent->SetWorldRotation(RandomRotationYaw);
			BulletHitComponent->SetWorldLocation(Hit.ImpactPoint);

			CallingEnemyDamageFunc(Hit);


		}
		else if (Hit.GetActor() == nullptr) {

			UE_LOG(LogTemp, Warning, TEXT("샷건 발사 : 쏘았으나 GetActor가 nullptr임."));
		}
		//HitResultList.Add(Hit);
	}
}

// 이 함수는 블루프린트에서 호출한다.
// 전달받은 데미지로 WeaponDamagePerBullet 값을 설정	
void UCustomWeaponComponent::SetThisWeaponDamage(float Damage)
{
	WeaponDamagePerBullet = Damage;
}

// 피격당한 적 객체의 데미지 함수 호출
void UCustomWeaponComponent::CallingEnemyDamageFunc(FHitResult Hit)
{
	AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor());

	if (HitEnemy != nullptr) {

		UE_LOG(LogTemp, Warning, TEXT("적에게 데미지를 가함"));

		if (HitEnemy->HP <= 0 ) {
			UE_LOG(LogTemp, Warning, TEXT("이미 적의 체력이 0이므로 명시적 반환 하였다."));
			return;
		}

		// 데미지를 가했을 때의 위젯 및 사운드 재생
		if (HitEnemy->HP > WeaponDamagePerBullet) {

			//UGameplayStatics::PlaySound2D(GetWorld(), Character->BulletHitSoundCue);
			Character->AttackHitFeedBackWidget->AddToViewport();
		}
		// 데미지를 가해 죽였을 때의 위젯 및 사운드 재생
		else {

			//UGameplayStatics::PlaySound2D(GetWorld(), Character->BulletKilledEnemySoundCue);
			UGameplayStatics::PlaySound2D(GetWorld(), Character->BulletHitSoundCue);
			Character->KilledHitFeedBackWidget->AddToViewport();
		}



		HitEnemy->Damaged(WeaponDamagePerBullet);
	}
}

void UCustomWeaponComponent::Aiming()
{
	if (!IsAiming)
	{
		IsAiming = true;
	}
}

void UCustomWeaponComponent::HipFire()
{
	IsAiming = false;
}

AMrPinstripeCharacter* UCustomWeaponComponent::GetCharacterByFinding()
{
	// Try to find the character by getting the player character from the world
	AMrPinstripeCharacter* MyCharacter = Cast<AMrPinstripeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (MyCharacter != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character found successfully."));
		return MyCharacter;
	}
	UE_LOG(LogTemp, Warning, TEXT("Character found failed."));
	return nullptr;
}

void UCustomWeaponComponent::MuzzleFlame()
{
	if (MuzzleFlameComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("나이아가라 발동됨 - 총구 화염"));

		// 이제 무기 종류가 여러개 추가되었으므로, 그 무기에 따라 오프셋을 변경한다.
		UMrPinstripeGameInstance* GI = Cast<UMrPinstripeGameInstance>(GetWorld()->GetGameInstance());


		if (GI->GetWeaponTypeStr().Equals("Pistol")) {
			MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 13.f, 0.f));
		}
		else if (GI->GetWeaponTypeStr().Equals("SMG")) {
			MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 45.f, 0.f));
		}
		else if (GI->GetWeaponTypeStr().Equals("Rifle")) {
			MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 63.f, -1.5f));
		}
		else if (GI->GetWeaponTypeStr().Equals("Shotgun")) {
			MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 72.f, 0.f));
		}

		MuzzleFlameComponent->Activate(true);
	}
	else {

		UE_LOG(LogTemp, Warning, TEXT("나이아가라 찾지 못함 - 총구 화염"));
	}
}

void UCustomWeaponComponent::InitMuzzleFlameOffset(FVector OffsetData)
{
	MuzzleFlameComponent->SetRelativeLocation(OffsetData);
}


void UCustomWeaponComponent::FindingNiagara()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// WeaponMesh 찾기
	USkeletalMeshComponent* WeaponMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!WeaponMesh) return;

	// WeaponMesh의 자식 컴포넌트 중에서 Niagara 찾기
	TArray<USceneComponent*> Children;
	WeaponMesh->GetChildrenComponents(true, Children);

	for (USceneComponent* Child : Children)
	{
		if (Child->GetName() == TEXT("Niagara")) // 블루프린트에서 이름 확인 필요
		{
			UNiagaraComponent* MuzzleFlashComp = Cast<UNiagaraComponent>(Child);
			if (MuzzleFlashComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("나이아가라 총구 화염을 찾음"));
				MuzzleFlameComponent = MuzzleFlashComp;
				MuzzleFlameComponent->Deactivate();
			}
			break;
		}
	}
	
	for (USceneComponent* Child : Children)
	{
		if (Child->GetName() == TEXT("BulletHitNiagara")) // 블루프린트에서 이름 확인 필요
		{
			UNiagaraComponent* BulletHitComponentComp = Cast<UNiagaraComponent>(Child);
			if (BulletHitComponentComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("나이아가라 피격 컴포넌트를 찾음"));
				BulletHitComponent = BulletHitComponentComp;
				BulletHitComponent->Deactivate();
			}
			break;
		}
	}
	
	for (USceneComponent* Child : Children)
	{
		if (Child->GetName() == TEXT("BulletTraceNiagara")) // 블루프린트에서 이름 확인 필요
		{
			UNiagaraComponent* BulletTraceComp = Cast<UNiagaraComponent>(Child);
			if (BulletTraceComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("나이아가라 총알 트레일 컴포넌트를 찾음"));
				BulletTrailComponent = BulletTraceComp;
				BulletTrailComponent->Deactivate();
			}
			break;
		}
	}

}

// NOTE : 필요하다면 원하는 나이아가라 시스템을 파라메터로 받아 사용하는 구조도 가능하다.
void UCustomWeaponComponent::CloneNiagaraAndFire(FVector Start, FVector End) {

	// 총알 궤적 나이아가라 시스템
	
	//FRotator FireRot = FireDirection.Rotation();
	//// 방향 → 회전값
	//BulletTrailComponent->SetVectorParameter(FName("FireVector"), FVector(50 * 20.f, FireRot.Yaw * 20, FireRot.Pitch * 20));
	//BulletTrailComponent->Activate(true);

	UNiagaraComponent* NewComp = NewObject<UNiagaraComponent>(BulletTrailComponent->GetOwner());
	NewComp->SetAsset(BulletTrailComponent->GetAsset());
	USceneComponent* Parent = BulletTrailComponent->GetAttachParent();
	if (Parent)
	{
		NewComp->AttachToComponent(Parent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	// transform 복사

	// 이제 무기 종류가 여러개 추가되었으므로, 그 무기에 따라 오프셋을 변경한다.
	UMrPinstripeGameInstance* GI = Cast<UMrPinstripeGameInstance>(GetWorld()->GetGameInstance());


	if (GI->GetWeaponTypeStr().Equals("Pistol")) {
		//MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 13.f, 0.f));
		BulletTrailComponent->SetRelativeLocation(FVector(0.f, 13.f, 0.f));
	}
	else if (GI->GetWeaponTypeStr().Equals("SMG")) {
		//MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 45.f, 0.f));
		BulletTrailComponent->SetRelativeLocation(FVector(0.f, 45.f, 0.f));
	}
	else if (GI->GetWeaponTypeStr().Equals("Rifle")) {
		//MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 63.f, -1.5f));
		BulletTrailComponent->SetRelativeLocation(FVector(0.f, 63.f, -1.5f));
	}
	else if (GI->GetWeaponTypeStr().Equals("Shotgun")) {
		//MuzzleFlameComponent->SetRelativeLocation(FVector(0.f, 72.f, 0.f));
		BulletTrailComponent->SetRelativeLocation(FVector(0.f, 72.f, 0.f));
	}

	NewComp->SetRelativeTransform(BulletTrailComponent->GetRelativeTransform());

	FVector FireDirection = (End - Start).GetSafeNormal();


	NewComp->SetVectorParameter(FName("FireVector"), FireDirection);

	// 컴포넌트 등록 + 재생
	NewComp->RegisterComponent();
	NewComp->Activate(true);
}
