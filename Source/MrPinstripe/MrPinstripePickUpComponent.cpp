// Copyright Epic Games, Inc. All Rights Reserved.

#include "MrPinstripePickUpComponent.h"

UMrPinstripePickUpComponent::UMrPinstripePickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UMrPinstripePickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UMrPinstripePickUpComponent::OnSphereBeginOverlap);
}

void UMrPinstripePickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AMrPinstripeCharacter* Character = Cast<AMrPinstripeCharacter>(OtherActor);
	if(Character != nullptr && Character->GetName().Contains("Viewmodel"))
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);
	}
}