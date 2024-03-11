#include "FlockingComputeShader.h"

#include "ComputeShaderExample.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "RHI.h"
#include "GlobalShader.h"
#include "RHICommandList.h"
#include "RenderGraphBuilder.h"
#include "RenderTargetPool.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "RenderGraphUtils.h"

IMPLEMENT_MODULE(FFlockingComputeShader, FlockingComputeShader)


void FFlockingComputeShader::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("FlockingComputeShader"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/FlockingComputeShaders"), PluginShaderDir);
}

void FFlockingComputeShader::ShutdownModule()
{
	EndRendering();
}

void FFlockingComputeShader::BeginRendering()
{
	if (!GEngine)
	{
		return;
	}

	bCachedParametersValid = false;
	if (!GEngine->GetPreRenderDelegateEx().IsBoundToObject(this))
	{
		GEngine->GetPreRenderDelegateEx().AddRaw(this, &FFlockingComputeShader::HandlePreRender_RenderThread);
	}
}

void FFlockingComputeShader::EndRendering()
{
	if (!GEngine)
	{
		return;
	}

	GEngine->GetPreRenderDelegateEx().RemoveAll(this);
}

void FFlockingComputeShader::UpdateParameters(FShaderUsageParameters& DrawParameters)
{
	FScopeLock UpdateLock(&InputLock);

	CachedShaderUsageParameters = DrawParameters;
	bCachedParametersValid = true;
}

void FFlockingComputeShader::HandlePreRender_RenderThread(FRDGBuilder& RDGBuilder)
{
	check(IsInRenderingThread());
	if (!bCachedParametersValid)
	{
		return;
	}

	InputLock.Lock();
	FShaderUsageParameters InputParameters = CachedShaderUsageParameters;
	InputLock.Unlock();

	if (!InputParameters.VelocityRenderTarget || !InputParameters.PositionRenderTarget)
	{
		return;
	}

	FRDGTextureDesc ComputeShaderOutputDesc = FRDGTextureDesc::Create2D(
		InputParameters.GetRenderTargetSize(), PF_FloatRGBA, FClearValueBinding::None, ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource);
	FRDGTextureRef VelocityTexture = RDGBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderVelocityOutput"), ERDGTextureFlags::None);
	FRDGTextureRef PositionTexture = RDGBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderPositionOutput"), ERDGTextureFlags::None);
	
	TRefCountPtr<IPooledRenderTarget> VelocityRenderTarget = CreateRenderTarget(InputParameters.VelocityRenderTarget->GetResource()->GetTexture2DRHI(), TEXT("Draw Velocity RenderTarget"));
	TRefCountPtr<IPooledRenderTarget> PositionRenderTarget = CreateRenderTarget(InputParameters.PositionRenderTarget->GetResource()->GetTexture2DRHI(), TEXT("Draw Position RenderTarget"));
	
	FRDGTextureRef VelocityTargetTexture = RDGBuilder.RegisterExternalTexture(VelocityRenderTarget);
	FRDGTextureRef PositionTargetTexture = RDGBuilder.RegisterExternalTexture(PositionRenderTarget);

	FComputeShaderExample::RunComputeShader_RenderThread(
		RDGBuilder, 
		InputParameters, 
		RDGBuilder.CreateUAV(VelocityTexture),
		VelocityTargetTexture,
		RDGBuilder.CreateUAV(PositionTexture),
		PositionTargetTexture
	);

	AddCopyTexturePass(RDGBuilder, VelocityTexture, VelocityTargetTexture, FRHICopyTextureInfo());
	AddCopyTexturePass(RDGBuilder, PositionTexture, PositionTargetTexture, FRHICopyTextureInfo());
}