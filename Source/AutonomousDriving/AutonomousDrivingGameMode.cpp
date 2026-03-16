// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutonomousDrivingGameMode.h"
#include "AutonomousDrivingPlayerController.h"

AAutonomousDrivingGameMode::AAutonomousDrivingGameMode()
{
	PlayerControllerClass = AAutonomousDrivingPlayerController::StaticClass();
}
