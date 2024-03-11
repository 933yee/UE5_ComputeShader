#include "ComputeShaderExample.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ShaderParameterStruct.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 32


class FComputeShaderExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FComputeShaderExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FComputeShaderExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, VelocityOutputTexture)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D<float4>, VelocityInput)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, PositionOutputTexture)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D<float4>, PositionInput)
		SHADER_PARAMETER(FVector2f, TextureSize) // Metal doesn't support GetDimensions(), so we send in this data via our parameters.
		SHADER_PARAMETER(float, Range)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                            ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FComputeShaderExampleCS, "/FlockingComputeShaders/Private/ComputeShader.usf", "MainComputeShader", SF_Compute);

void FComputeShaderExample::RunComputeShader_RenderThread(
	FRDGBuilder& RDGBuilder,
	const FShaderUsageParameters& DrawParameters,
	FRDGTextureUAVRef VelocityOutpuUAV,
	FRDGTextureRef VelocityInputRT,
	FRDGTextureUAVRef PositionOutputUAV,
	FRDGTextureRef PositionInputRT
)
{
	FComputeShaderExampleCS::FParameters *PassParameters = RDGBuilder.AllocParameters<FComputeShaderExampleCS::FParameters>();;
	PassParameters->VelocityOutputTexture = VelocityOutpuUAV;
	PassParameters->VelocityInput = VelocityInputRT;
	PassParameters->PositionOutputTexture = PositionOutputUAV;
	PassParameters->PositionInput = PositionInputRT;
	PassParameters->TextureSize = FVector2f(DrawParameters.GetRenderTargetSize().X, DrawParameters.GetRenderTargetSize().Y);
	PassParameters->Range = DrawParameters.Range;

	TShaderMapRef<FComputeShaderExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = 
		FIntVector(FMath::DivideAndRoundUp(DrawParameters.GetRenderTargetSize().X, NUM_THREADS_PER_GROUP_DIMENSION), FMath::DivideAndRoundUp(DrawParameters.GetRenderTargetSize().Y, NUM_THREADS_PER_GROUP_DIMENSION), 1);
	FComputeShaderUtils::AddPass(RDGBuilder, RDG_EVENT_NAME("RenderThread"), ERDGPassFlags::Compute | ERDGPassFlags::NeverCull, ComputeShader, PassParameters, GroupCounts);
}
