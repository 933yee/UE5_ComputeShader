#include "InstancingRootActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"

AInstancingRootActor::AInstancingRootActor()
{
	PrimaryActorTick.bCanEverTick = true;

	InstancedComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("UInstancedStaticMeshComponent"));

	InstancedComponent->NumCustomDataFloats = 1;
}

void AInstancingRootActor::InitInstances(int instatnceNum)
{
	for (int i = 0; i < instatnceNum; i++)
	{
		FTransform t = FTransform();
		InstancedComponent->AddInstance(t);
		InstancedComponent->SetCustomDataValue(i, 0, i);
	}
}

void AInstancingRootActor::BeginPlay()
{
	Super::BeginPlay();
	if (DrawMat != nullptr)
	{
		InstancedComponent->CreateDynamicMaterialInstance(0, DrawMat);
		InitInstances(2000);
	}
}

void AInstancingRootActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
