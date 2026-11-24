// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheelType.h"
#include "Components/StaticMeshComponent.h"
#include "SurfaceType.h"
#include "WheelData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct RC_RACER_API FWheelData
{

	GENERATED_BODY()

	

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WheelRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WheelCompression;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float PreviousWheelCompression;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector SuspensionLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsInContact;
	
	UPROPERTY()
	UStaticMeshComponent* WheelMesh;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector WantedAccelerationForce;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector WantedBrakeForce;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector WantedSteeringForce;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxTotalWheelForce;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector SurfaceLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WheelLoad;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FrictionCoefficient = 1.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector WheelForwardVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WheelForwardSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WheelRotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SteerAngle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESurfaceType SurfaceType;
};
