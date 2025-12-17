// Fill out your copyright notice in the Description page of Project Settings.


#include "MrPinstripeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponStructClass.h"
#include "MrPinstripeSaveGame.h"

UMrPinstripeGameInstance::UMrPinstripeGameInstance()
{
	//// Initialize default values
	//BGMVolume = 0.5f; // Default background music volume
	//SFXVolume = 0.5f; // Default sound effects volume
	//BrightnessFloat = 1.0f; // Default brightness level
	//ResolutionStr = "1920x1080"; // Default resolution
	//ScreenModeStr = "Fullscreen"; // Default screen mode
}

void UMrPinstripeGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("UMrPinstripeGameInstance::Init() called"));
	//// Initialize default values
	BGMVolume = 0.5f; // Default background music volume
	SFXVolume = 0.5f; // Default sound effects volume
	BrightnessFloat = 0.0f; // Default brightness level
	ResolutionStr = "1600 X 900"; // Default resolution
	ScreenModeStr = "FULLSCREEN"; // Default screen mode

	isCrossHairVisible = true; // Default crosshair visibility
}

// 현재 무기 상태를 저장
void UMrPinstripeGameInstance::SaveWeaponSetting()
{
	UMrPinstripeSaveGame* SaveGameInstance = Cast<UMrPinstripeSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMrPinstripeSaveGame::StaticClass()));

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveWeaponSetting: SaveGameInstance 생성 실패"));
		return;
	}

	SaveGameInstance->SavedWeaponData = PlayerWeaponStruct;

	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Log, TEXT("SaveWeaponSetting: 저장 성공"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveWeaponSetting: 저장 실패"));
	}
}

// 현재 무기 상태를 불러옴
void UMrPinstripeGameInstance::LoadWeaponSetting()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadWeaponSetting: 저장 슬롯이 없습니다."));
		return;
	}

	UMrPinstripeSaveGame* LoadedGame = Cast<UMrPinstripeSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

	if (!LoadedGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadWeaponSetting: Load 실패"));
		return;
	}

	PlayerWeaponStruct = LoadedGame->SavedWeaponData;

	UE_LOG(LogTemp, Log, TEXT("LoadWeaponSetting: 데이터 로드 완료"));
}

void UMrPinstripeGameInstance::StartChapterTimer(float TimeLimit)
{
	// 챕터 타이머 시작 로직 구현
	UE_LOG(LogTemp, Warning, TEXT("챕터 타이머가 %.2f 초로 시작되었습니다."), TimeLimit);

	ChapterTimeLimit = TimeLimit;
	GetWorld()->GetTimerManager().SetTimer(
		ChapterTimerHandler,
		this,
		&UMrPinstripeGameInstance::ChapterTimerTick,
		1.0f,
		true
	);
}

void UMrPinstripeGameInstance::ChapterTimerTick()
{
	// 챕터 타이머 틱 로직 구현
	CurrentChapterTime++;
}

bool UMrPinstripeGameInstance::CheckingTimeOver()
{
	if (CurrentChapterTime > ChapterTimeLimit)
	{
		// 시간 초과 시 처리 로직 구현
		UE_LOG(LogTemp, Warning, TEXT("챕터 시간이 초과되었습니다."));
		GetWorld()->GetTimerManager().ClearTimer(ChapterTimerHandler);
		GetWorld()->GetTimerManager().ClearTimer(PlayerViewersStruct.ViewerDecreaseTimerHandle);
		CurrentChapterTime = 0.f;
		return true;
	}
	else {
		return false;
	}
}

bool UMrPinstripeGameInstance::DecreaseEnemyNumberAndCheckingGameDone()
{
	if (CurrentEnemyNumber > 0) {
		CurrentEnemyNumber--;
		UE_LOG(LogTemp, Warning, TEXT("남은 적의 수: %d"), CurrentEnemyNumber);
	}
	else if (CurrentEnemyNumber <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("모든 적을 처치했습니다! 게임 클리어!"));
		// 게임 클리어 처리 로직 구현
		GetWorld()->GetTimerManager().ClearTimer(ChapterTimerHandler);
		GetWorld()->GetTimerManager().ClearTimer(PlayerViewersStruct.ViewerDecreaseTimerHandle);
		CurrentChapterTime = 0.f;
		return true;
	}
	return false;
}

// 권총의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHavePistol() {
	return PlayerWeaponStruct.IsPlayerHavePistol;
}
void UMrPinstripeGameInstance::SetIsPlayerHavePistol(bool Input) {
	PlayerWeaponStruct.IsPlayerHavePistol = Input;
}

// SMG의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHaveSMG() {
	return PlayerWeaponStruct.IsPlayerHaveSMG;
}

void UMrPinstripeGameInstance::SetIsPlayerHaveSMG(bool Input) {
	PlayerWeaponStruct.IsPlayerHaveSMG = Input;
}

// 소총의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHaveRifle() {
	return PlayerWeaponStruct.IsPlayerHaveRifle;
}

void UMrPinstripeGameInstance::SetIsPlayerHaveRifle(bool Input) {
	PlayerWeaponStruct.IsPlayerHaveRifle = Input;
}

// 샷건의 소지 여부
bool UMrPinstripeGameInstance::GetIsPlayerHaveShotgun() {
	return PlayerWeaponStruct.IsPlayerHaveShotgun;
}

void UMrPinstripeGameInstance::SetIsPlayerHaveShotgun(bool Input) {
	PlayerWeaponStruct.IsPlayerHaveShotgun = Input;
}

// 모은 권총의 탄환
int UMrPinstripeGameInstance::GetGatheredAmmo_Pistol() {
	return PlayerWeaponStruct.GatheredAmmo_Pistol;
}

void UMrPinstripeGameInstance::SetGatheredAmmo_Pistol(int data) {
	PlayerWeaponStruct.GatheredAmmo_Pistol = data;
}

// 모은 SMG의 탄환 
int UMrPinstripeGameInstance::GetGatheredAmmo_SMG() {
	return PlayerWeaponStruct.GatheredAmmo_SMG;
}
void UMrPinstripeGameInstance::SetGatheredAmmo_SMG(int data) {
	PlayerWeaponStruct.GatheredAmmo_SMG = data;
}

// 모은 소총의 탄환
int UMrPinstripeGameInstance::GetGatheredAmmo_Rifle() {
	return PlayerWeaponStruct.GatheredAmmo_Rifle;
}
void UMrPinstripeGameInstance::SetGatheredAmmo_Rifle(int data) {
	PlayerWeaponStruct.GatheredAmmo_Rifle = data;
}

// 모은 샷건의 탄환
int UMrPinstripeGameInstance::GetGatheredAmmo_Shotgun() {
	return PlayerWeaponStruct.GatheredAmmo_Shotgun;
}
void UMrPinstripeGameInstance::SetGatheredAmmo_Shotgun(int data) {
	PlayerWeaponStruct.GatheredAmmo_Shotgun = data;
}

// 현재 들고 있는 무기의 이름
FString UMrPinstripeGameInstance::GetWeaponTypeStr() {
	return PlayerWeaponStruct.WeaponTypeStr;
}
void UMrPinstripeGameInstance::SetWeaponTypeStr(FString data) {
	PlayerWeaponStruct.WeaponTypeStr = data;
}

// 시청자 수
// 방송 시작 / 종료
void UMrPinstripeGameInstance::SetStreaming(bool data) {
	PlayerViewersStruct.IsLiveStreaming = data;

	if (PlayerViewersStruct.IsLiveStreaming) {
		GetWorld()->GetTimerManager().SetTimer(
			PlayerViewersStruct.ViewerDecreaseTimerHandle,
			this,
			&UMrPinstripeGameInstance::DecreaseViewersNumbersByTime,
			0.75f,
			true
		);
	}

	else if (!PlayerViewersStruct.IsLiveStreaming) {
		GetWorld()->GetTimerManager().ClearTimer(PlayerViewersStruct.ViewerDecreaseTimerHandle);
	}
}

// 시청자 수는 시간에 따라 점점 줄어듬
UFUNCTION(BlueprintCallable)
void UMrPinstripeGameInstance::DecreaseViewersNumbersByTime()
{
	if (PlayerViewersStruct.IsLiveStreaming) {
		if (PlayerViewersStruct.CurrentViewersCount > 0) {
			PlayerViewersStruct.CurrentViewersCount -= 1;
			UE_LOG(LogTemp,Warning, TEXT("시청자 수 감소중 %d"), PlayerViewersStruct.CurrentViewersCount);
		}
	}
}

// 특정한 값을 더해 시청자 수를 늘림
UFUNCTION(BlueprintCallable)
void UMrPinstripeGameInstance::IncreaseViewersNumbers(int data) {
	PlayerViewersStruct.CurrentViewersCount += data;
}

// 특정한 값을 빼 시청자 수를 줄임
UFUNCTION(BlueprintCallable)
void UMrPinstripeGameInstance::DecreaseViewersNumbers(int data) {
	PlayerViewersStruct.CurrentViewersCount -= data;
}

// 특정한 값을 입력해 시청자 수를 설정
UFUNCTION(BlueprintCallable)
void UMrPinstripeGameInstance::SetViewersNumbers(int data) {
	PlayerViewersStruct.CurrentViewersCount = data;
}

// getter
UFUNCTION(BlueprintCallable)
int UMrPinstripeGameInstance::GetViewersNumbers() {
	return PlayerViewersStruct.CurrentViewersCount;
}
UFUNCTION(BlueprintCallable)
bool UMrPinstripeGameInstance::GetIsLiveStreaming() {
	return PlayerViewersStruct.IsLiveStreaming;
}