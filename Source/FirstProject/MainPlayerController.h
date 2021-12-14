// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This class takes care of the HUD elements for the game, it provides the references 
 * and variables needed to create User Interface Widget Blueprints and set the functionality.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AMainPlayerController();

	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	/** Variable to hold the widget after creating it */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	/** Variable to hold the Enemy Health Bar widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	/** Variable to hold the widget after creating it */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	/** Show or Hide Enemy Health Bar */
	bool bEnemyHealthBarVisible;

	/** Open or Close Pause Menu */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	bool bPauseMenuOpen;

	/** Vector to store the Enemy location in the world to display the EnemyHealthBar */
	FVector EnemyLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Show Enemy Health Bar */
	void DisplayEnemyHealthBar();
	
	/** Hide Enemy Health Bar */
	void RemoveEnemyHealthBar();

	/** Open Pause Menu */
	UFUNCTION(BlueprintNativeEvent)
	void DisplayPauseMenu();

	/** Close Pause Menu */
	UFUNCTION(BlueprintNativeEvent)
	void RemovePauseMenu();

	/** Function to call between DisplayPauseMenu() or RemovePauseMenu() */
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	/** Resume Gameplay when closing the menu */
	void GameModeOnly();
};
