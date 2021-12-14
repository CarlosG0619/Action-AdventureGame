// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating box component for the floor switch overlap
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	// Setting box component collision
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	// Box component size
	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));
	// Creating Floor Switch StaticMesh
	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());
	// Creating Door StaticMesh
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());
	// Timer before door closes after stepping out of the floor switch
	SwitchTimer = 2.0f;
	// Player is not on top of the switch by default
	bCharacterOnSwitch = false;
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();
	// Preparing TriggerBox to call OnOverlapBegin() when player steps on the floor switch box component
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	// Preparing TriggerBox to call OnOverlapEnd() when player steps out of it
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);
	// Setting initial values for the door and floor switch meshes
	InitialDoorLocation = Door->GetComponentLocation();
	InitialSwitchLocation = FloorSwitch->GetComponentLocation();
	
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when player enters floor switch collision
void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin.")); // Check to verify if the player is overlapping with the box component
	if (!bCharacterOnSwitch) bCharacterOnSwitch = true; // Player is on top of the switch
	RaiseDoor(); // Door raises
	LowerFloorSwitch(); // Floor switch is pressed down by the character
}

// Called when player exists floor switch collision
void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End.")); // Check to verify if the player is no longer overlapping with the box component
	if (bCharacterOnSwitch) bCharacterOnSwitch = false; // Player stepped out of the switch
	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::CloseDoor, SwitchTimer); // Timer for the door to close after the time set by SwitchTimer
}

// Called when player activates the floor switch
void AFloorSwitch::UpdateDoorLocation(float z)
{
	// Moving the door location
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += z;
	Door->SetWorldLocation(NewLocation);
}

// Called when player activates the floor switch
void AFloorSwitch::UpdateFloorSwitchLocation(float z)
{
	// Moving the floor switch location
	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z += z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

// Called when player steps out of the floor switch
void AFloorSwitch::CloseDoor()
{
	if (!bCharacterOnSwitch)
	{
		LowerDoor();
		RaiseFloorSwitch();
	}

}