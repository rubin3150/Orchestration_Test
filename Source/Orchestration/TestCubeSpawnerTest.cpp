// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "TestCubeSpawner.h"

#if WITH_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTestCubeSpawnerDefaultsTest,
	"Orchestration.TestCubeSpawner.Defaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTestCubeSpawnerDefaultsTest::RunTest(const FString& Parameters)
{
	const ATestCubeSpawner* Defaults = GetDefault<ATestCubeSpawner>();
	if (!TestNotNull(TEXT("Default object exists"), Defaults))
	{
		return false;
	}

	// The default configuration should describe a 5x5 grid (25 cubes).
	TestEqual(TEXT("Default grid rows"), Defaults->GridRows, 5);
	TestEqual(TEXT("Default grid columns"), Defaults->GridColumns, 5);
	TestEqual(TEXT("Default cube count"), Defaults->GridRows * Defaults->GridColumns, 25);

	// Layout/animation defaults must stay positive to produce a visible result.
	TestTrue(TEXT("Grid spacing is positive"), Defaults->GridSpacing > 0.0f);
	TestTrue(TEXT("Cube scale is positive"), Defaults->CubeScale > 0.0f);
	TestTrue(TEXT("Bob amplitude is non-negative"), Defaults->BobAmplitude >= 0.0f);

	return true;
}

#endif // WITH_AUTOMATION_TESTS
