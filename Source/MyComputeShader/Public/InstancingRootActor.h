// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InstancingRootActor.generated.h"

UCLASS()
class AInstancingRootActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AInstancingRootActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking")
	class UInstancedStaticMeshComponent* InstancedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flocking")
	class UMaterial* DrawMat;

	void InitInstances(int instatnceNum);
};
