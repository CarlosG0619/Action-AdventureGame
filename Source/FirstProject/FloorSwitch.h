// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class has the functionality to place a door opening switch in the world,
 * used to provide common gameplay mechanics, the mesh movement functionality is done
 * in Unreal blueprints, using the Timeline node for ease of use.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	/** Overlap volume for the functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	class UBoxComponent* TriggerBox;

	/** Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	class UStaticMeshComponent* FloorSwitch;

	/** Door to move when the floor switch is stepped on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	class UStaticMeshComponent* Door;

	/** Initial location for the door */
	UPROPERTY(BlueprintReadWrite, Category = FloorSwitch)
	FVector InitialDoorLocation;

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = FloorSwitch)
	FVector InitialSwitchLocation;

	/** TimerHandle object to call GetWorldTimerManager().SetTimer() in OnOverlapEnd() */
	FTimerHandle SwitchHandle;

	/** Timer for the floor swith */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	float SwitchTimer;

	/** Boolean to determine if player is on top of the switch */
	bool bCharacterOnSwitch;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Enable Overlap for the Floor Switch functionality */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** The next 4 functions are defined in the FloorSwitch_BP event graph.
	/* BP Location: Content/GameplayMechanics/FloorSwitch_BP */
	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void RaiseDoor();
	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void LowerDoor();
	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void RaiseFloorSwitch();
	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void LowerFloorSwitch();

	/** Functions to update the locations of the Door and FloorSwitch meshes in the world*/
	UFUNCTION(BlueprintCallable, Category = FloorSwitch)
	void UpdateDoorLocation(float z);
	UFUNCTION(BlueprintCallable, Category = FloorSwitch)
	void UpdateFloorSwitchLocation(float z);

	/** Close door on a timer after player steps out of the Floor Switch */
	void CloseDoor();
};
