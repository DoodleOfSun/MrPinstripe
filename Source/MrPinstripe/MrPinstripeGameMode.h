// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "MrPinstripeCharacter.h"
#include "Sound/SoundCue.h"
#include "MrPinstripeGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	TitleScreen,
	GamePlaying,
	GamePaused
};	

UCLASS(minimalapi)
class AMrPinstripeGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AMrPinstripeCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere)
	UUserWidget* TitleWidget;

	UPROPERTY(EditAnywhere)
	UUserWidget* PauseWidget;

	UPROPERTY(EditAnywhere)
	UUserWidget* TitleOptionWidget;

	UPROPERTY(EditAnywhere)
	UUserWidget* CrosshairWidget;

	UPROPERTY(EditAnywhere)
	USoundCue* TitleBGMCue;

	UPROPERTY(EditAnywhere)
	UUserWidget* GameOverWidget;

	UPROPERTY(EditAnywhere)
	USoundCue* BGMCueBeta;

public :

	UPROPERTY(BlueprintReadWrite)
	EGameState MyCustomState;

	UFUNCTION(BlueprintCallable)
	void InitGameMode();

public:

	AMrPinstripeGameMode();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void BackToTitle();

	UFUNCTION(BlueprintCallable)
	void Exit();

	UFUNCTION(BlueprintCallable)
	void Pause();

	UFUNCTION(BlueprintCallable)
	void ContinueGame();

	UFUNCTION(BlueprintCallable)
	void TitleOptionOpen();

	UFUNCTION(BlueprintCallable)
	void TitleOptionClose();

	UFUNCTION(BlueprintCallable)
	EGameState GetGameState();

	void MouseCursorOn();

	void MouseCursorOff();
};



