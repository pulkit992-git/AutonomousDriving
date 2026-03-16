// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDataComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TextureResource.h"

UCameraDataComponent::UCameraDataComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraDataComponent::CaptureFrame()
{
	if (!TextureTarget)
		return;

	FTextureRenderTargetResource* RenderTargetResource = TextureTarget->GameThread_GetRenderTargetResource();

	TArray<FColor> RawPixels;
	RenderTargetResource->ReadPixels(RawPixels);

	if (RawPixels.Num() > 0)
	{
		UE_LOG(LogTemp, Display, TEXT ("AI Camera captured %d pixels"), RawPixels.Num());
	}
}
