// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "AIController.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Creating the box for the area where the actor can spawn
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	
	SpawningBox->bHiddenInGame = true;

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	// Storing all actors in the TArray
	if (Actor_1 && Actor_2 && Actor_3 && Actor_4)
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called by SpawnVolume_BP blueprint
FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent(); // Vector to determine the edge of the spawn area
	FVector Origin = SpawningBox->GetComponentLocation(); // Vector to determine the middle point of the spawn area

	// Function that returns a random location between the origin and the edges of the spawn area
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	return Point;
}

// Called by SpawnVolume_BP blueprint
TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1); // Random number from 0 to 3 (for the index of SpawnArray)

		return SpawnArray[Selection]; // Return randomly selected actor
	}
	else
	{
		return nullptr;
	}
}

// Called by SpawnVolume_BP blueprint
void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		// Getting World and ActorSpawnParameters objects to call SpawnActor<>()
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)
		{
			// Actor is spawned in the world, stored in case its an enemy
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);
			// Casting to Enemy to check if the actor is an enemy
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				// If it is an enemy, spawn a default controller
				Enemy->SpawnDefaultController();
				
				// Cast the default controller to an AIController
				// and set the AIController variable to the enemy spawned so it can have the functionality from Enemy.h
				AAIController* AICont = Cast<AAIController>(Enemy->GetController());
				if (AICont)
				{
					Enemy->AIController = AICont;
				}
			}
		}
	}
}