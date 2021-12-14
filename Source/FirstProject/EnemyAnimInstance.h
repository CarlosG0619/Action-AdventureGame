// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This is the class to create the animation instance of the enemies in the game,
 * it provides the functionality for their movement and combat animation.
 */

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	/** Pawn reference for blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	/** Enemy reference for blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class AEnemy* Enemy;

	/** Enemy speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	/** Inherited function from AnimInstance, called when enemy is created */
	virtual void NativeInitializeAnimation() override;

	/** Fucntion active every frame, updating values of the enemy that change its animations */
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

};