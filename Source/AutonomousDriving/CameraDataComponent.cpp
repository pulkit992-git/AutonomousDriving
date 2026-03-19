// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDataComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TextureResource.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"

UCameraDataComponent::UCameraDataComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraDataComponent::BeginPlay()
{
	Super::BeginPlay();

	//Force the folder to exist
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString TotalPath = FPaths::ProjectSavedDir() / TEXT("Captures");

	if (!PlatformFile.DirectoryExists(*TotalPath))
	{
		PlatformFile.CreateDirectory(*TotalPath);
	}

	//Start the timer (Check if the World exists first)
	if (GetWorld())
	{
		// 0.1f = 10 times per second
		GetWorld()->GetTimerManager().SetTimer(RecordTimerHandle, this, &UCameraDataComponent::CaptureFrame, 0.1f, true);
		UE_LOG(LogTemp, Warning, TEXT("DATA RECORDER STARTED!"));
	}
}

void UCameraDataComponent::CaptureFrame()
{
	if (!TextureTarget)
		return;

	if (!VehicleMovement) {
		AActor* Owner = GetOwner();
		VehicleMovement = Owner->FindComponentByClass<UChaosWheeledVehicleMovementComponent>();
	}

	float CurrentSteering = VehicleMovement ? VehicleMovement->GetSteeringInput() : 0.0f;

	FTextureRenderTargetResource* RenderTargetResource = TextureTarget->GameThread_GetRenderTargetResource();

	TArray<FColor> RawPixels;
	RenderTargetResource->ReadPixels(RawPixels);

	if (RawPixels.Num() > 0)
	{
	//	UE_LOG(LogTemp, Display, TEXT ("AI Camera captured %d pixels"), RawPixels.Num());
		SaveImageToDisk(RawPixels, CurrentSteering);
	}
}

void UCameraDataComponent::SaveImageToDisk(const TArray<FColor>& RawPixels, float SteeringAngle)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	// Set the raw data
	ImageWrapper->SetRaw(RawPixels.GetData(), RawPixels.Num() * sizeof(FColor), 224, 224, ERGBFormat::BGRA, 8);

	// Compressed data
	const TArray64<uint8>& CompressedData = ImageWrapper->GetCompressed(100);

	// Create a filename like: Steering_0.5_123.jpg
	FString Filename = FPaths::ProjectSavedDir() / TEXT("Captures") /
		FString::Printf(TEXT("Steer_%f_Frame_%d.jpg"), SteeringAngle, FrameCounter++);

	FFileHelper::SaveArrayToFile(CompressedData, *Filename);
}
