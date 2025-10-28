// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpProxy.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


AJumpProxy::AJumpProxy()
{
	OnSmartLinkReached.AddDynamic(this, &AJumpProxy::Jump);
}
void AJumpProxy::Jump(AActor* MovingActor, const FVector& DestinationPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("JumpProxy: Jump called"));
	// AI가 링크에 도달했을 때 점프 실행
	ACharacter* Character = Cast<ACharacter>(MovingActor);
	if (Character)
	{
		Character->LaunchCharacter(FVector(0.f, 0.f, 600.f), false, true);
	}
}