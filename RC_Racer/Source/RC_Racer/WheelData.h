// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheelType.h"
#include "Components/StaticMeshComponent.h"
#include "WheelData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct RC_RACER_API FWheelData
{

	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWheelType WheelType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelCompression;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PreviousWheelCompression;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SuspensionLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bISInContact;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WheelMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WantedAccelerationForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WantedSteeringForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SurfaceLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelLoad;
};
