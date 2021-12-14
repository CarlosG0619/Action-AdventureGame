// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AMainPlayerController::AMainPlayerController()
{
	bPauseMenuOpen = false;
}

// Called when the game starts or when spawned
void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// If HUDOverlayAsset is selected in the Blueprint, create the widget and add it to viewport
	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// If WEnemyHealthBar is selected in the Blueprint, create the widget and add it to viewport
	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		// Alignment of the HealthBar in the screen
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	// If WPauseMenu is selected in the Blueprint, create the widget and add it to viewport
	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// Called every frame
void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Functionality to display Enemy Health Bar above the enemy
	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport; // 2D Vector for the Enemy HealthBar location in the screen
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport); // Projecting the Enemy location in the screen
		PositionInViewport.Y -= 85.f; // Raising the HealthBar location above the enemy
		FVector2D SizeInViewport(300.f, 25.f); // Size vector for the enemy Halth Bar in the screen
		
		// Displaying the HealthBar in the screen
		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

// Called when player is inside the CombatSphere of an enemy
void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

// Called when player exits the CombatSphere of an enemy
void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Called when player presses the menu key
void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	bPauseMenuOpen = true;

	// Enabling the mouse cursor when the menu is open
	bShowMouseCursor = true;
	FInputModeGameAndUI InputModeGameAndUI;
	SetInputMode(InputModeGameAndUI);
}

// Called when player presses the menu key while in the menu or by selecting Resume
void AMainPlayerController::RemovePauseMenu_Implementation()
{
	bPauseMenuOpen = false;

	// Disabling the mouse cursor
	bShowMouseCursor = false;
	GameModeOnly();
}

// Called when player presses the menu key
void AMainPlayerController::TogglePauseMenu()
{
	if (!bPauseMenuOpen)
	{
		DisplayPauseMenu();
	}
	else
	{
		RemovePauseMenu();
	}
}

// Called from RemovePauseMenu(), it disables the mouse cursor when resuming gameplay
void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}