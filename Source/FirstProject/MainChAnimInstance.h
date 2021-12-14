// Fill out your copyright notice in the Description page of Project Settings.

/**
 * Animation instance class for the player's character (Main Character),
 * it provides the necessary values to create an animation blueprint and 
 * set the animations depending on what the player is doing.
 */

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainChAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UMainChAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	/** Pawn reference for blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;
	/** Main Character reference for blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class AMainCharacter* MainCh;
	/** Main Character's speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;
	/** Main Character is in the air Yes/No */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;

	/** Inherited function from AnimInstance, called when character is created */
	virtual void NativeInitializeAnimation() override;

	/** Fucntion active every frame, updating values of the main character that change its animations */
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();
	
};
