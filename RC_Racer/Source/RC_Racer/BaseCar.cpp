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
	
	// for each wheel apply Steering using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		ApplySteeringAngle(Pair.Key);
	}
	
	// for each wheel get Acceleration forces using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		GetAccelerationForce(Pair.Key);
	}
	
	// for each wheel get Brake forces using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		GetBrakeForce(Pair.Key);
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
		FVector AccelerationForce = Data->WheelForwardVector * (EnginePower * ThrottleAmount);	
		Data->WantedAccelerationForce = AccelerationForce;	
	}

}

void ABaseCar::GetBrakeForce(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);

	if (Data->bISInContact)
	{
		FVector wheelVelocity = CarMesh->GetPhysicsLinearVelocityAtPoint(Data->SurfaceLocation);		
		
		FVector wheelForward = Data->WheelForwardVector;
		float linearVelocity = FVector::DotProduct(wheelVelocity,wheelForward);
		
		float LinearForce = -linearVelocity * BrakeFactor * BrakeAmount;  
		
		FVector wantedWheelBrakeForce = LinearForce * wheelForward;
		
		
		
		Data->WantedBrakeForce = wantedWheelBrakeForce;
	}
}

void ABaseCar::GetGripForce(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);

	if (Data->bISInContact)
	{
		FVector wheelVelocity = CarMesh->GetPhysicsLinearVelocityAtPoint(Data->SurfaceLocation);		
		
		FVector wheelRight = Data->WheelMesh->GetRightVector();
		float sideVelocity = FVector::DotProduct(wheelVelocity,	wheelRight);
		
		float SideForce = -sideVelocity * Data->WheelLoad * LateralStiffness;
		
		FVector wantedWheelSideForce = SideForce * wheelRight;
		
		Data->WantedSteeringForce = wantedWheelSideForce;
		
		DrawDebugString(
				GetWorld(),
				CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation),
				FString::Printf(TEXT("sideVelocity: %.1f"), sideVelocity),
				nullptr,
				FColor::White,
				0.0f,
				true);
		
		
		DrawDebugLine(
		GetWorld(),
		CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation),
		CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation) + wheelRight * 100.f,
		FColor::Red,
		false,
		0.0f,
		0,
		2.0f
		);
	}
}

void ABaseCar::ApplyWheelForces(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);
	
	

	
	FVector CombinedForce =
	Data->WantedSteeringForce +
	Data->WantedAccelerationForce + 
	Data->WantedBrakeForce;

	float ForceMagnitude = CombinedForce.Size();
	
	FVector Direction = CombinedForce.GetSafeNormal();
	
	float MaxWheelForce = FrictionCoefficient * Data->WheelLoad * GripFactor * 100;
	
	float ClampedWheelForceMagnitude = FMath::Clamp(ForceMagnitude, -MaxWheelForce, MaxWheelForce);
	FVector CombinedClampedWheelForce = Direction * ClampedWheelForceMagnitude;

	CarMesh->AddForceAtLocation(CombinedClampedWheelForce, Data->SurfaceLocation);			

	
	
}

void ABaseCar::ApplySteeringAngle(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);

	if (Wheel == EWheelType::BL || Wheel == EWheelType::BR)
	{
		Data->WheelForwardVector = CarMesh->GetForwardVector();
		return;
	}
	
	float SteerAngle = SteerAmount * MaxSteeringAngle;
	
	Data->WheelForwardVector =
		CarMesh->GetForwardVector().RotateAngleAxis(
			SteerAngle,
			CarMesh->GetUpVector()
		);
	
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


