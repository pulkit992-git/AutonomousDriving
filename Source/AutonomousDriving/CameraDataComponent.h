// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Engine/EngineTypes.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
	bool bIsRecording = false;

	UFUNCTION(BlueprintCallable, Category = "AI Perception")
	void StartRecording();

	UFUNCTION(BlueprintCallable, Category = "AI Perception")
	void StopRecording();

private:

	void SaveImageToDisk(const TArray<FColor>& RawPixels, float SteeringAngle, int32 Width, int32 Height);

	void WriteCSVRow(const FString& Filename, float SteeringAngle);

	void InitCSV();

	IImageWrapperModule* ImageWrapperModule = nullptr;

	int32 FrameCounter = 0;

	FString CaptureDir;
	FString CSVPath;
	bool bCSVInitialised = false;

	UPROPERTY()
	UChaosWheeledVehicleMovementComponent* VehicleMovement;

	FTimerHandle RecordTimerHandle;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
