// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class is not used in the game, it was one of my first classes in Unreal
 * and was used to learn about the engine and gaming programming in general.
 * In this class I recreate the MovementComponent already built in the engine to learn
 * and understand how it works.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ColliderMovementComponent.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API UColliderMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
