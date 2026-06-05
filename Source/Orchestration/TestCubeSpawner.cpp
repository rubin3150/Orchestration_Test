// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestCubeSpawner.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ATestCubeSpawner::ATestCubeSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATestCubeSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnTestCubes();
}

void ATestCubeSpawner::SpawnTestCubes()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	// 엔진 내장 큐브 메시 로드
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UMaterialInterface* CubeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	if (CubeMesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("TestCubeSpawner: Cube mesh load failed"));
		return;
	}

	// 5x5 격자, 200uu 간격으로 25개 스폰
	for (int32 Row = 0; Row < 5; ++Row)
	{
		for (int32 Col = 0; Col < 5; ++Col)
		{
			const FVector Location = GetActorLocation() + FVector(Row * 200.0f, Col * 200.0f, 50.0f);
			const FRotator Rotation = FRotator::ZeroRotator;

			AStaticMeshActor* CubeActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation);
			if (CubeActor == nullptr)
			{
				continue;
			}

			UStaticMeshComponent* MeshComp = CubeActor->GetStaticMeshComponent();
			MeshComp->SetMobility(EComponentMobility::Movable);
			MeshComp->SetStaticMesh(CubeMesh);
			CubeActor->SetActorScale3D(FVector(0.75f, 0.75f, 0.75f));

			// 행/열 인덱스에 따라 색을 살짝 다르게 줘서 구분이 쉽게
			if (CubeMaterial != nullptr)
			{
				UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(CubeMaterial, CubeActor);
				const FLinearColor Color(Row * 0.2f, Col * 0.2f, 0.5f, 1.0f);
				DynMat->SetVectorParameterValue(TEXT("Color"), Color);
				MeshComp->SetMaterial(0, DynMat);
			}

			SpawnedCubes.Add(CubeActor);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("TestCubeSpawner: spawned %d cubes"), SpawnedCubes.Num());
}

void ATestCubeSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	// 스폰된 큐브들을 천천히 회전시켜 살아있는지 확인
	for (int32 i = 0; i < SpawnedCubes.Num(); ++i)
	{
		AStaticMeshActor* Cube = SpawnedCubes[i];
		if (Cube != nullptr)
		{
			const float Speed = 45.0f + i * 3.0f;
			Cube->AddActorLocalRotation(FRotator(0.0f, Speed * DeltaTime, 0.0f));

			// 위아래로 둥둥 떠다니게
			const float Offset = FMath::Sin(ElapsedTime * 2.0f + i * 0.5f) * 20.0f;
			FVector Loc = Cube->GetActorLocation();
			Loc.Z = 50.0f + Offset;
			Cube->SetActorLocation(Loc);
		}
	}
}
