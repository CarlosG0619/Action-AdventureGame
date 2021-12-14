// Fill out your copyright notice in the Description page of Project Settings.

#include "MainChAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"

// Pawn will get the main character assigned to it and then cast it
void UMainChAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			MainCh = Cast<AMainCharacter>(Pawn);
		}
	}
}

// If Pawn is valid, update the main character's movement speed and status (if its in the air or not)
void UMainChAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr) // Double checking
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (MainCh == nullptr)
		{
			MainCh = Cast<AMainCharacter>(Pawn);
		}
	}
}