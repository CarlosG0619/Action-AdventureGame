// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

// Sets default values
APickup::APickup()
{

}

// Called when player enters the pickup sphere collision
void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Calling the Item.h version of OnOverlapBegin()
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//UE_LOG(LogTemp, Warning, TEXT("Pickup::OnOverlapBegin()")); // Checking if overlap works
	
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting OtherActor to Main Character
		if (MainCharacter)
		{
			OnPickupBP(MainCharacter); // Calling blueprint implementable function
			
			//MainCharacter->PickupLocations.Add(GetActorLocation()); // Getting pickup location and adding it to the MainCharacter TArray for debugging

			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true); // Particle effect when player overlaps with pickup
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound); // Sound effect when player overlaps with pickup
			}

			Destroy();
		}
	}
}

// Called when player exists the pickup sphere collision
void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Calling the Item.h version of OnOverlapEnd()
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	//UE_LOG(LogTemp, Warning, TEXT("Pickup::OnOverlapEnd()")); // Checking if overlap works
}

