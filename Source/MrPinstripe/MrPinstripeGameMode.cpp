// Copyright Epic Games, Inc. All Rights Reserved.

#include "MrPinstripeGameMode.h"
#include "MrPinstripeCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "MrPinstripeGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"


AMrPinstripeGameMode::AMrPinstripeGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMrPinstripeGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitGameMode();
}

void AMrPinstripeGameMode::InitGameMode()
{
	// 게임 시작 시
	if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true).Contains("Gamemap"))
	{
		// 플레이어 캐릭터 초기화
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMrPinstripeCharacter::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors)
		{
			AMrPinstripeCharacter* MyChar = Cast<AMrPinstripeCharacter>(Actor);
			if (MyChar && MyChar->GetName().Contains("Viewmodel"))
			{
				PlayerCharacter = MyChar;
				break;
			}
		}


		MyCustomState = EGameState::GamePlaying;

		//if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))

		UMrPinstripeGameInstance* GI = Cast<UMrPinstripeGameInstance>(GetGameInstance());

		if (GI->isCrossHairVisible)
		{
			CrosshairWidget->AddToViewport();
		}

		else
		{
			CrosshairWidget->RemoveFromParent();
		}
	}

	// 메인화면 시
	else
	{
		MyCustomState = EGameState::TitleScreen;
		CrosshairWidget->RemoveFromParent();
	}

	// 게임 시작 시 마우스 커서 비활성화
	if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true).Contains("Gamemap"))
	{
		TitleWidget->RemoveFromParent();
		MouseCursorOff();
	}
	else
	{
		TitleWidget->AddToViewport();
		MouseCursorOn();
	}

	// 게임 시작 시 BGM 재생
	if (MyCustomState == EGameState::GamePlaying) {
		UE_LOG(LogTemp, Warning, TEXT("BGM1 재생 시작"));
		//UGameplayStatics::PlaySound2D(this, BGMCueBeta);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("현재 타이틀 화면. 상태는 : %d"), MyCustomState);
		UGameplayStatics::PlaySound2D(this, TitleBGMCue);
	}

	// 어떤 상태이든지 처음에는 GameOverWidget은 비활성화
	// 이 코드가 없어도 되는데, 분명 이유가 있어서 예전에 이걸 해놨을것임
	// 근데 정확한 사항이 기억이 안나고, 주석처리도 안되어있음. 일단 비활성화시켜둠. (주석해제시 오류남)
	//GameOverWidget->RemoveFromParent();
}

void AMrPinstripeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 게임 시작 시
	if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true).Contains("Gamemap"))
	{
		UMrPinstripeGameInstance* GI = Cast<UMrPinstripeGameInstance>(GetGameInstance());

		if (GI->isCrossHairVisible && MyCustomState == EGameState::GamePlaying)
		{
			CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		}


		// 플레이어 체력이 다 할시 게임 오버 위젯 활성화
		if (!PlayerCharacter->IsPlayerDead) {
			GameOverWidget->RemoveFromParent();
		}
		else if (PlayerCharacter->IsPlayerDead) {
			GameOverWidget->AddToViewport();
		}
	}
}

void AMrPinstripeGameMode::StartGame()
{
	// Logic to start the game, e.g., initializing game state, spawning players, etc.
	UE_LOG(LogTemp, Log, TEXT("Game Started"));
	auto world = GEngine->GetCurrentPlayWorld();
	UGameplayStatics::OpenLevel(this, "/Game/MrPinstripe/Map/Gamemap");
}

void AMrPinstripeGameMode::BackToTitle()
{
	// Logic to return to the title screen, e.g., loading a main menu level
	UE_LOG(LogTemp, Log, TEXT("Returning to Title Screen"));
	auto world = GEngine->GetCurrentPlayWorld();
	UGameplayStatics::OpenLevel(this, "/Game/MrPinstripe/Map/Titlemap");

}

void AMrPinstripeGameMode::Exit()
{
	// Logic to exit the game, e.g., closing the application
	UE_LOG(LogTemp, Log, TEXT("Exiting Game"));
	FGenericPlatformMisc::RequestExit(false);
}

void AMrPinstripeGameMode::Pause()
{
	UE_LOG(LogTemp, Error, TEXT("Pause() 호출됨 — 상태 강제 변경"));

	MyCustomState = EGameState::GamePaused;
	// Logic to pause the game, e.g., showing a pause menu, freezing time
	UE_LOG(LogTemp, Warning, TEXT("Game Paused %d") , MyCustomState);
	MouseCursorOn();
	PauseWidget->AddToViewport();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.001f);
}

void AMrPinstripeGameMode::ContinueGame()
{
	// Logic to continue the game, e.g., resuming from a saved state
	MyCustomState = EGameState::GamePlaying;
	UE_LOG(LogTemp, Warning, TEXT("GameMode 주소 여기는 컨티뉴게임: %p"), this);

	UE_LOG(LogTemp, Warning, TEXT("Continuing Game %d"), MyCustomState);
	MouseCursorOff();
	PauseWidget->RemoveFromParent();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

// 마우스 커서 활성화, 조작 비활성화
void AMrPinstripeGameMode::MouseCursorOn()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("마우스 커서가 활성화되고 조작이 비활성화됨."));

		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(PauseWidget->TakeWidget()); // UUserWidget 포인터
		PC->SetInputMode(InputMode);

	}
}

// 마우스 커서 비활성화, 조작 활성화
void AMrPinstripeGameMode::MouseCursorOff()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("마우스 커서가 비활성화되고 조작이 활성화됨."));
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;

		PC->SetInputMode(FInputModeGameOnly());
	}
}

void AMrPinstripeGameMode::TitleOptionOpen()
{
	UE_LOG(LogTemp, Warning, TEXT("Title Option Opened"));
	TitleWidget->RemoveFromParent();
	TitleOptionWidget->AddToViewport();
}


// HACK : 이거 리팩토링 필요함. 이름이 잘못됨. 그러니 메인화면일때 / 게임 화면일 때 버튼 역할이 달라지는 함수인거임
// 된거같은데 왜 주석 안지워놨지 일단 되있음
void AMrPinstripeGameMode::TitleOptionClose()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true).Contains("Titlemap")) {

		UE_LOG(LogTemp, Warning, TEXT("Title Option Closed"));
		TitleWidget->AddToViewport();
		TitleOptionWidget->RemoveFromParent();
	}

	else if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true).Contains("Gamemap")) {
		ContinueGame();
	}
}

EGameState AMrPinstripeGameMode::GetGameState()
{
	// Return the current game state
	return MyCustomState;
}

