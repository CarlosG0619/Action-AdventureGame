// Fill out your copyright notice in the Description page of Project Settings.

#include "FirstSaveGame.h"

// Sets default values
UFirstSaveGame::UFirstSaveGame()
{
	SaveSlotName = TEXT("FirstSaveSlot");
	UserIndex = 0;

	CharacterStats.WeaponName = "";
	CharacterStats.LevelName = "";
}

