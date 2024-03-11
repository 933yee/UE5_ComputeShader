#pragma once

#include "CoreMinimal.h"
#include "FlockingComputeShader.h"


class FComputeShaderExample
{
public:
	static void RunComputeShader_RenderThread(
		FRDGBuilder& RDGBuilder, 
		const FShaderUsageParameters& DrawParameters, 
		FRDGTextureUAVRef VelocityOutpuUAV,
		FRDGTextureRef VelocityInputRT,
		FRDGTextureUAVRef PositionOutputUAV,
		FRDGTextureRef PositionInputRT
	);
};
