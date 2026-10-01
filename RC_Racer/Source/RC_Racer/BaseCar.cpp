// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCar.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseCar::ABaseCar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>("CarMesh");
	RootComponent = CarMesh;
	CarMesh->SetSimulatePhysics(true);
	CarMesh->SetMassOverrideInKg(NAME_None, 1000.f);
	CarMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CarMesh->SetCollisionResponseToAllChannels(ECR_Block);
	
	FLWheel = CreateDefaultSubobject<UStaticMeshComponent>("FLWheel");
	FRWheel = CreateDefaultSubobject<UStaticMeshComponent>("FRWheel");
	BLWheel	= CreateDefaultSubobject<UStaticMeshComponent>("BLWheel");
	BRWheel = CreateDefaultSubobject<UStaticMeshComponent>("BRWheel");
	
	FLWheel->SetupAttachment(CarMesh);
	FRWheel->SetupAttachment(CarMesh);	
	BLWheel->SetupAttachment(CarMesh);
	BRWheel->SetupAttachment(CarMesh);	
	
	FLWheel->SetSimulatePhysics(false);
	FRWheel->SetSimulatePhysics(false);
	BLWheel->SetSimulatePhysics(false);
	BRWheel->SetSimulatePhysics(false);
	
	FLWheel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FRWheel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BLWheel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BRWheel->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	FLWheel->SetRelativeLocation(FVector(100.f, -100.f, -50.f));
	FRWheel->SetRelativeLocation(FVector(100.f, 100.f, -50.f));
	BLWheel->SetRelativeLocation(FVector(-100.f, -100.f, -50.f));
	BRWheel->SetRelativeLocation(FVector(-100.f, 100.f, -50.f));
	
	WheelData.Add(EWheelType::FL, FWheelData());
	WheelData.Add(EWheelType::FR, FWheelData());
	WheelData.Add(EWheelType::BL, FWheelData());
	WheelData.Add(EWheelType::BR, FWheelData());
}

// Called when the game starts or when spawned
void ABaseCar::BeginPlay()
{
	Super::BeginPlay();
	
	FWheelData& FLWheelData = WheelData[EWheelType::FL];	
	FLWheelData.SuspensionLocation = FLWheel->GetRelativeLocation() + CarMesh->GetUpVector() * SuspensionLength/2;
	FLWheelData.WheelRadius = FLWheel->Bounds.BoxExtent.Z;
	FLWheelData.WheelMesh = FLWheel;
	
	FWheelData& FRWheelData = WheelData[EWheelType::FR];
	FRWheelData.SuspensionLocation = FRWheel->GetRelativeLocation() + CarMesh->GetUpVector() * SuspensionLength/2;
	FRWheelData.WheelRadius = FRWheel->Bounds.BoxExtent.Z;
	FRWheelData.WheelMesh = FRWheel;
	
	FWheelData& BLWheelData = WheelData[EWheelType::BL];
	BLWheelData.SuspensionLocation = BLWheel->GetRelativeLocation() + CarMesh->GetUpVector() * SuspensionLength/2;
	BLWheelData.WheelRadius = BLWheel->Bounds.BoxExtent.Z;
	BLWheelData.WheelMesh = BLWheel;
	
	FWheelData& BRWheelData = WheelData[EWheelType::BR];
	BRWheelData.SuspensionLocation = BRWheel->GetRelativeLocation() + CarMesh->GetUpVector() * SuspensionLength/2;
	BRWheelData.WheelRadius = BRWheel->Bounds.BoxExtent.Z;
	BRWheelData.WheelMesh = BRWheel;
}

// Called every frame
void ABaseCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// for each wheel apply suspension forces using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		ApplySuspensionForce(Pair.Key, DeltaTime);
	}
	
	// for each wheel apply Acceleration forces using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		GetAccelerationForce(Pair.Key);
	}
	
	// for each wheel apply Steering using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		ApplySteeringAngle(Pair.Key);
	}
	
	// for each wheel apply Grip forces using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		GetGripForce(Pair.Key);
	}
	
	for (const auto& Pair : WheelData)
	{
		ApplyWheelForces(Pair.Key);
	}
	
	
	
}


void ABaseCar::ApplySuspensionForce(EWheelType WheelType, float DeltaTime)
{
	FWheelData* Data = WheelData.Find(WheelType);
	
	FHitResult Hit;
	FVector Start = CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation);
	FVector End = Start - CarMesh->GetUpVector() * (SuspensionLength + Data->WheelRadius);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility
	);
	
	if (bHit)
	{
		Data->bISInContact = true;
		Data->SurfaceLocation = Hit.Location;
		
		float Distance = (Hit.Location - Start).Size();
		
		// Spring Force
		float Compression = SuspensionLength/2 - (Distance - Data->WheelRadius);	
		Data->WheelCompression = Compression;
		
		FVector SpringForce = CarMesh->GetUpVector() * Compression * SuspensionForce; 		
		CarMesh->AddForceAtLocation(SpringForce, Start);
		
		// Damper Force
		FVector DamperForce = (
			(Data->WheelCompression - Data->PreviousWheelCompression)
			/ DeltaTime
			*DamperRate 
			*CarMesh->GetUpVector());	
		
		CarMesh->AddForceAtLocation(DamperForce, Start);
		
		Data->WheelMesh->SetRelativeLocation(
				Data->SuspensionLocation -
				(CarMesh->GetUpVector() * SuspensionLength/2) +
				(CarMesh->GetUpVector() * Compression) 				
		);
		
		FVector TotalForce = SpringForce + DamperForce;
		Data->WheelLoad = (FVector::DotProduct(TotalForce, CarMesh->GetUpVector())/ 100);

		Data->PreviousWheelCompression = Compression;
	}

	else
	{
		Data->bISInContact = false;
		Data->WheelLoad = 0.0f;
		Data->WheelMesh->SetRelativeLocation(
				Data->SuspensionLocation -
				(CarMesh->GetUpVector() * SuspensionLength/2) 				
		);
	}
	
}

void ABaseCar::GetAccelerationForce(EWheelType Wheel)
{
	// Rear-wheel drive, so it ignores front wheels
	if (Wheel == EWheelType::FL || Wheel == EWheelType::FR)
	{
		return;
	}
	
	FWheelData* Data = WheelData.Find(Wheel);
	
	if (Data->bISInContact)
	{
		FVector AccelerationForce = CarMesh->GetForwardVector() * (EnginePower * ThrottleAmount);	
		Data->WantedAccelerationForce = AccelerationForce;
		
		
			DrawDebugLine(
		GetWorld(),
		Data->SurfaceLocation,
		Data->SurfaceLocation + AccelerationForce,
		FColor::Red,
		false,
		0.0f,
		0,
		2.0f
		);
		
		
	}

}

void ABaseCar::GetBrakeForce(EWheelType Wheel)
{
}

void ABaseCar::GetGripForce(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);

	if (Data->bISInContact)
	{
		FVector wheelVelocity = CarMesh->GetPhysicsLinearVelocityAtPoint(Data->SurfaceLocation);		
		
		FVector wheelRight = Data->WheelMesh->GetRightVector();
		float sideVelocity = FVector::DotProduct(wheelVelocity,	wheelRight);
		FVector wantedWheelSideForce = -sideVelocity * Data->WheelLoad * wheelRight;
		
		Data->WantedSteeringForce = wantedWheelSideForce;
	}
}

void ABaseCar::ApplyWheelForces(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);
	
	float ForceMagnitude = sqrt(
		Data->WantedSteeringForce.Size() *
		Data->WantedSteeringForce.Size() + 
		Data->WantedAccelerationForce.Size() *
		Data->WantedAccelerationForce.Size()); 
	
	FVector Direction = Data->WantedSteeringForce + Data->WantedAccelerationForce;
	
	float MaxWheelForce = FrictionCoefficient * Data->WheelLoad * GripFactor * 100;
	
	float ClampedWheelForceMagnitude = FMath::Clamp(ForceMagnitude, 0.f, MaxWheelForce);
	FVector CombinedClampedWheelForce = Direction.GetSafeNormal() * ClampedWheelForceMagnitude;

	CarMesh->AddForceAtLocation(CombinedClampedWheelForce, Data->SurfaceLocation);			

	
	DrawDebugString(
			GetWorld(),
			CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation),
			FString::Printf(TEXT("MaxWheelForce: %.1f"), MaxWheelForce),
			nullptr,
			FColor::White,
			0.0f,
			true);
}

void ABaseCar::ApplySteeringAngle(EWheelType Wheel)
{
	if (Wheel == EWheelType::BL || Wheel == EWheelType::BR)
	{
		return;
	}
	FWheelData* Data = WheelData.Find(Wheel);
	
	float SteerAngle = SteerAmount * MaxSteeringAngle;
	Data->WheelMesh->SetRelativeRotation(FRotator(0, SteerAngle, 0));
}


void ABaseCar::Accelerate(float Throttle)
{
	ThrottleAmount = Throttle;
}

void ABaseCar::Brake(float BrakeInput)
{
	BrakeAmount = BrakeInput;
}

void ABaseCar::Steer(float SteerInput)
{ 
	SteerAmount = SteerInput;
}

void ABaseCar::ToggleHandbrake(bool HandbrakeState)
{
}

void ABaseCar::BackwardAbility()
{
}

void ABaseCar::ForwardAbility()
{
}


