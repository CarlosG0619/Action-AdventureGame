// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class has the functionality to place floating moving platforms in the world,
 * used to provide common gameplay mechanics, the platforms move with the use of interpolation.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/** Mesh for the platform */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	class UStaticMeshComponent* Mesh;
	/** Start and End points for the platform to move between */
	UPROPERTY(EditAnywhere)
	FVector StartPoint;
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true")) // MakeEditWidget enables special widget in the editor to move the vector in the world
	FVector EndPoint;
	/** Speed value for the interpolation function */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
	float InterpSpeed;
	/** Time to wait between interpolations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
	float InterpTime;
	/** TimerHandle to have pauses between the interpolations */
	FTimerHandle InterpTimer;
	/** Boolean to know if the platform is currently interpolating */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
	bool bInterping;
	/** Distance between start point and end point */
	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Toggles interpolation on or off */
	void ToggleInterping();

	/** Swaping vectors for the start and end points of the interpolation */
	void SwapVectors(FVector& VecOne, FVector& VecTwo);

};
