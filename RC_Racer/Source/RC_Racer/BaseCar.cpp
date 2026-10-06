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

	// for each wheel get Acceleration forces using relevant wheel data
	for (const auto& Pair : WheelData)
	{
		GetAccelerationForce(Pair.Key);
	}
	
	for (const auto& Pair : WheelData)
	{
		ApplyWheelForces(Pair.Key);
	}
	
	for (const auto& Pair : WheelData)
	{
		ApplyWheelRotation(Pair.Key, DeltaTime);
	}
	
	
	
}


void ABaseCar::ApplySuspensionForce(EWheelType WheelType, float DeltaTime)
{
	FWheelData* Data = WheelData.Find(WheelType);
	
	FHitResult Hit;
	FVector Start = CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation);
	FVector End = Start - CarMesh->GetUpVector() * (SuspensionLength + Data->WheelRadius);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);
	
	if (bHit)
	{
		Data->bIsInContact = true;
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
				(FVector::UpVector * SuspensionLength/2) +
				(FVector::UpVector * Compression) 				
		);
		
		FVector TotalForce = SpringForce + DamperForce;
		Data->WheelLoad = (FVector::DotProduct(TotalForce, CarMesh->GetUpVector())/ 100);

		Data->PreviousWheelCompression = Compression;
	}

	else
	{
		Data->bIsInContact = false;
		Data->WheelCompression = 0.f;
		Data->PreviousWheelCompression = 0.f;
		
		
		if (Data->WheelLoad >= 0.1f) Data->WheelLoad -= .1f;
		else Data->WheelLoad = 0.f;
		
		Data->WheelMesh->SetRelativeLocation(
				Data->SuspensionLocation -
				(FVector::UpVector * SuspensionLength/2) 				
		);
	}
	
	// DrawDebugString(
	// 			GetWorld(),
	// 			CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation + FVector::UpVector * 300.f),
	// 			FString::Printf(TEXT("WheelLoad: %.1f"), Data->WheelLoad),
	// 			nullptr,
	// 			FColor::White,
	// 			0.0f,
	// 			true);
	
}

void ABaseCar::GetAccelerationForce(EWheelType Wheel)
{
	// Rear-wheel drive, so it ignores front wheels
	if (Wheel == EWheelType::FL || Wheel == EWheelType::FR)
	{
		return;
	}
	
	FWheelData* Data = WheelData.Find(Wheel);
	
	if (Data->bIsInContact)
	{

		// Get speed at the point of the wheel
		FVector wheelVelocity = CarMesh->GetPhysicsLinearVelocityAtPoint(Data->SurfaceLocation);


		// Get the speed in the forward direction of the wheel
		FVector wheelForward = Data->WheelForwardVector;
		float CurrentSpeed = FVector::DotProduct(wheelVelocity, wheelForward);


		// Rate that reduces engine power by how close the speed is to the max speed
		float EngineReductionRate = 1.f - (CurrentSpeed / EngineMaxSpeed);


		FVector AccelerationForce = Data->WheelForwardVector * (EngineReductionRate * EnginePower * ThrottleAmount);	
		Data->WantedAccelerationForce = AccelerationForce;	
		
		// DrawDebugLine(
		// GetWorld(),
		// CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation),
		// CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation) + Data->WantedAccelerationForce * .001f,
		// FColor::Red,
		// false,
		// 0.0f,
		// 0,
		// 2.0f
		// );
	}
	else
	{
		Data->WantedAccelerationForce = FVector::ZeroVector;     
	}

}

void ABaseCar::GetBrakeForce(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);

	if (Data->bIsInContact)
	{
		// Get speed at the point of the wheel
		FVector wheelVelocity = CarMesh->GetPhysicsLinearVelocityAtPoint(Data->SurfaceLocation);		
		
		// Get the speed in the forward direction of the wheel
		FVector wheelForward = Data->WheelForwardVector;
		float linearVelocity = FVector::DotProduct(wheelVelocity,wheelForward);
		
		// Get the force of brakes
		float LinearBrakeForce = -linearVelocity * BrakeStrength * BrakeAmount;  
		
		// Get the force of the handbrake
		float LinearHandBrakeForce = 0.f;
		if (bISHandbrakeOn && (Wheel == EWheelType::BL || Wheel == EWheelType::BR))
		{
			LinearHandBrakeForce = -linearVelocity * HandbrakeStrength;
		}
		
		// apply the highest force betweeen handbrake and brake 
		float LinearForce;
		float HighestForce = FMath::Max(FMath::Abs(LinearBrakeForce), FMath::Abs(LinearHandBrakeForce));	
		if (HighestForce == FMath::Abs(LinearBrakeForce))
		{
			LinearForce = LinearBrakeForce;
		}
		else
		{
			LinearForce = LinearHandBrakeForce;
		}
		FVector wantedWheelBrakeForce = LinearForce * wheelForward;
				
		
		Data->WantedBrakeForce = wantedWheelBrakeForce;
		
		// DrawDebugLine(
		// GetWorld(),
		// CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation),
		// CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation) + Data->WantedBrakeForce * .001f,
		// FColor::Blue,
		// false,
		// 0.0f,
		// 0,
		// 2.0f
		// );
	}
	else
	{
		Data->WantedBrakeForce = FVector::ZeroVector;
	}
}

void ABaseCar::GetGripForce(EWheelType Wheel)
{
	FWheelData* Data = WheelData.Find(Wheel);

	if (Data->bIsInContact)
	{
		FVector wheelVelocity = CarMesh->GetPhysicsLinearVelocityAtPoint(Data->SurfaceLocation);	
		
		Data->WheelForwardSpeed = FVector::DotProduct(wheelVelocity,	Data->WheelForwardVector);
		
		FVector WheelUp = CarMesh->GetUpVector();

		FVector WheelRight =
			FVector::CrossProduct(
				WheelUp,
				Data->WheelForwardVector
			).GetSafeNormal();
		float sideVelocity = FVector::DotProduct(wheelVelocity,	WheelRight);
		
		float SideForce = -sideVelocity * Data->WheelLoad * LateralStiffness;
		
		FVector wantedWheelSideForce = SideForce * WheelRight;
		
		Data->WantedSteeringForce = wantedWheelSideForce;
		
		
		
		
		// DrawDebugLine(
		// GetWorld(),
		// CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation),
		// CarMesh->GetComponentTransform().TransformPosition(Data->SuspensionLocation) + Data->WantedSteeringForce * .001f,
		// FColor::White,
		// false,
		// 0.0f,
		// 0,
		// 2.0f
		// );
	}
	else
	{
		Data->WantedSteeringForce = FVector::ZeroVector;
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
	
	float ClampedWheelForceMagnitude = FMath::Clamp(ForceMagnitude, 0, MaxWheelForce);
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
	
}

void ABaseCar::ApplyWheelRotation(EWheelType Wheel, float DeltaTime)
{
	FWheelData* Data = WheelData.Find(Wheel);

	
	if (Wheel == EWheelType::FL || Wheel == EWheelType::FR)
	{
		float SteerAngle = SteerAmount * MaxSteeringAngle;
		
		float AngularVelocity = Data->WheelForwardSpeed / Data->WheelRadius;
		float ChangeInDegrees =  FMath::RadiansToDegrees(AngularVelocity) * DeltaTime;
		
		Data->WheelRotation -= ChangeInDegrees;
		if (Data->WheelRotation > 360.f) Data->WheelRotation -= 360.f; // Keeps value within 0 -360
		if (Data->WheelRotation < 0.f) Data->WheelRotation += 360.f; // Keeps value within 0 -360
		
		Data->WheelMesh->SetRelativeRotation(FRotator(Data->WheelRotation, SteerAngle, 0));
	}
	else
	{
		float NormalAngularVelocity = Data->WheelForwardSpeed / Data->WheelRadius;
		float EngineAddedVelocity = 0.f;
		
		if (Data->WantedAccelerationForce.Size() > Data->MaxTotalWheelForce)
		{
			float Slippage = Data->WantedAccelerationForce.Size() - Data->MaxTotalWheelForce;
			
			float WheelSpinFactor = 0.00005f;
			EngineAddedVelocity = Slippage * WheelSpinFactor;	
		}
		
		float AngularVelocity = NormalAngularVelocity + EngineAddedVelocity;		
		float ChangeInDegrees =  FMath::RadiansToDegrees(AngularVelocity) * DeltaTime;		
		
		if (bISHandbrakeOn)
		{
			ChangeInDegrees = 0.f;
		}
		
		Data->WheelRotation -= ChangeInDegrees;
		if (Data->WheelRotation > 360.f) Data->WheelRotation -= 360.f; // Keeps value within 0 -360
		if (Data->WheelRotation < 0.f) Data->WheelRotation += 360.f; // Keeps value within 0 -360		
		
		Data->WheelMesh->SetRelativeRotation(FRotator(Data->WheelRotation, 0, 0));
	}
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
	bISHandbrakeOn = HandbrakeState;
}

void ABaseCar::Jump()
{
	// Get how many wheels are touching the ground
	int WheelsInContact = 0;
	for (const auto& Pair : WheelData)
	{
		if (Pair.Value.bIsInContact == true)
		{
			WheelsInContact++;
		}
	}
	
	// if enough wheels are touching ground, perform jump
	if (WheelsInContact >= MinContactedWheelsForJump)
	{
		FVector UpImpulse = FVector::UpVector * JumpStrength;
		CarMesh->AddImpulse(UpImpulse);
	}

}

void ABaseCar::BackwardAbility()
{
}

void ABaseCar::ForwardAbility()
{
}


