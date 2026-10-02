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
	
	UPROPERTY()
	float ThrottleAmount;
	
	UPROPERTY()
	float BrakeAmount;
	
	UPROPERTY()
	float SteerAmount;
	
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	UStaticMeshComponent* CarMesh;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	UStaticMeshComponent* FLWheel;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	UStaticMeshComponent* FRWheel;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	UStaticMeshComponent* BLWheel;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	UStaticMeshComponent* BRWheel;

	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector FLWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector FRWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector BLWheelLocation;
	
	UPROPERTY(BlueprintReadWrite, Category="Wheels")
	FVector BRWheelLocation;
	
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	TMap<EWheelType, FWheelData> WheelData;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	float SuspensionLength = 40.f;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	float SuspensionForce = 10000.f;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	float DamperRate = 8000.f;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	float MaxSteeringAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category="Engine")
	float EnginePower = 2000.f;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	float GripFactor = 1.f;
	
	UPROPERTY(EditAnywhere, Category="Wheels")
	float FrictionCoefficient = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wheels")
	float LateralStiffness = 5000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Brakes")
	float BrakeFactor = 1.f;
};
