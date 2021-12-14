// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class provides the functionality to place box component volumes in the world
 * and spawn any Actor (enemies or items for example) within the boundaries of the box. 
 * Used to provide some level of randomness to the game.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class FIRSTPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();
	// Box component to determine an area where the actor can spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
	class UBoxComponent* SpawningBox;
	// Variables to spawn different actors
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<AActor> Actor_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<AActor> Actor_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<AActor> Actor_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<AActor> Actor_4;

	/** TArray to store the actors */
	TArray<TSubclassOf<AActor>> SpawnArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Returns a random location inside the box component area
	UFUNCTION(BlueprintPure, Category = Spawning)
	FVector GetSpawnPoint();

	// Returns a random actor for the spawn volume
	UFUNCTION(BlueprintPure, Category = Spawning)
	TSubclassOf<AActor> GetSpawnActor();

	/** Spawning our actor in the world, this function is a Code / Blueprint Hybrid
	/** Blueprint implementation is located at Content/GameplayMechanics/SpawnVolume_BP event graph
	/* @param ToSpawn: Pointer to UClass type, this is the actor we want to spawn in the world
	/* @param Location: Vector type, location of where inside the box component will the actor spawn
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Spawning)
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);
};
