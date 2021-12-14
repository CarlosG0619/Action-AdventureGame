// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"



// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Setting up AgroSphere
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	AgroSphere->InitSphereRadius(600.f);
	// Setting up CombatSphere
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(100.f);
	// Setting up attack hitbox
	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(GetMesh(), FName("EnemySocket"));
	// Enemy Stats Initialization
	bOverlappingCombatSphere = false;
	Health = 100.f;
	MaxHealth = 100.f;
	Damage = 10.f;
	AttackMinTime = 0.5f;
	AttackMaxTime = 2.f;
	DeathDelay = 3.f;
	bHasValidTarget = false;
	// Enum Initialization
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	// Getting Reference of AI Controller
	AIController = Cast<AAIController>(GetController());
	// Enabling Overlap with AgroSphere
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
	// Enabling Overlap with CombatSphere
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
	// Enabling Overlap with attack hitbox
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::HitboxOnOverlapBegin);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &AEnemy::HitboxOnOverlapEnd);
	// Setting up hitbox collision responses
	Hitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Hitbox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Hitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Hitbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	// Disabling Collision response of the camera against the enemy mesh and capsule
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called when player enters the enemy AgroSphere, enemy starts chasing the player
void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Geeting player character reference
		if (MainCharacter)
		{
			MoveToTarget(MainCharacter); // Enemy chases player
		}
	}
}

//Called when player exits the enemy AgroSphere, enemy stops chasing the player
void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting MainCharacter
		{
			if (MainCharacter)
			{
				// Setting all variables related to chasing the player back to null/false
				bHasValidTarget = false;
				if (MainCharacter->CombatTarget == this)
				{
					MainCharacter->SetCombatTarget(nullptr);
				}
				MainCharacter->SetHasCombatTarget(false);
				MainCharacter->UpdateCombatTarget();

				// Setting the enemy to the idle state and stoping it from moving
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
				if (AIController)
				{
					AIController->StopMovement();
				}
			}
		}
	}
}

// Called when player enters the enemy CombatSphere, enemy starts attacking the player
void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting MainCharacter
		{
			if (MainCharacter)
			{
				bOverlappingCombatSphere = true;
				bHasValidTarget = true;
				// Calling MainCharacter functions related to the interpolation and displaying enemy health bar during combat
				MainCharacter->SetCombatTarget(this);
				MainCharacter->SetHasCombatTarget(true);
				MainCharacter->UpdateCombatTarget();
				CombatTarget = MainCharacter;
				// Setting a randomize time for the enemy to wait before attacking
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
			}
		}
	}
}

// Called when player exits the enemy CombatSphere, enemy stops attacking the player and goes back to chasing
void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherComp)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting MainCharacter
		{
			if (MainCharacter)
			{
				bOverlappingCombatSphere = false;
				CombatTarget = nullptr;
				MoveToTarget(MainCharacter); // Player is no longer close to the enemy so MoveToTarget is called

				// Setting Main Character Interpolation related variables back to false/null until player enters the CombatSphere again
				if (MainCharacter->CombatTarget == this)
				{
					MainCharacter->SetCombatTarget(nullptr);
					MainCharacter->bHasCombatTarget = false;
					MainCharacter->UpdateCombatTarget();
				}

				if (MainCharacter->MainPlayerController)
				{
					// Remove Enemy Health bar only if the component ending the overlap is the main characters mesh
					USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
					if (MainMesh) MainCharacter->MainPlayerController->RemoveEnemyHealthBar();
				}

				// Clearing enemy attack timer if main character runs away
				GetWorldTimerManager().ClearTimer(AttackTimer);
			}
		}
	}
}

// Called by AgroSphereOnOverlapBegin(), in this function the AIController function called MoveTo is called
// To utilize MoveTo two parameters are used, a FAIMoveRequest and a FNavPathSharedPtr
void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget); // Setting enemy movement status to chase

	if (AIController) // If AIController is Valid
	{
		FAIMoveRequest MoveRequest; // FAIMoveRequest declared
		MoveRequest.SetGoalActor(Target); // Setting GoalActor variable for FAIMoveRequest
		MoveRequest.SetAcceptanceRadius(15.0f); // Setting AcceptanceRadius variable FAIMoveRequest

		FNavPathSharedPtr NavPath; // FNavPathSharedPtr declared

		AIController->MoveTo(MoveRequest, &NavPath); // Call MoveTo with the two parameters

		/**
		// Experimenting with the PathPoints inside FNavPathSharedPtr using DebugSpheres
		auto PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}
		*/
	}
}

// Called when Enemy hitbox collides with the player
void AEnemy::HitboxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor); // Casting MainCharacter (to apply damage)
		if (MainCharacter)
		{
			// Particles when the enemy hitbox hits the player
			if (MainCharacter->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket"); // Creating the enemy socket reference
				if (TipSocket)
				{
					// Spawning the particle system at the enemy socket
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainCharacter->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			// Playing the main character hit sound
			if (MainCharacter->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainCharacter->HitSound);
			}
			// Applying damage to main character
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainCharacter, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

// Function not used
void AEnemy::HitboxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called when enemy attacks (in Blueprints)
void AEnemy::ActivateHitbox()
{
	Hitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryOnly = Only overlap is enabled, no physics
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

// Called after finishing an attack animation(in Blueprints)
void AEnemy::DeactivateHitbox()
{
	Hitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called by CombatSphereOnOverlapBegin() function on the last line (when GetWorldTimerManager() is called)
void AEnemy::Attack()
{
	if (Alive() && bHasValidTarget && !bAttacking)
	{
		bAttacking = true;

		if (AIController)
		{
			// Stop enemy movement while attacking
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); // Getting the enemy anim instance to use Montage_Play()
		if (AnimInstance)
		{
			
			// Play enemy CombatMontage animation
			AnimInstance->Montage_Play(CombatMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
		}
	}
}

// Called via Blueprint after combat montage animation ends
void AEnemy::AttackEnd()
{
	bAttacking = false;
	if (bOverlappingCombatSphere)
	{
		// Randomize how fast will the enemy attack again
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}

// Called by ApplyDamage() in the HitboxOnOverlapBegin function of AWeapon
float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		Die(DamageCauser);
	}

	return DamageAmount;
}

// Called when enemy Health Points reach 0
void AEnemy::Die(AActor* Causer)
{
	bAttacking = false; // Stop attacking
	
	// Playing Death animation from CombatMontage
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
	
	// Disabling all collisions of the enemy
	Hitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Updating MainCharacter Combat target to change to another enemy
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(Causer);
	if (MainCharacter)
	{
		MainCharacter->UpdateCombatTarget();
	}
}

// Called from Animation Blueprint after the enemy dies, pause animation and sets the timer to then call Disappear()
void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

// Called after enemy dies after a set time to destroy the actor
void AEnemy::Disappear()
{
	Destroy();
}

// Called from Blueprint, returns true if enemy is not dead
bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}
