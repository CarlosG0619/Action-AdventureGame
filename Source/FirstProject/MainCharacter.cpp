// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "WeaponContainerActor.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set capsule size for collision
	GetCapsuleComponent()->SetCapsuleSize(34.f, 88.f);

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Character does not rotate with camera. Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Character movement configuration...
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character faces the direction of input when moving
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // Rotation rate of the character
	GetCharacterMovement()->JumpZVelocity = 470.f; // Character jump height
	GetCharacterMovement()->AirControl = 0.2f; // Character mid air control

	// Stats initialization
	Health = 100.f;
	MaxHealth = 100.f;
	Stamina = 200.f;
	MaxStamina = 200.f;
	Coins = 0;

	bMovingForward = false;
	bMovingRight = false;
	RunningSpeed = 350.f;
	SprintingSpeed = 800.f;
	bSprintKeyDown = false;
	StaminaDrainRate = 50.f;
	LowStamina = 50.f;

	bLightAttackKeyDown = false;
	bHeavyAttackKeyDown = false;
	bAttacking = false;

	InterpSpeed = 7.f;
	bInterpToEnemy = false;
	bHasCombatTarget = false;

	bMenuKeyDown = false;

	// Enums Initialization
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MainPlayerController = Cast<AMainPlayerController>(GetController()); // Getting Main player controller

	// Get level name without prefix
	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "SunTemple") // Load saved stats and weapon if not on the first level
	{
		LoadGameNoSwitch(); // Loading without changing levels or character position

		if (MainPlayerController)
		{
			// Making sure mouse is disabled in case loading from the menu
			// and previous save was in another level
			MainPlayerController->GameModeOnly();
		}
	}
}

// Called every frame
// Stamina Bar and Interpolation Functionality
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	
	// Functionality for Stamina Bar
	float DeltaStamina = StaminaDrainRate * DeltaTime;
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal: // Stamina bar is above LowStamina threshold
		if (bSprintKeyDown) // Sprint key pressed
		{
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
				Stamina -= DeltaStamina;
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			
			if (Stamina <= LowStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_LowStamina);
			}

		}
		else // Sprint key released
		{
			SetMovementStatus(EMovementStatus::EMS_Normal);
			
			if (Stamina < MaxStamina)
			{
				Stamina += DeltaStamina;
			}
			
		}
		break;

	case EStaminaStatus::ESS_LowStamina: // Stamina Bar is below Low Stamina threshold
		if (bSprintKeyDown) // Sprint key pressed
		{
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
				Stamina -= DeltaStamina;
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}

			if (Stamina <= 0.f)
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
			}

		}
		else // Sprint key released
		{
			SetMovementStatus(EMovementStatus::EMS_Normal);
			
			if (Stamina < LowStamina)
			{
				Stamina += DeltaStamina;
			}
			else
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}

		}
		break;

	case EStaminaStatus::ESS_Exhausted: // Stamina reached 0
		SetMovementStatus(EMovementStatus::EMS_Normal);
		if (bSprintKeyDown) // Sprint key pressed
		{
			Stamina = 0.f;
		}
		else // Sprint key released
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhastedRecovering);
			Stamina += DeltaStamina;
		}
		break;

	case EStaminaStatus::ESS_ExhastedRecovering: // Stamina Recovery from 0 until Low Stamina threshold
		SetMovementStatus(EMovementStatus::EMS_Normal);
		Stamina += DeltaStamina;
		if (Stamina >= LowStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		break;

	default:
		;
	}

	// Functionality to Interpolate towards enemy when inside its combat sphere collision
	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation()); // Get enemy location to rotate towards (Yaw Only)
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed); // Interpolation for a smooth rotation transition
		SetActorRotation(InterpRotation); // Rotate the character
	}

	// Functionality to Display Enemy HealthBar above the Enemy in the world
	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation; // Updating the enemy location in the controller
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Menu", IE_Pressed, this, &AMainCharacter::MenuKeyDown);
	PlayerInputComponent->BindAction("Menu", IE_Released, this, &AMainCharacter::MenuKeyUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::SprintKeyPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::SprintKeyReleased);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::InteractKeyPressed);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AMainCharacter::LightAttackKeyPressed);
	PlayerInputComponent->BindAction("LightAttack", IE_Released, this, &AMainCharacter::LightAttackKeyReleased);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AMainCharacter::HeavyAttackKeyPressed);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Released, this, &AMainCharacter::HeavyAttackKeyReleased);

	PlayerInputComponent->BindAxis("MoveFoward", this, &AMainCharacter::MoveFoward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}

// Called in main input receiving functions (Example: Moving the Character and/or the Camera)
bool AMainCharacter::AllowInput(float Value)
{
	return (Controller != nullptr) &&
		(Value != 0.0f) &&
		(!bAttacking) &&
		(MovementStatus != EMovementStatus::EMS_Dead) &&
		(!MainPlayerController->bPauseMenuOpen);
}

// Called when foward/back character movement input is received
void AMainCharacter::MoveFoward(float Value)
{
	bMovingForward = false;
	if (AllowInput(Value))
	{
		// Find out which way is foward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		// Get foward direction
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

// Called when right/left character movement input is received
void AMainCharacter::MoveRight(float Value)
{
	bMovingRight = false;
	if (AllowInput(Value))
	{
		// Find out which way is foward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		// Get right direction
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

// Called when jump input is received
void AMainCharacter::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuOpen) return;

	if (MovementStatus != EMovementStatus::EMS_Dead && !bAttacking)
	{
		Super::Jump();
	}
}

// Called when turn right/left input is received (with keyboard or controller)
void AMainCharacter::TurnAtRate(float Rate)
{
	if (AllowInput(Rate))
	{
		float a = Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
		float b = FMath::Clamp(a, -1.0f, 1.0f);
		AddControllerYawInput(Rate);
	}
}

// Called when look up/down input is received (with keyboard or controller)
void AMainCharacter::LookUpAtRate(float Rate)
{
	if (AllowInput(Rate))
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

// Called when turn right/left input is received (with mouse)
void AMainCharacter::Turn(float Value)
{
	if (AllowInput(Value))
	{
		AddControllerYawInput(Value);
	}
}

// Called when look up/down input is received (with mouse)
void AMainCharacter::LookUp(float Value)
{
	if (AllowInput(Value))
	{
		AddControllerPitchInput(Value);
	}
}

// Called when player hits the spinting key/button
void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

// Called when player hits the spinting key/button
void AMainCharacter::SprintKeyPressed()
{
	bSprintKeyDown = true;
}

// Called when player releases the spinting key/button
void AMainCharacter::SprintKeyReleased()
{
	bSprintKeyDown = false;
}

// Called when player gets a new coin
void AMainCharacter::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

// Called when player interacts with a health increasing item
void AMainCharacter::IncrementHealth(float Amount)
{
	Health += Amount;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

// Called when player takes damage
void AMainCharacter::DecrementHealth(float Amount)
{
	Health -= Amount;
	if (Health <= 0.f)
	{
		PlayerDead();
	}
}

// Called when player hits the Interact key/button
void AMainCharacter::InteractKeyPressed()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuOpen) return;
	
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	
	if (ActiveOverlappingItem) // Checking if there is an item overlapping with the character
	{
		// Equipping a weapon
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem); // Casting the weapon currently overlapping with our character to the local Weapon variable
		if (Weapon)
		{
			Weapon->Equip(this); // Equipping the weapon
		}
	}
}

// Called when player equips a new weapon
void AMainCharacter::SetEquippedWeapon(AWeapon* Weapon)
{ 
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	
	EquippedWeapon = Weapon;
}

// Called when player presses the LightAttack key/button
void AMainCharacter::LightAttackKeyPressed()
{
	bLightAttackKeyDown = true;

	if (MainPlayerController) if (MainPlayerController->bPauseMenuOpen) return;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (EquippedWeapon)
	{
		Attack();
	}
}

// Called when player releases the LightAttack key/button
void AMainCharacter::LightAttackKeyReleased()
{
	bLightAttackKeyDown = false;
}

// Called when player presses the HeavyAttack key/button
void AMainCharacter::HeavyAttackKeyPressed()
{
	bHeavyAttackKeyDown = true;

	if (MainPlayerController) if (MainPlayerController->bPauseMenuOpen) return;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (EquippedWeapon)
	{
		Attack();
	}
}

// Called when player releases the HeavyAttack key/button
void AMainCharacter::HeavyAttackKeyReleased()
{
	bHeavyAttackKeyDown = false;
}

// Called by AttackKeyPressed() when player Attacks
void AMainCharacter::Attack()
{
	bool bInTheAir = GetMovementComponent()->IsFalling(); // Checking if character is in the air
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bInTheAir)
	{
		bAttacking = true;
		SetInterpToEnemy(true); // Interpolate towards enemy when attacking
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); // Getting the main character anim instance to use Montage_Play()
		if (AnimInstance && CombatMontage)
		{
			if (bLightAttackKeyDown)
			{
				AnimInstance->Montage_Play(CombatMontage, 0.9f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
			}
			else if (bHeavyAttackKeyDown)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
			}
		}
	}
}

// Called via Blueprint after combat montage animation ends
void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false); // Stop interpolation after attacking
	if (bLightAttackKeyDown || bHeavyAttackKeyDown)
	{
		Attack();
	}
}

// Called when player swings the weapon (in Blueprint)
void AMainCharacter::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

// Called while attacking close to an enemy
void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

// Called when character enters the enemy CombatSphere
FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target); // Rotation from the character location to the target
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f); // Yaw only Rotator to return for the interpolation calculation in the Tick function
	return LookAtRotationYaw;
}

// Called when player enters and exits combat with enemies
void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors; // TArray to save all the actors (enemies) currently overlapping with the player
	GetOverlappingActors(OverlappingActors, EnemyFilter); // Get all the enemies overlapping

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar(); // Remove health bar if no enemies are overlapping
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]); // Getting the first enemy in the array
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation(); // Main Character location
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size(); // Distance between enemy and Main character

		for (auto Actor : OverlappingActors) // Loop thru OverlappingActors array
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor); //Getting the enemy at the index of the current iteration of the loop
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size(); // Distance between current enemy in the loop and Main character
				if (DistanceToActor < MinDistance)
				{
					// If any of the enemies from Index 1 or greater are closer than enemy at index 0
					// change the ClosestEnemy and the MinDistance variables to the new closest enemy and its location
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}

		// Display HealthBar of the closest enemy to the player
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		// Set CombatTarget to the closest enemy to make Interpolation functionality work
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

// Called by ApplyDamage() in the HitboxOnOverlapBegin function of AEnemy
float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		PlayerDead();

		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false; // Stop enemy from attacking after main character dies
			}
		}
	}

	return DamageAmount;
}

// Called when player's health reaches 0
void AMainCharacter::PlayerDead()
{
	// Prevents playing the death animation again if the character takes damage before finishing the animation
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

// Called after player dies
void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

// Called when menu key is pressed
void AMainCharacter::MenuKeyDown()
{
	bMenuKeyDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

// Called when menu key is released
void AMainCharacter::MenuKeyUp()
{
	bMenuKeyDown = false;
}

// Called when player overlaps with a Level transition Volume
void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld(); // Get world reference to call GetMapName()
	if (World)
	{
		FString CurrentLevel = World->GetMapName(); // Get current level name
		CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // Removing the prefix set by GetMapName()

		FName CurrentLevelName(*CurrentLevel); // Dereference operator here takes the string literal from the FString CurrentLevel to initialize the FName
		if (CurrentLevelName != LevelName) // Check that the next level is not the one the player is currently at
		{
			UGameplayStatics::OpenLevel(World, LevelName); // Change to the next level
		}
	}
}

// Called when selecting Save Game from the pause menu
void AMainCharacter::SaveGame()
{
	//// StaticClass returns a UClass because its called from UFirstSaveGame, 
	///  that UClass is then sent to CreateSaveGameObject which returns a USaveGame object,
	//   the USaveGame object is casted to a UFirstSaveGame object and stored in SaveObject.
	UFirstSaveGame* SaveObject = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	
	// Copying variables to SaveObject
	SaveObject->CharacterStats.Health = Health;
	SaveObject->CharacterStats.MaxHealth = MaxHealth;
	SaveObject->CharacterStats.Stamina = Stamina;
	SaveObject->CharacterStats.MaxStamina = MaxStamina;
	SaveObject->CharacterStats.Coins = Coins;
	SaveObject->CharacterStats.Location = GetActorLocation();
	SaveObject->CharacterStats.Rotation = GetActorRotation();

	// Copying the level name
	FString MapName = GetWorld()->GetMapName(); // Name of the level prefixed by the editor
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // Removing the prefix from the level name
	SaveObject->CharacterStats.LevelName = MapName; // Saving the level name without the prefix
	
	if (EquippedWeapon)
	{
		// Copying the weapon name if the player has one equipped, 
		// used for to access the weapon from the Map in WeaponContainerActor
		SaveObject->CharacterStats.WeaponName = EquippedWeapon->Name;
		SaveObject->CharacterStats.bWeaponParticles = EquippedWeapon->bWeaponParticles;
	}

	// Saving everyithing that was copied in SaveObject to the selected save slot in the menu
	UGameplayStatics::SaveGameToSlot(SaveObject, SaveObject->SaveSlotName, SaveObject->UserIndex);
}

// Called when selecting Load Game from the pause menu
void AMainCharacter::LoadGame(bool LoadPosition)
{
	// Creating a USaveGame object to get the SlotName and SlotNumber (index) from the menu
	UFirstSaveGame* Load = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	// Creating another USaveGame object to load all the stored data from the selected save file
	UFirstSaveGame* LoadObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)
	{
		// Loading the map
		if (LoadObject->CharacterStats.LevelName != "")
		{
			FName Map(*LoadObject->CharacterStats.LevelName);
			SwitchLevel(Map);
		}

		// Loading the character stats
		Health = LoadObject->CharacterStats.Health;
		MaxHealth = LoadObject->CharacterStats.MaxHealth;
		Stamina = LoadObject->CharacterStats.Stamina;
		MaxStamina = LoadObject->CharacterStats.MaxStamina;
		Coins = LoadObject->CharacterStats.Coins;

		// Loading the weapon
		if (WeaponContainer)
		{
			AWeaponContainerActor* Container = GetWorld()->SpawnActor<AWeaponContainerActor>(WeaponContainer); // Spawning the WeaponContainerActor to access the weapons
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName; // Loading the name of the weapon the player had equipped when he saved the game
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName)) // Checking if the weapon is in the Map from WeaponContainerActor
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]); // Spawn the Weapon and store it
						if (Weapon)
						{
							// Equip the weapon to the character
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);

						}
					}
				}
			}
		}

		// Loading the position of the character
		if (LoadPosition)
		{
			SetActorLocation(LoadObject->CharacterStats.Location);
			SetActorRotation(LoadObject->CharacterStats.Rotation);
		}

		// Resetting Main Character to a playable state when loading after dying
		SetMovementStatus(EMovementStatus::EMS_Normal);
		GetMesh()->bPauseAnims = false;
		GetMesh()->bNoSkeletonUpdate = false;
	}
}

// Called by BeginPlay(), it loads characters stats and weapon from a previous level into the next one
void AMainCharacter::LoadGameNoSwitch()
{
	UFirstSaveGame* Load = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	UFirstSaveGame* LoadObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)
	{
		Health = LoadObject->CharacterStats.Health;
		MaxHealth = LoadObject->CharacterStats.MaxHealth;
		Stamina = LoadObject->CharacterStats.Stamina;
		MaxStamina = LoadObject->CharacterStats.MaxStamina;
		Coins = LoadObject->CharacterStats.Coins;


		if (WeaponContainer)
		{
			AWeaponContainerActor* Container = GetWorld()->SpawnActor<AWeaponContainerActor>(WeaponContainer);
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName;
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName))
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]);
						if (Weapon)
						{
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);

						}
					}
				}
			}
		}

		// Resetting Main Character to a playable state when loading after dying
		SetMovementStatus(EMovementStatus::EMS_Normal);
		GetMesh()->bPauseAnims = false;
		GetMesh()->bNoSkeletonUpdate = false;
	}
}

// Called from MainCharacter_BP via any input key bind
void AMainCharacter::ShowPickupLocations()
{
	for (auto Location : PickupLocations) // Range based for loop (auto is changed to FVector by the compiler)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Red, 10.f, .5f);
	}
}