// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutonomousDrivingWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UAutonomousDrivingWheelFront::UAutonomousDrivingWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}