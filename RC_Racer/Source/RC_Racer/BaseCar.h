// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "WheelType.h"
#include "WheelData.h"
#include "BaseCar.generated.h"

UCLASS()
class RC_RACER_API ABaseCar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void Accelerate(float ThrottleInput);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void Brake(float BrakeInput);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void Steer(float SteerInput);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void ToggleHandbrake(bool HandbrakeState);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void Jump();
	
	

	
	UFUNCTION()
	void ApplySuspensionForce(EWheelType Wheel, float DeltaTime);
	
	UFUNCTION()
	void GetAccelerationForce(EWheelType Wheel);
	
	UFUNCTION()
	void GetBrakeForce(EWheelType Wheel);
	
	UFUNCTION()
	void GetGripForce(EWheelType Wheel);
	
	UFUNCTION()
	void ApplyWheelForces(EWheelType Wheel);
	
	UFUNCTION()
	void ApplySteeringAngle(EWheelType Wheel);
	
	UFUNCTION()
	void ApplyWheelRotation(EWheelType Wheel, float DeltaTime);
	
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTyreSmoke(EWheelType Wheel, ESurfaceType Surface, bool bIsOn);

	UFUNCTION(BlueprintCallable)
	void SetEngineBoosted();
	
	UFUNCTION(BlueprintCallable)
	void SetEngineSlowed();
	
	UFUNCTION(BlueprintCallable)
	void ResetEngine();
	
	
	UPROPERTY()
	float ThrottleAmount;
	
	UPROPERTY()
	float BrakeAmount;
	
	UPROPERTY()
	float SteerAmount;
	
	UPROPERTY()
	bool bISHandbrakeOn = false;
	
	
	UPROPERTY()
	bool bIsSuspensionActive = true;
	
	// deactivates the suspension forces on jump for this length (used to stop car flipping when jumping)
	UPROPERTY()
	float SuspensionDeactivationTimer = 0.2f;
	
	UPROPERTY()
	float SuspensionDeactivationTime = 0.2f;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<EWheelType, FWheelData> WheelData;	
	
	//////////////////////////////
    // Mesh
    /////////////////////////////	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Meshes")
	UStaticMeshComponent* CarMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Meshes")
	UStaticMeshComponent* FLWheel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Meshes")
	UStaticMeshComponent* FRWheel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Meshes")
	UStaticMeshComponent* BLWheel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Meshes")
	UStaticMeshComponent* BRWheel;

	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector FLWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector FRWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector BLWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector BRWheelLocation;
	
	
	//////////////////////////////
	// Surfaces
	/////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - Surface")
	TObjectPtr<UPhysicalMaterial> RoadMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - Surface")
	TObjectPtr<UPhysicalMaterial> CarpetMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - Surface")
	TObjectPtr<UPhysicalMaterial> DirtMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - Surface")
	TObjectPtr<UPhysicalMaterial> SandMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - Surface")
    TObjectPtr<UPhysicalMaterial> OilMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - Surface")
    TObjectPtr<UPhysicalMaterial> GrippyMaterial;
	

	//////////////////////////////
	// Surface Friction
	/////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - SurfaceFriction")
	float RoadFriction = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - SurfaceFriction")
	float CarpetFriction = 1.0f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - SurfaceFriction")
	float DirtFriction = 0.7f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - SurfaceFriction")
	float SandFriction =  0.6f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - SurfaceFriction")
	float OilFriction = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - SurfaceFriction")
	float GrippyFriction = 1.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="BaseCar - SurfaceFriction")
	float NoneFriction = 1.f;
	
	
	
	//////////////////////////////
	// Suspension
	/////////////////////////////
	UPROPERTY(EditAnywhere, Category="BaseCar - Suspension")
	float SuspensionLength = 40.f;
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Suspension")
	float SuspensionForce = 10000.f;
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Suspension")
	float DamperRate = 600.f;
	
	
	//////////////////////////////
	// Steering
	/////////////////////////////
	UPROPERTY(EditAnywhere, Category="BaseCar - Steering")
	float MaxSteeringAngle = 35.f;
	
	UPROPERTY()
	float WheelBase = 200.f;
	
	UPROPERTY()
	float FrontTrackWidth = 200.f;
	
	//////////////////////////////
	// Engine
	/////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Engine")
	float EnginePower = 200000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Engine")
	float StandardEnginePower =  200000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Engine")
	float BoostedEnginePower = 300000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Engine")
	float SlowedEnginePower = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseCar - Engine")
	float MaxSpeed = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseCar - Engine")
	float StandardMaxSpeed = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseCar - Engine")
	float BoostedMaxSpeed = 2500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseCar - Engine")
	float SlowedMaxSpeed = 1500.f;
	
	UPROPERTY(EditAnywhere, Category = "BaseCar - Engine")
    float ReverseMaxSpeed = 1500.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "BaseCar - Engine")
	bool bIsInReverse = false;
	
	UPROPERTY(EditAnywhere, Category = "BaseCar - Engine")
	float MinSpeedForReverse = 100.f;
	
	
	//////////////////////////////
	// Grip
	/////////////////////////////
	UPROPERTY(EditAnywhere, Category="BaseCar - Grip")
	float GripFactor = 1.f;
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Grip")
	float FrictionCoefficient = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Grip")
	float LateralStiffness = 2.0f;
	
	//////////////////////////////
	// Braking
	/////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Braking")
	float BrakeStrength = 5000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Braking")
	float HandbrakeStrength = 4000.f;
	
	//////////////////////////////
	// Jump
	/////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Jump")
	float JumpStrength = 500000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Jump")
	int MinContactedWheelsForJump = 2;
	
	
	//////////////////////////////
	// Effects
	/////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Effects")
	float TyreSmokeRate = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Effects")
	float DirtEffectRate = .8f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseCar - Effects")
	float OilEffectRate = 1.f;

};
