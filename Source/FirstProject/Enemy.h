// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This is the class to create Enemies in the game, it provides functionality for their AI behavior,
 * the stats, particle effects for when they get damaged, sound effects and combat.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

/** Enum to determine the movement status of the enemy */
UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{
	EMS_Idle UMETA(DeplayName = "Idle"),
	EMS_MoveToTarget UMETA(DeplayName = "MoveToTarget"),
	EMS_Attacking UMETA(DeplayName = "Attacking"),
	EMS_Dead UMETA(DeplayName = "Dead"),
	EMS_MAX UMETA(DeplayName = "DefaulMAX")
};

UCLASS()
class FIRSTPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AEnemy();

	/** Enum variable for the Enemy movement status */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	/** Sphere that once the player overlaps the enemy will chase after */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	/** Sphere that once the player overlaps the enemy will attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere;

	/** AIController variable for reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	/** Enemy's Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	/** Enemy's Max Health  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	/** Enemy's Damage when attacking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	/** Particles when player attack hits the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	/** Sound when player hits the enemy with an attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	/** Sound when enemy attacks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* SwingSound;

	/** Box component for the hitbox */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* Hitbox;

	/** Enemy animation combat montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	/** TimerHandle for enemy attacks */
	FTimerHandle AttackTimer;

	/** Player is Overlapping with enemy CombatSphere Y/N */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	/** Main Character reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMainCharacter* CombatTarget;

	/** Enemy has a valid target Y/N */
	bool bHasValidTarget;

	/** Enemy is attacking Y/N */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	/** Fastest enemy attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;

	/** Slowest enemy attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	/** DamageType SubclassTo for the ApplyDamage() Function used in HitboxOnOverlapBegin() */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	/** TimerHandle for the Destroy() function */
	FTimerHandle DeathTimer;

	/** Value for the amount of time the Destroy() function will be delayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Set and Get functions for EnemyStatus Enum */
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	/** Enables Overlap with Enemy AgroSphere */
	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Enables Overlap with Enemy CombatSphere */
	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Enemy moves to the player */
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMainCharacter* Target);

	/** Enables Overlap with Enemy Hitbox during attacks */
	UFUNCTION()
	void HitboxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void HitboxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Enable/Disable enemy hitbox */
	UFUNCTION(BlueprintCallable)
	void ActivateHitbox();
	UFUNCTION(BlueprintCallable)
	void DeactivateHitbox();

	/** Enter combat animation and stop enemy movement */
	void Attack();

	/** Exit combat animation and return movement to the enemy */
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	/** Virtual Function inherited from AActor, returns a float with the value of the damage */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Enemy dies when its health points reach 0 */
	void Die(AActor* Causer);

	/** Stop the enemy from changing animations once its dead */
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	/** Destroy the enemy from the world after it dies */
	void Disappear();

	/** Checks if the enemy is alive for conditional checks in enemy functions */
	UFUNCTION(BlueprintCallable)
	bool Alive();

};
