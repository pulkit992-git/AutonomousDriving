// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AutonomousDrivingPawn.h"
#include "AutonomousDrivingSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class AAutonomousDrivingSportsCar : public AAutonomousDrivingPawn
{
	GENERATED_BODY()
	
public:

	AAutonomousDrivingSportsCar();
};
