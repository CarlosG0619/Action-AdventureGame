// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class is used to access save data, it provides the default SaveSlotName and UserIndex,
 * as well as some default values of the Character Stats, to access the data when loading.
 */

#pragma once

#include "MainCharacter.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	// Set default values
	UFirstSaveGame();

	///
	//// UPROPERTY and Category = Basic is required, otherwise the engine wont save the variables
	///

	/** Name for the saved data */
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	/** Integer for the slot the saved data is located */
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	/** CharcaterStats struct from MainCharacter.h to save various player stats */
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;
};