// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This simple class is responsible of level transition, it includes a box component
 * with an overlap functionality to spawn the player in the next level of the game.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionVolume.generated.h"

UCLASS()
class FIRSTPROJECT_API ALevelTransitionVolume : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelTransitionVolume();

	/** Box component for the overlap event */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transition")
	class UBoxComponent* TransitionVolume;

	/** Billboard component to make it easier to adjust the box in the editor */
	class UBillboardComponent* Billboard;

	/** Name of the next level the player will spawn in */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	FName TransitionLevelName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Spawn the player in the next level on overlap */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
