// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutonomousDrivingWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UAutonomousDrivingWheelRear::UAutonomousDrivingWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}