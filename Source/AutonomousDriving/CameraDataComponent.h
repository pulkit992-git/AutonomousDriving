// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "CameraDataComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AUTONOMOUSDRIVING_API UCameraDataComponent : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	UCameraDataComponent();

	UFUNCTION(BlueprintCallable, Category = "AI Perception")
	void CaptureFrame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
	UTextureRenderTarget2D* CameraViewTarget;
};
