// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

// Called every frame
void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Checking if the pawn and its components are valid
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Vector to store the value of the input clamped down
	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f);

	// Checking if the Vector is close to a value of zero
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit; // Creating a HitResult object needed to call SafeMoveUpdatedComponent()
		
		// Function that moves the pawn in the world
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bump into something, slide along the side of it
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
			UE_LOG(LogTemp, Warning, TEXT("Valid Blocking Hit"));
		}
	}
}