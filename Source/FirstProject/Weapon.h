// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class has all the functionality to provide the player with a weapon
 * to engage in combat with the enmies, it provides stats for the weapon, a skeletal mesh,
 * particles, sound effects, and a box component to use as the hitbox of the weapon to
 * apply damage to the enemies.
 */

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/** Enum to determine if player has a weapon equipped */
UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_MAX UMETA(DisplayName = "DefaultMax")
};

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AWeapon();

	/** String variable for the weapon name to access a specific weapon from the WeaponContainerActor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FString Name;

	/** Enum variable for the weapon state */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponState WeaponState;

	/** Boolean to enable/disable weapon particle effects */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	/** SoundCue when equipping a weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	/** SoundCue when attacking with the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* SwingSound;

	/** Skeletal mesh for the weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	/** Weapon's Hitbox when attacking */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* Hitbox;

	/** Damage value for the weapon attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

	/** DamageType SubclassTo for the ApplyDamage() Function used in HitboxOnOverlapBegin() */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	/** Instigator for the ApplyDamage() Function used in HitboxOnOverlapBegin() */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Inherited virtual functions to enable overlap for our Weapon actors */
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	/** Equip the weapon to the character */
	void Equip(class AMainCharacter* Char);

	/** Set and Get for Weapon State enum */
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

	/** Overlap functionality for the weapon hitbox */
	UFUNCTION()
	void HitboxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void HitboxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Enable/Disable weapon hitbox */
	UFUNCTION(BlueprintCallable)
	void ActivateHitbox();
	UFUNCTION(BlueprintCallable)
	void DeactivateHitbox();

	/** Setter for WeaponInstigator */
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }
};
