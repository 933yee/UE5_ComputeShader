// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyCSTestActor.generated.h"

UCLASS()
class MYCOMPUTESHADER_API AMyCSTestActor : public AActor
{
	GENERATED_BODY()

public:
	AMyCSTestActor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputeShaderFlocking)
	class UTextureRenderTarget2D* VelocityRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputeShaderFlocking)
	class UTextureRenderTarget2D* PositionRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputeShaderFlocking)
	float Range;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	virtual void Tick(float DeltaTime) override;

};
