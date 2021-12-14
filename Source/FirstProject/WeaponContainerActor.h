// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This is a class dedicated to provide a way to save a weapon (if equipped)
 * so the player can have the weapon again when loading the game from the saved data.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponContainerActor.generated.h"

UCLASS()
class FIRSTPROJECT_API AWeaponContainerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponContainerActor();

	/** Map to store the Weapons blueprints made in the editor,
	/* used to save the weapon the player is currently holding when saving the game 
	/* and loading it back when loading from a save file */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TMap<FString, TSubclassOf<class AWeapon>> WeaponMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
