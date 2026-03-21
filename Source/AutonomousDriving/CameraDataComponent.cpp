// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDataComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TextureResource.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

UCameraDataComponent::UCameraDataComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraDataComponent::BeginPlay()
{
	Super::BeginPlay();

	ImageWrapperModule = &FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	FString SessionName = FString::Printf(TEXT("Session_%s"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	CaptureDir = FPaths::ProjectSavedDir() / TEXT("Captures") / SessionName;

	//Force the folder to exist
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.DirectoryExists(*CaptureDir))
	{
		PlatformFile.CreateDirectory(*CaptureDir);
	}

	CSVPath = CaptureDir / TEXT("labels.csv");

	//Start the timer (Check if the World exists first)
	if (GetWorld())
	{
		// 0.1f = 10 times per second
		GetWorld()->GetTimerManager().SetTimer(RecordTimerHandle, this, &UCameraDataComponent::CaptureFrame, 0.1f, true);
		UE_LOG(LogTemp, Warning, TEXT("DATA RECORDER STARTED!"));
	}
}

void UCameraDataComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopRecording();
	UE_LOG(LogTemp, Warning, TEXT("CameraDataComponent stopped, Total frame %d"), FrameCounter);
}

void UCameraDataComponent::InitCSV()
{
	FString Header = TEXT("filename, steering, throttle, brake, timestamp\n");
	FFileHelper::SaveStringToFile(Header, *CSVPath);
	bCSVInitialised = true;
	UE_LOG(LogTemp, Warning, TEXT("CSV initialised at %s"), *CSVPath);
}

void UCameraDataComponent::CaptureFrame()
{
	if (!TextureTarget || !bIsRecording)
		return;

	if (!VehicleMovement) {
		AActor* Owner = GetOwner();
		VehicleMovement = Owner->FindComponentByClass<UChaosWheeledVehicleMovementComponent>();
	}

	float CurrentSteering = VehicleMovement ? VehicleMovement->GetSteeringInput() : 0.0f;
	float CurrentThrottle = VehicleMovement ? VehicleMovement->GetThrottleInput() : 0.0f;
	float CurrentBrake = VehicleMovement ? VehicleMovement->GetBrakeInput() : 0.0f;

	FTextureRenderTargetResource* RenderTargetResource = TextureTarget->GameThread_GetRenderTargetResource();

	if (!RenderTargetResource)
		return;

	TArray<FColor> RawPixels;
	RenderTargetResource->ReadPixels(RawPixels);

	if (RawPixels.Num() > 0)
	{
		int32 Width = TextureTarget->SizeX;
		int32 Height = TextureTarget->SizeY;

		SaveImageToDisk(RawPixels, CurrentSteering, Width, Height);

		WriteCSVRow(FString::Printf(TEXT("frame_%05d.jpg"), FrameCounter - 1), CurrentSteering);
	}
}

void UCameraDataComponent::SaveImageToDisk(const TArray<FColor>& RawPixels, float SteeringAngle, int32 Width, int32 Height)
{
	if (!ImageWrapperModule)
		return;
	
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::JPEG);

	// Set the raw data
	ImageWrapper->SetRaw(RawPixels.GetData(), RawPixels.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8);

	// Compressed data
	const TArray64<uint8>& CompressedData = ImageWrapper->GetCompressed(85);

	// Create a filename like: Steering_0.5_123.jpg
	FString Filename = CaptureDir / FString::Printf(TEXT("frame_%05d.jpg"), FrameCounter++);

	FFileHelper::SaveArrayToFile(CompressedData, *Filename);
}

void UCameraDataComponent::WriteCSVRow(const FString& Filename, float SteeringAngle)
{
	if (!bCSVInitialised)
		return;

	// Append one row per frame
	FString Row = FString::Printf(
		TEXT("%s,%.4f,%.4f,%.4f,%lld\n"),
		*Filename,
		SteeringAngle,
		0.0f, // throttle placeholder — hook up same as steering
		0.0f, // brake placeholder
		FDateTime::Now().ToUnixTimestamp()
	);

	FFileHelper::SaveStringToFile(
		Row,
		*CSVPath,
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		FILEWRITE_Append  // Append not overwrite
	);
}

void UCameraDataComponent::StartRecording()
{
	if (GetWorld() && !bIsRecording)
	{
		bIsRecording = true;
		GetWorld()->GetTimerManager().SetTimer(
			RecordTimerHandle,
			this,
			&UCameraDataComponent::CaptureFrame,
			0.1f,
			true
		);
		UE_LOG(LogTemp, Warning, TEXT("Recording STARTED"));
	}
}

void UCameraDataComponent::StopRecording()
{
	if (GetWorld() && bIsRecording)
	{
		bIsRecording = false;
		GetWorld()->GetTimerManager().ClearTimer(RecordTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Recording STOPPED. Frames saved: %d"), FrameCounter);
	}
}

