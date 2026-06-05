// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestCubeSpawner.generated.h"

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

private:
	// 격자 형태로 테스트 큐브를 스폰한다.
	void SpawnTestCubes();

	UPROPERTY()
	TArray<AStaticMeshActor*> SpawnedCubes;

	float ElapsedTime = 0.0f;
};
