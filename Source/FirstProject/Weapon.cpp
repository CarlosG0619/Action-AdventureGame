// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"


// Sets default values
AWeapon::AWeapon()
{
	// Mesh creation
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());
	// Hitbox creation
	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(GetRootComponent());
	Hitbox->SetVisibility(false);
	//Initializing values
	bWeaponParticles = false;
	Damage = 50.f;
	//Initializing the Enum
	WeaponState = EWeaponState::EWS_Pickup;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	// Enabling Overlap for the weapon hitbox collision
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::HitboxOnOverlapBegin);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::HitboxOnOverlapEnd);
	// Setting up weapon hitbox collision
	Hitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Hitbox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Hitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Hitbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when player enters the weapon box collision
void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Calling the Item.h version of OnOverlapBegin()
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting OtherActor to Main Character
		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(this); // Passing the item currently overlapping with the character
		}
	}
}

// Called when player exits the weapon box collision
void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Calling the Item.h version of OnOverlapEnd()
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting OtherActor to Main Character
		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(nullptr); // Resetting the ActiveOverlappingItem variable in MainCharacter.h to nullptr
		}
	}
}

// Called by InteractKeyPressed() in MainCharacter.h
void AWeapon::Equip(AMainCharacter* MainCharacter)
{
	if (MainCharacter)
	{
		// Getting the Main Character Controller for our Weapon to ...
		// use in the ApplyDamage() function called from HitboxOnOverlapBegin()
		SetInstigator(MainCharacter->GetController());
		
		// Setting collision response for the weapon to ignore the camera and the pawn once attached
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetSimulatePhysics(false); // Disable physics because weapon will be attached to the character
		
		// Attaching the weapon to the character
		const USkeletalMeshSocket* RightHandSocket = MainCharacter->GetMesh()->GetSocketByName("RightHandSocket"); // Getting socket from skeleton
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, MainCharacter->GetMesh()); // Attaching weapon to socket
			bRotate = false; // Stopping the weapon from rotating in the character's hand
			MainCharacter->SetEquippedWeapon(this); // Passing the weapon to the MainCharacter EquippedWeapon variable
			MainCharacter->SetActiveOverlappingItem(nullptr); // Resetting the ActiveOverlappingItem varibale in MainCharacter.h to nullptr
		}
		
		// Playing a sound cue when equipping a weapon
		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
		
		// Playing particle effects on the weapon while equipped
		if (!bWeaponParticles)
		{
			IdleParticlesComponent->Deactivate();
		}
		else
		{
			IdleParticlesComponent->Activate();
		}
	}
}

// Called when player attack collides with enemy
void AWeapon::HitboxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor); // Casting Enemy (to apply damage)
		if (Enemy)
		{
			// Particles when the weapon hitbox hits the enemy
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket"); // Creating a weapon socket reference
				if (WeaponSocket)
				{
					// Spawning the particle system at the weapon socket
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			// Playing the enemy hit sound
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			// Applying damage to enemy
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

// Function not used
void AWeapon::HitboxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called when player attacks (in Blueprints)
void AWeapon::ActivateHitbox()
{
	Hitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryOnly = Only overlap is enabled, no physics
}

// Called after finishing an attack animation (in Blueprints)
void AWeapon::DeactivateHitbox()
{
	Hitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

