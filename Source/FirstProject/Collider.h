// Fill out your copyright notice in the Description page of Project Settings.

/** 
 * This class is not used in the game, it was one of my first classes in Unreal
 * and was used to learn about the engine and gaming programming in general.
 * In this class I make a simple Pawn to learn how to code the essentials like meshes,
 * camera and collision (which is why I called it "Collider").
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class FIRSTPROJECT_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();
	/** Mesh for the pawn */
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;
	/** Sphere component for the pawn */
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;
	/** Camera for the pawn */
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;
	/** Spring Arm to attach the camera for the pawn */
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* SpringArm;
	/** Movement Component for the pawn */
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UColliderMovementComponent* OurMovementComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Getter for the Movement Component */
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	/** Get and Set for the Mesh Component */
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }
	/** Get and Set for the Sphere Component */
	FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }
	/** Get and Set for the Camera Component */
	FORCEINLINE UCameraComponent* GetCameraComponent() { return Camera; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* InCamera) { Camera = InCamera; }
	/** Get and Set for the Spring Arm Component */
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* InSpringArm) { SpringArm = InSpringArm; }

private:
	/** Called to move the pawn */
	void MoveFoward(float input);
	void MoveRight(float input);
	/** Called to move the camera attached to the pawn */
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);
	/** Vector for the camera input */
	FVector2D CameraInput;
};
