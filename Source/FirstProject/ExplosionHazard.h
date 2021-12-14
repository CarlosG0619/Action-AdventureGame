// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This is a class derived from AItem (Item.h), specified for Hazards that can trigger explosions
 * that will damage the player and reduce health points.
 */

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ExplosionHazard.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API AExplosionHazard : public AItem
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AExplosionHazard();
	
	/** Damage value to the player's health points when overlaping with this actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float Damage;
	
	/** DamageType SubclassTo for the ApplyDamage() Function used in OnOverlapBegin() */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	/** Inherited from Item.h, called when player overlaps with Item actors */
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
