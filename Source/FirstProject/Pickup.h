// Fill out your copyright notice in the Description page of Project Settings.

/**
 * Derived class from AItem (Item.h), this class focuses on the interactive items
 * that have positive effects like potions to increase the player's health.
 */

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API APickup : public AItem
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	APickup();
	
	/** Called inside OnOverlapBegin(), function definition in Content/GameplayMechanics/Pickup_BP - event graph
	/* Takes care of the functionality of Pickup objects after the character interacts with them */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupBP(class AMainCharacter* Target);

	/** Inherited from Item.h, called when player overlaps with Item actors */
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
