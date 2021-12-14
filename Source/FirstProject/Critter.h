// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class is not used in the game, it was one of my first classes in Unreal
 * and was used to learn about the engine and gaming programming in general.
 * In this class I make a Pawn (like "Collider.h"), this time I used the mesh of a Critter (hence the name).
 * In addition, the movement functions are simplified.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Critter.generated.h"

UCLASS()
class FIRSTPROJECT_API ACritter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACritter();
	/** Mesh for the Critter pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* MeshComponent;
	/** Camera for the Critter pawn */
	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;
	/** Max speed of the Critter pawn */
	UPROPERTY(EditAnywhere, Category = "Pawn Movement")
	float MaxSpeed;

private:
	/** Vector to determine the current speed of the Critter pawn */
	FVector CurrentVelocity;
	
	/** Functions for the Critter pawn to enable inputs and control the pawn in-game */
	void MoveFoward(float Value);
	void MoveRight(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
