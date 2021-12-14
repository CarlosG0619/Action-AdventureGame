// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Creating the mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	// Initializing values
	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);
	InterpSpeed = 2.0f;
	InterpTime = 1.0f;
	bInterping = false;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	// StartPoint is the platform initial location
	StartPoint = GetActorLocation();
	// Since EndPoint is local to the platform it needs to be changed to world location
	EndPoint += StartPoint;
	// Timer for the interpolation
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
	// Initializing distance, since the points are vectors Size() is used to get a float
	Distance = (EndPoint - StartPoint).Size();
	
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bInterping) // Using Interpolation function to move the platform between start point and end point
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		
		if (Distance - DistanceTraveled <= 1.f) // Preparing the platform to interpolate back to start point
		{
			ToggleInterping();
			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}

}

// Toggles interpolation on or off 
void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

// Swaping vectors for the start and end points of the interpolation
void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
	FVector Temp = VecOne;
	VecOne = VecTwo;
	VecTwo = Temp;
}