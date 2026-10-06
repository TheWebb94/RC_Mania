// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
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
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void BackwardAbility();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void ForwardAbility();

	
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
	
	UPROPERTY()
	float ThrottleAmount;
	
	UPROPERTY()
	float BrakeAmount;
	
	UPROPERTY()
	float SteerAmount;
	
	UPROPERTY()
	bool bISHandbrakeOn = false;
	
	
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Meshes")
	UStaticMeshComponent* CarMesh;
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Meshes")
	UStaticMeshComponent* FLWheel;
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Meshes")
	UStaticMeshComponent* FRWheel;
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Meshes")
	UStaticMeshComponent* BLWheel;
	
	UPROPERTY(EditAnywhere, Category="BaseCar - Meshes")
	UStaticMeshComponent* BRWheel;

	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector FLWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector FRWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector BLWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector BRWheelLocation;
	
	
	
	
	
	
	UPROPERTY()
	TMap<EWheelType, FWheelData> WheelData;	
	
	
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
	float MaxSteeringAngle = 30.f;
	
	
	//////////////////////////////
	// Engine
	/////////////////////////////
	UPROPERTY(EditAnywhere, Category="BaseCar - Engine")
	float EnginePower = 800000.f;

	UPROPERTY(EditAnywhere, Category = "BaseCar - Engine")
	float EngineMaxSpeed = 800.f;
	
	
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
	
};
