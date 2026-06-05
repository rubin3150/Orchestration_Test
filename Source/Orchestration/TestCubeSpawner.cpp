// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestCubeSpawner.h"
#include "Orchestration.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	// Engine built-in assets used to render the test cubes.
	const TCHAR* const CubeMeshPath = TEXT("/Engine/BasicShapes/Cube.Cube");
	const TCHAR* const CubeMaterialPath = TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial");

	// Name of the vector parameter on BasicShapeMaterial used to tint each cube.
	const FName CubeColorParameterName(TEXT("Color"));
}

ATestCubeSpawner::ATestCubeSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	// Resolve engine built-in assets at construction time instead of blocking the
	// game thread with a runtime LoadObject.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(CubeMeshPath);
	if (CubeMeshFinder.Succeeded())
	{
		CubeMesh = CubeMeshFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CubeMaterialFinder(CubeMaterialPath);
	if (CubeMaterialFinder.Succeeded())
	{
		CubeMaterial = CubeMaterialFinder.Object;
	}
}

void ATestCubeSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Only the authority should populate the world to avoid double-spawning on clients.
	if (HasAuthority())
	{
		SpawnTestCubes();
	}

	// Nothing to animate -> save the per-frame cost.
	if (SpawnedCubes.Num() == 0)
	{
		SetActorTickEnabled(false);
	}
}

void ATestCubeSpawner::SpawnTestCubes()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (CubeMesh == nullptr)
	{
		UE_LOG(LogOrchestration, Warning, TEXT("TestCubeSpawner: Cube mesh is not available"));
		return;
	}

	// Re-entry (PIE re-enter, streaming re-spawn) must not accumulate stale entries.
	SpawnedCubes.Reset();
	CubeBaseLocations.Reset();
	SpawnedCubes.Reserve(GridRows * GridColumns);
	CubeBaseLocations.Reserve(GridRows * GridColumns);

	const FVector Origin = GetActorLocation();

	for (int32 Row = 0; Row < GridRows; ++Row)
	{
		for (int32 Col = 0; Col < GridColumns; ++Col)
		{
			const FVector Location = Origin + FVector(Row * GridSpacing, Col * GridSpacing, SpawnHeight);

			AStaticMeshActor* CubeActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator);
			if (CubeActor == nullptr)
			{
				continue;
			}

			UStaticMeshComponent* MeshComp = CubeActor->GetStaticMeshComponent();
			if (MeshComp == nullptr)
			{
				CubeActor->Destroy();
				continue;
			}

			MeshComp->SetMobility(EComponentMobility::Movable);
			MeshComp->SetStaticMesh(CubeMesh);
			CubeActor->SetActorScale3D(FVector(CubeScale));

			// Tint each cube slightly differently so individual cubes are distinguishable.
			if (CubeMaterial != nullptr)
			{
				UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(CubeMaterial, CubeActor);
				if (DynMat != nullptr)
				{
					const FLinearColor Color(Row * ColorStep, Col * ColorStep, 0.5f, 1.0f);
					DynMat->SetVectorParameterValue(CubeColorParameterName, Color);
					MeshComp->SetMaterial(0, DynMat);
				}
			}

			SpawnedCubes.Add(CubeActor);
			CubeBaseLocations.Add(Location);
		}
	}

	UE_LOG(LogOrchestration, Log, TEXT("TestCubeSpawner: spawned %d cubes"), SpawnedCubes.Num());
}

void ATestCubeSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	for (int32 i = 0; i < SpawnedCubes.Num(); ++i)
	{
		AStaticMeshActor* Cube = SpawnedCubes[i];
		if (Cube == nullptr)
		{
			continue;
		}

		const float Speed = RotationBaseSpeed + i * RotationSpeedPerIndex;
		Cube->AddActorLocalRotation(FRotator(0.0f, Speed * DeltaTime, 0.0f));

		// Bob around the cube's original spawn position so off-origin spawners work.
		const float Offset = FMath::Sin(ElapsedTime * BobFrequency + i * BobPhasePerIndex) * BobAmplitude;
		FVector Loc = CubeBaseLocations[i];
		Loc.Z += Offset;
		Cube->SetActorLocation(Loc);
	}
}
