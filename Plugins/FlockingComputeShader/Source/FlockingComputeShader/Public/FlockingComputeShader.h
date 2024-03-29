#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "RenderGraphResources.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"


struct FShaderUsageParameters
{
	UTextureRenderTarget2D* VelocityRenderTarget;
	UTextureRenderTarget2D* PositionRenderTarget;
	float Range;

	FIntPoint GetRenderTargetSize() const
	{
		return CachedRenderTargetSize;
	}

	FShaderUsageParameters() { }
	FShaderUsageParameters(UTextureRenderTarget2D* InRenderTarget, UTextureRenderTarget2D* InRenderTarget2)
		: VelocityRenderTarget(InRenderTarget)
		, PositionRenderTarget(InRenderTarget2)
	{
		CachedRenderTargetSize = VelocityRenderTarget ? FIntPoint(VelocityRenderTarget->SizeX, VelocityRenderTarget->SizeY) : FIntPoint::ZeroValue;
	}

private:
	FIntPoint CachedRenderTargetSize;
};


class FLOCKINGCOMPUTESHADER_API FFlockingComputeShader : public IModuleInterface
{
public:
	static inline FFlockingComputeShader& Get()
	{
		return FModuleManager::LoadModuleChecked<FFlockingComputeShader>("FlockingComputeShader");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FlockingComputeShader");
	}

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	// Call this when you want to hook onto the renderer and start drawing. The shader will be executed once per frame.
	void BeginRendering();

	// When you are done, call this to stop drawing.
	void EndRendering();

	// Call this whenever you have new parameters to share. You could set this up to update different sets of properties at
	// different intervals to save on locking and GPU transfer time.
	void UpdateParameters(FShaderUsageParameters& DrawParameters);

private:
	/*TRefCountPtr<IPooledRenderTarget> VelocityOutput;
	TRefCountPtr<IPooledRenderTarget> PositionOutput;*/
	FShaderUsageParameters CachedShaderUsageParameters;
	FDelegateHandle OnPostResolvedSceneColorHandle;
	FCriticalSection RenderEveryFrameLock;
	FCriticalSection InputLock;
	volatile bool bCachedParametersValid;

	void HandlePreRender_RenderThread(class FRDGBuilder& RDGBuilder);
};
