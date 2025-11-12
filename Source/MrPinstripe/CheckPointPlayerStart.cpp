// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPointPlayerStart.h"

void ACheckPointPlayerStart::BeginPlay()
{
	Super::BeginPlay();
	CheckPointLocation = GetActorLocation();
}

void ACheckPointPlayerStart::MovePostionToCheckPoint(FVector PosData)
{
	SetActorLocation(PosData);
}