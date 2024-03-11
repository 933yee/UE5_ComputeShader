// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCSTestActor.h"
#include "FlockingComputeShader.h"

AMyCSTestActor::AMyCSTestActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyCSTestActor::BeginPlay()
{
	Super::BeginPlay();
	FFlockingComputeShader::Get().BeginRendering();

	FShaderUsageParameters DrawParameters(VelocityRenderTarget, PositionRenderTarget);
	{
		DrawParameters.Range = Range;
	}
	FFlockingComputeShader::Get().UpdateParameters(DrawParameters);
}

void AMyCSTestActor::BeginDestroy()
{
	FFlockingComputeShader::Get().EndRendering();
	Super::BeginDestroy();
}

void AMyCSTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

