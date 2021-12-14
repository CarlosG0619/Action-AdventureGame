// Fill out your copyright notice in the Description page of Project Settings.

/**
 * This is the class dedicated to the player's character (Main Character of the game),
 * it provides functionality to alter the stats of the character, combat, movement,
 * interactivity with items and save/load capability.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

/** Struct to save data for future play */
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float Health;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float Stamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FVector Location;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	int32 Coins;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FString WeaponName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	bool bWeaponParticles;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FString LevelName;
};

/** Enum to determine player movement status */
UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

/** Enum to determine Stamina bar status */
UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_LowStamina UMETA(DisplayName = "LowStamina"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhastedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AMainCharacter();

	/** Struct variable for the saved data */
	FCharacterStats CharacterStats;


	/// Camera variables
	//
	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccesss = "True"))
	class USpringArmComponent* CameraBoom;
	
	/** Follow Camera following the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccesss = "True"))
	class UCameraComponent* FollowCamera;
	
	/** Base turn rates to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;


	/// Player Character variables
	//
	/** Player Stats */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	/** Player is moving Y/N */
	bool bMovingForward;
	bool bMovingRight;

	/** Enum variable for the player movement status */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enums)
	EMovementStatus MovementStatus;

	/** Enum variable for the player stamina status */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enums)
	EStaminaStatus StaminaStatus;
	
	/** Character movement speed values */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Running)
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Running)
	float SprintingSpeed;

	/** Sprint key/button pressed Y/N */
	bool bSprintKeyDown;
	
	/** Character stamina values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float LowStamina;


	/// Combat and Interactivity variables
	//
	/** Main Controller reference for HUD functions */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Controller)
	class AMainPlayerController* MainPlayerController;

	/** Variable for the weapon attached to the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	/** Variable for the item currently overlapping with the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;

	/** LightAttack key/button pressed Y/N */
	bool bLightAttackKeyDown;

	/** HeavyAttack key/button pressed Y/N */
	bool bHeavyAttackKeyDown;

	/** Player is attacking Y/N */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Anims)
	bool bAttacking;

	/** Variable for the Combat Animation Montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anims)
	class UAnimMontage* CombatMontage;

	/** Particle system when player takes damage from enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	class UParticleSystem* HitParticles;

	/** Sound when enemy hits the player with an attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	class USoundCue* HitSound;

	/** Interpolation rate to turn the main character towards the enemy when attacking */
	float InterpSpeed;

	/** Interpolate towards enemy Y/N */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat)
	bool bInterpToEnemy;

	/** Enemy target for the interpolation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	class AEnemy* CombatTarget;

	/** MainCharacter has a CombatTarget Y/N */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bHasCombatTarget;

	/** CombatTarget Location vector used for dispalying Enemy HealthBar in the World */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Combat)
	FVector CombatTargetLocation;

	/** Variable to use in the UpdateCombatTarget() function. 
	/* This variable will be used to make sure only enemies are included 
	/* in the TArray OverlappingActors inside the function */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TSubclassOf<AEnemy> EnemyFilter;

	/** Variable to load the weapon the player had equipped when saving the game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<class AWeaponContainerActor> WeaponContainer;


	/// Menu and Debugging variables
	//
	/** Menu Key Pressed Y/N */
	bool bMenuKeyDown;

	/** TArray to store the locations of pickup items after the player interacts with them
	/* for debugging purposes */
	TArray<FVector> PickupLocations;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	// Contains Functionality for the stamina bar and ...
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/// Camera and Character Movement functions
	//
	/** Allow input when in-gameplay */
	bool AllowInput(float Value);

	/** Character foward and backward movement */
	void MoveFoward(float Value);
	
	/** Character side to side movement */
	void MoveRight(float Value);

	/** Character jump, Override from ACharacter */
	virtual void Jump() override;

	/** Rotate camera yaw via keyboard input at a given rate 
	/* @param Rate: this is a normalized rate, i.e. 1.0 mean 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/** Rotate camera pitch via keyboard input at a given rate
	/* @param Rate: this is a normalized rate, i.e. 1.0 mean 100% of desired look up/down rate
	*/
	void LookUpAtRate(float Rate);

	/** Rotate camera yaw via mouse input */
	void Turn(float Value);

	/** Rotate camera pitch via mouse input */
	void LookUp(float Value);

	/** Getters for CameraBoom and FollowCamera */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Set Movement Status enum and running speed */
	void SetMovementStatus(EMovementStatus Status);

	/** Set Stamina Status enum */
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	/** Press key/button to sprint */
	void SprintKeyPressed();

	/** Release key/button to stop sprinting */
	void SprintKeyReleased();


	/// Character Stat modification functions
	//
	/** Increases player coins count */
	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	/** Increment player's health points */
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	/** Decrease player's health points */
	void DecrementHealth(float Amount);


	/// Combat and Interactivity Functions
	//
	/** Press key/button to Interact */
	void InteractKeyPressed();

	/** Sets EquippedWeapon and Destroys previous EquippedWeapon */
	void SetEquippedWeapon(AWeapon* Weapon);

	/** Getter for EquippedWeapon */
	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	/** Setter for ActiveOverlappingItem variable */
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	/** Press key/button to LightAttack */
	void LightAttackKeyPressed();

	/** Release key/button to stop LightAttacking */
	void LightAttackKeyReleased();

	/** Press key/button to HeavyAttack */
	void HeavyAttackKeyPressed();

	/** Release key/button to stop HeavyAttacking */
	void HeavyAttackKeyReleased();

	/** Enter combat animation and stop character movement */
	void Attack();

	/** Exit combat animation and return control to player */
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	/** Play sound when player swings the weapon */
	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	/** Setter for InterToEnemy */
	void SetInterpToEnemy(bool Interp);

	/** Get a rotator (only in the yaw) for the main character to rotate
	/* towards the closest enemy when attacking */
	FRotator GetLookAtRotationYaw(FVector Target);

	/** Setter for bHasCombatTarget */
	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	/** Setter for AEnemy* CombatTarget */
	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	/** Automatically update combat target after fighting against multiple enemies and one of them is killed
	/* This function makes sure the Interpolation and the Enemy Health Bar functionality works for the next target
	/* in the case of multiple enemies engagements */
	void UpdateCombatTarget();

	/** Virtual Function inherited from AActor, returns a float with the value of the damage */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Player's health reached 0 */
	void PlayerDead();

	/** Pause all character animations after death */
	UFUNCTION(BlueprintCallable)
	void DeathEnd();


	/// Menu and Debugging Functions
	//
	/** Toggle Pause menu */
	void MenuKeyDown();
	void MenuKeyUp();
	
	/** Switch between levels */
	void SwitchLevel(FName LevelName);

	/** Save current game */
	UFUNCTION(BlueprintCallable)
	void SaveGame();

	/** Load previously saved game
	/* @param: Load Position of character only when loading in the same level as the save data */
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool LoadPosition);

	/** Load game without switching levels if your last save was in a different level
	/* Works the same as LoadGame() but without loading the level or the character position in the world
	/* Takes care of loading the players stats and weapon from previous level when transitioning to another level in game */
	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();

	/** Show location of previously obtained pickups for debbuging */
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();
};
