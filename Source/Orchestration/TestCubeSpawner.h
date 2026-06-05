// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestCubeSpawner.generated.h"

class UStaticMesh;
class UMaterialInterface;
class AStaticMeshActor;

UCLASS()
class ORCHESTRATION_API ATestCubeSpawner : public AActor
{
	GENERATED_BODY()

public:
	ATestCubeSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// --- Grid layout ---------------------------------------------------------

	// Number of rows of cubes to spawn.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Grid", meta = (ClampMin = "1"))
	int32 GridRows = 5;

	// Number of columns of cubes to spawn.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Grid", meta = (ClampMin = "1"))
	int32 GridColumns = 5;

	// Distance between neighbouring cubes (Unreal units).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Grid")
	float GridSpacing = 200.0f;

	// Height above the spawner the cubes rest at (Unreal units).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Grid")
	float SpawnHeight = 50.0f;

	// Uniform scale applied to each cube.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Grid")
	float CubeScale = 0.75f;

	// --- Animation -----------------------------------------------------------

	// Base yaw rotation speed (degrees/second).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Animation")
	float RotationBaseSpeed = 45.0f;

	// Additional yaw speed added per cube index (degrees/second).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Animation")
	float RotationSpeedPerIndex = 3.0f;

	// Vertical bob amplitude (Unreal units).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Animation")
	float BobAmplitude = 20.0f;

	// Vertical bob frequency (radians/second multiplier).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Animation")
	float BobFrequency = 2.0f;

	// Phase offset applied per cube index so they bob out of sync.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Animation")
	float BobPhasePerIndex = 0.5f;

	// Per row/column colour increment used to differentiate cubes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCubeSpawner|Animation")
	float ColorStep = 0.2f;

private:
	// Spawns the configured grid of test cubes.
	void SpawnTestCubes();

	// Assets resolved in the constructor.
	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> CubeMaterial;

	UPROPERTY()
	TArray<TObjectPtr<AStaticMeshActor>> SpawnedCubes;

	// Initial world location of each spawned cube, used as the bob baseline.
	UPROPERTY(Transient)
	TArray<FVector> CubeBaseLocations;

	UPROPERTY(Transient)
	float ElapsedTime = 0.0f;
};
