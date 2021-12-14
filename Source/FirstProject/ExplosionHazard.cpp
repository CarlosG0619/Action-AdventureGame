// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosionHazard.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"

// Sets default values
AExplosionHazard::AExplosionHazard()
{
	Damage = 15.f;
}

// Called when player enters the hazard sphere collision
void AExplosionHazard::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Calling the Item.h version of OnOverlapBegin()
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//UE_LOG(LogTemp, Warning, TEXT("AExplosionHazard::OnOverlapBegin()")); // Checking if overlap works
	
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting OtherActor to Main Character
		AEnemy* Enemy = Cast<AEnemy>(OtherActor); // Casting OtherActor to Enemy
		if (MainCharacter || Enemy)
		{
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);

			Destroy();
		}
	} 
}

// Called when player exists the hazard sphere collision
void AExplosionHazard::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Calling the Item.h version of OnOverlapEnd()
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	//UE_LOG(LogTemp, Warning, TEXT("AExplosionHazard::OnOverlapEnd()")); // Checking if overlap works
}

